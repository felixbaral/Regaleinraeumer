#include "simulation.h"

/*
 * Typ: 
 * 1: x,y-Sensoren
 * 2: drei Z-Sensoren auf dem Turm
 * 3: Lichtschranken
 * 4: Lichttaster
 */
// Initialisierung der aktuellen Position durch Startparameter
int towerPositionX = StartPositionX * sensorDistanceX;
int towerPositionY = StartPositionY * sensorDistanceY;
int towerPositionZ = StartPositionZ * sensorDistanceZ;
// und der Ein- und Ausgabe-Slots
int TickCountInput = Delay_Time_IO_Slots;
int TickCountOutput = Delay_Time_IO_Slots;

// Belegungsmatrix der Simulation
bool belegungsMatrix[10][5];// Wird noch nicht ausgefuellt

// MessageQueue
#define MSG_Q_MAX_Messages 200
MSG_Q_ID mesgQueueIdSensorCollector;

// Struct for Bus-Kommunikation
typedef struct {
	UINT id :7;
	bool value : 1;
} Sensorresult;

// Für MessageQueue
typedef union {
	char charvalue; 
	Sensorresult result;
} SensorResultUnion;

//----------------------------------------------------------

void Simulation_Beweger(void);
void Simulation_Sensor(int id);
void Simulation_SensorCollector(void);
// In Sensor-Tasks aufgerufene Funktionen, die informieren ob der jeweile Sensor ausgelöst oder nicht
bool triggersX(int x);
bool triggersY(int y);
bool triggersZ(int z);


int Simulation_init(void){
	printf("Start: Simulation \n");
	bool abort = false;
	sbusdata temp;	// nur für Größe der zu übertragenden Daten in msgQ
	int i;
	
	// Erzeugung der msgQ's
	if ((mesgQueueIdSensorCollector = msgQCreate(MSG_Q_MAX_Messages,1,MSG_Q_FIFO))	== NULL){
		printf("msgQCreate in failed\n");
		abort = true;
	}	
	if ((mesgQueueIdSensorData = msgQCreate(3,sizeof(temp.smsg),MSG_Q_FIFO))	== NULL){
		printf("msgQCreate in SensorCollector failed\n");
		abort = true;
	}
	
	if (abort){
		return (-1);
	}
	else {
		// Erzeuge Binär-Semaphore mit Mutual-Exclusion zur Prioritäts-Inversion (Wartezustand vermeiden)
		semBinary_SteuerungToSimulation = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE);
		// printf("Semaphore fuer Simulation <-> Steuerung erstellt \n");
		
		// Task-Spawn von Sensor-Beweger, Sensor 0-25 und Sensor-Collector
		taskSpawn("SensorBeweger", Priority_Simulation, 0x100, 2000, (FUNCPTR)Simulation_Beweger, 0,0,0,0,0,0,0,0,0,0);
		char sensorname[20];
		for (i=0; i < 26; i++)
		{
			sprintf(sensorname, "Sensor %d", i);
			taskSpawn(sensorname,Priority_Simulation, 0x100, 2000, (FUNCPTR)Simulation_Sensor,i,0,0,0,0,0,0,0,0,0);
		}
		taskSpawn("SensorCollector", Priority_Simulation, 0x100, 2000, (FUNCPTR)Simulation_SensorCollector, 0,0,0,0,0,0,0,0,0,0);
		printf("Sensor & Sensorcollector gespawnt \n");
		
		return 0;
	}
	
	
}

/* 	
 * 	Bekommt kontinuierlich Aktor-Daten und schiebt die Positionen virtuell weiter
 *  um einzelnen Sensoren Grundlage für Zustandsabfrage zu bieten
 */
void Simulation_Beweger(void)
{
	printf("Start: Task - Sensorverwaltung \n");
	abusdata AktorBusData;			// regelmäßig auszulesende Daten
	AktorBusData.i=0;				// Initialisierung alle Motoren auf inaktiven Zustand
	int direction;					// Richtungserfassung des Motors
	int inputticks, outputticks;	// Zeitkonstanten für Ein- und Ausgabe-Slot zum füllen bzw. leeren
	while(true)
	{
		// regelmaessige Abfrage der Aktordaten
		semTake(semBinary_SteuerungToSimulation, WAIT_FOREVER); 
		abusdata AktorBusData = SteuerungToSimulation;
		semGive(semBinary_SteuerungToSimulation);
		
		// virtuelles Weitersetzen je nach Aktoraktivität
		//x-Achse
		direction = 0;
		if (AktorBusData.abits.axl)	// doppelte Aktivierung in beiden Richtung wird mit Stillstand abgefangen
			direction--;
		if (AktorBusData.abits.axr)
			direction++;
		if (AktorBusData.abits.axs)
			direction = direction * 2;
		towerPositionX += direction;
		// Abfangen von eventuellem Überlauf
		if (towerPositionX < 0)
			towerPositionX = 0;
		if (towerPositionX > 9*sensorDistanceX)
			towerPositionX = 9*sensorDistanceX;
		
		//y-Achse
		direction = 0;
		if (AktorBusData.abits.ayu)
		{
			direction++;
		}
		if (AktorBusData.abits.ayo)
		{
			direction--;
		}
		towerPositionY += direction;
		if (towerPositionY < 0)
			towerPositionY = 0;
		if (towerPositionY > 9*sensorDistanceY)
			towerPositionY = 9*sensorDistanceY;
		
		//z-Achse
		direction = 0;
		if (AktorBusData.abits.azv)
		{
			direction++;
		}
		if (AktorBusData.abits.azh)
		{
			direction--;
		}
		towerPositionZ += direction;
		if (towerPositionZ < 0)
			towerPositionZ = 0;
		if (towerPositionZ > 2*sensorDistanceZ)
			towerPositionZ = 2*sensorDistanceZ;
		
		// Ein- & Ausgabe-Slot
		if ( (AktorBusData.abits.aealre == 1) && (AktorBusData.abits.aealra == 0) ){	// Fließband rein bewegen
			TickCountInput--;
		}
		if ( (AktorBusData.abits.aearra == 1) && (AktorBusData.abits.aearre == 0) ){	// Fließband raus bewegen
			TickCountOutput--;
		}
		
		/* 
		 * Dieser Task hat höchste Priorität, daher muss er delayed werden um andere dran kommen zu lassen,
		 * außerdem Frequenz-Takt der Hardware
		 */
		taskDelay(Delay_Time_Simulation);
	}
}

/*
 * 26 Task-Instanzen dieser Funktion
 * Durchnummeriert mit Id
 * Nummerierung wie in Busdata.h
 */

void Simulation_Sensor(int id)
{
	// Bewegungsmustererkennung für Lichtsensorik
    int previousX = 0;				
    int previousY = 0;
    int previousZ = 0;
    bool firstStepTaken = false;
    bool secondStepTaken = false;

    bool boxOnLichttaster = false;		// Zustand des Lichttasters
    SensorResultUnion returnValue;		// Format des von dem einzelnen Sensors in die msgQ geschriebenen Wertes
    returnValue.result.id = id;			// Id des jeweiligen Sensors wird gespeichert
	
	int triggerOffset = 0;					// Variable der absoluten Position eines Sensor pro Achse mit Betrachtung der Zwischenabstände zwischen 2 Sensoren

	// Berechnung des triggerOffsets
	
	if(id < 10)							// Y-Achse
	{
		if(id < 5)	// unten
		{
			triggerOffset = (id*2+1)*sensorDistanceY;
		}
		else		// oben
		{
			triggerOffset = (id-5)*2*sensorDistanceY;
		}
	}
	else if(id < 20)					// X-Achse
	{
		triggerOffset = (id-10)*sensorDistanceX;
	}
	else if(id < 23)					// Z-Achse
	{
		//Z=0: Arm in Regal herein
		triggerOffset = (id-20)*sensorDistanceZ;
	}
	printf("Start: Task - Sensor #%d mit offset: %d\n", id, triggerOffset);
	// Ende der Triggeroffset-Berechnung	
	
	//----------------------------------------------------------------------------------------------------
	
	// Beginn des Arbeits-Loops
	while(1)
	{	//Unterscheidung nach Sensor-ID
		if(id < 10)// Y-Sensoren
		{
			returnValue.result.value = triggersY(triggerOffset);
		}
		else if(id < 20)// X-Achse
		{
			returnValue.result.value = triggersX(triggerOffset);
		}
		else if(id < 23)// Z-Achse
		{	//Z=0: Arm in Regal herein
			returnValue.result.value = triggersZ(triggerOffset);
		}

		else
		{
            if(id == 25)	// Lichttaster Tower
            {
            	// hat sich zum vorherigen Schritt nicht auf X- und Z-Achse bewegt
            	if(firstStepTaken && (previousX == towerPositionX) && (towerPositionZ == towerPositionZ))
            	{
            		returnValue.result.value = boxOnLichttaster;
            		// Ausgabe
					if(boxOnLichttaster)
					{
						// Bewegungsmuster nach unten -> Ablegungsmodus
						if(previousY < towerPositionY)
						{
							if(towerPositionZ == 2*sensorDistanceZ)	// außen -> Ein-/Ausgabe-Slots
							{
								returnValue.result.value = false;  
								firstStepTaken = false;	
								secondStepTaken = true;
								TickCountOutput = Delay_Time_IO_Slots;
								boxOnLichttaster = false;
								//printf("Box wurde von Lichttaster genommen\n");
							}
							else if(towerPositionZ == 0)	// innen -> im Regal
							{
								// es ist kein Klötzchen im Regal an dieser Stelle
								if(!belegungsMatrix[towerPositionX/sensorDistanceX][towerPositionY/(sensorDistanceY*2)])
								{
									returnValue.result.value = false;
									firstStepTaken = false;
									secondStepTaken = true;
									boxOnLichttaster = false;
									//printf("Box wurde von Lichttaster genommen\n");
								}
							}
						}
					}
					// Aufnahme
					else
					{
	            		// Bewegungsmuster nach oben -> Aufnahmemodus
						if(previousY > towerPositionY)
						{	
							if(towerPositionZ == 2*sensorDistanceZ)	// außen -> Ein-/Ausgabe-Slots
							{
								if(TickCountInput <= 0)// es ist ein Klötzchen auf dem Eingabe-Slot
								{
									returnValue.result.value = true; 
									firstStepTaken = false;
									secondStepTaken = true;
									TickCountInput = Delay_Time_IO_Slots;
									boxOnLichttaster = true;
									//printf("Box wurde auf Lichttaster gelegt\n");
								}
							}
							else	// innen -> im Regal
							{
								if(belegungsMatrix[towerPositionX/sensorDistanceX][towerPositionY/(sensorDistanceY*2)])	// ist ein Klötzchen im Regal an dieser Stelle?
								{
									returnValue.result.value = true;
									firstStepTaken = false;
									secondStepTaken = true;
									boxOnLichttaster = true;
									//printf("Box wurde auf Lichttaster gelegt\n");
								}
							}
						}
					}
            	}

                else if((towerPositionZ == 2*sensorDistanceZ) || (towerPositionZ == 0))	// erster Schritt: Z-Position auf einer Außenseite?   
				{
					firstStepTaken = true;
					returnValue.result.value = boxOnLichttaster;
	            	//printf("habe ersten schritt genommen -----  \n");
				}
                else //reset 
                {
					returnValue.result.value = boxOnLichttaster;
                	firstStepTaken = false;
                	secondStepTaken = false;
                }
            	
            	// Position abspeichern für Bewegungsmuster
				previousX = towerPositionX;
				previousY = towerPositionY;
				previousZ = towerPositionZ;
            }
            else if (id == 23){ 			// Eingabe (links)
            	if (TickCountInput <= 0)	// Muss noch Zeit vergehen bis Eingabe voll ist
            		returnValue.result.value = true;
            	else 
            		returnValue.result.value = false;
            }
            else if (id == 24){ 			// Ausgabe (rechts)
				if (TickCountOutput > 0)	// Muss noch Zeit vergehen bis Ausgabe leer ist
					returnValue.result.value = true;
				else 
					returnValue.result.value = false;
			}
		}
        
        // Daten in msgQ an Sensorcollector senden
		if((msgQSend(mesgQueueIdSensorCollector,&returnValue.charvalue, 1, NO_WAIT, MSG_PRI_NORMAL)) == ERROR)
			printf("msgQSend in Sensor #%d failed\n", id);			
				
		taskDelay(Delay_Time_Simulation);
	}
    
}

/* 
 * In Sensor-Tasks aufgerufene Funktionen, die informieren ob der jeweile Sensor ausgelöst oder nicht
 * Vegleich: Virtueller- mit Sensor-Position
 */
bool triggersX(int x)
{
	if(towerPositionX == x) return true;
	return false;
}

bool triggersY(int y)
{
	if(towerPositionY == y) return true;
	return false;
}

bool triggersZ(int z)
{
	if(towerPositionZ == z) return true;
	return false;
}

/*
 * Sammeln der einzelnen Sensordaten und Zusammenfassen
 * anschließende Übertragung über msgQ an HRL_Steuerung
 */
void Simulation_SensorCollector(void){
	sbusdata sensorBusData;			// Füllvariable Collector -> Steuerung
	int i;							// Zählvariable
	SensorResultUnion ValueToBus;	// Füllvariable Sensor -> Collector
	
	printf("Start: Task - SensorCollector \n");
	while(1)
	{
		i=0;
		while ( (msgQNumMsgs(mesgQueueIdSensorCollector)>0) && i<26)		// Abzählen der 26 Sensoren
		{
			i++;
			if(msgQReceive(mesgQueueIdSensorCollector, &ValueToBus.charvalue, 1, WAIT_FOREVER) == ERROR)
				printf("msgQReceive in SensorCollector failed\n");
			else
			{
				// Invertierte Logik für Busdaten
				if (ValueToBus.result.value){
					//printf("Sensor #%d ist 	++ aktiv ++\n",ValueToBus.result.id);
					sensorBusData.l &= ~(1<<ValueToBus.result.id);
					
				}
				else {
					//printf("Sensor #%d ist 	-- passiv --\n",ValueToBus.result.id);
					sensorBusData.l |= (1<<ValueToBus.result.id);
				}	
			}
		}
		
		// Senden in msgQ an HRL-Steuerung
		if((msgQSend(mesgQueueIdSensorData, sensorBusData.smsg, sizeof(sensorBusData.smsg), NO_WAIT, MSG_PRI_NORMAL)) == ERROR)
			if(!(msgQNumMsgs(mesgQueueIdSensorData) > 2)) 
				printf("msgQSend in SensorCollector failed\n");		
		
		taskDelay(Delay_Time_Simulation);
	}
}
