#include "simulation.h"

/*
 * Typ: 
 * 1: x,y-Sensoren
 * 2: drei Z-Sensoren auf dem Turm
 * 3: Lichtschranken
 * 4: Lichttaster
 */

int towerPositionX = StartPositionX * sensorDistanceX;
int towerPositionY = StartPositionY * sensorDistanceY;
int towerPositionZ = StartPositionZ * sensorDistanceZ;
int TickCountInput = Delay_Time_IO_Slots;
int TickCountOutput = Delay_Time_IO_Slots;

// Belegungsmatrix der Simulation
bool belegungsMatrix[10][5];// Wird noch nicht ausgefuellt

// MessageQueue
#define MSG_Q_MAX_Messages 200 //TODO: kleiner machen
MSG_Q_ID mesgQueueIdSensorCollector;

// Struct for Bus-Communication
typedef struct {
	UINT id :7;
	bool value : 1;
} Sensorresult;

typedef union {
	char charvalue; // Für MessageQueue
	Sensorresult result;
} MessageQSensorresult;

//----------------------------------------------------------

void Simulation_Sensorverwaltung(void);
void Simulation_Sensor(int id);
void Simulation_SensorCollector(void);
bool triggersX(int x);
bool triggersY(int y);
bool triggersZ(int z);


int Simulation_init(void){
	printf("Start: Simulation \n");
	bool abort = false;
	sbusdata temp;
	
	/* create message queue */
	if ((mesgQueueIdSensorCollector = msgQCreate(MSG_Q_MAX_Messages,1,MSG_Q_FIFO))	== NULL){
		printf("msgQCreate in failed\n");
		abort = true;
	}
	else{
		printf("messageQ SensorCollector created\n");
	}
	
	if ((mesgQueueIdSensorData = msgQCreate(3,sizeof(temp.smsg),MSG_Q_FIFO))	== NULL){
		printf("msgQCreate in SensorCollector failed\n");
		abort = true;
	}
	else {
		printf("messageQ-SensorData created\n");
	}
		
	if (abort){
		return (-1);
	}
	else {
		/*create Binary Semaphore*/
		semBinary_SteuerungToSimulation = semBCreate(SEM_Q_FIFO, SEM_FULL);
		printf("Semaphore fuer Simulation <-> Steuerung erstellt \n");
		
		taskSpawn("SensorVerwaltung", Priority_Simulation, 0x100, 2000, (FUNCPTR)Simulation_Sensorverwaltung, 0,0,0,0,0,0,0,0,0,0);
			
		int i;
		for (i=0; i < 26; i++)
		{
			taskSpawn("Sensor",Priority_Simulation, 0x100, 2000, (FUNCPTR)Simulation_Sensor,i,0,0,0,0,0,0,0,0,0);
		}
		taskSpawn("SensorCollector", Priority_Simulation, 0x100, 2000, (FUNCPTR)Simulation_SensorCollector, 0,0,0,0,0,0,0,0,0,0);
		printf("Sensor & Sensorcollector gespawnt \n");
		
		return 0;
	}
	
	
}

void Simulation_Sensorverwaltung(void)
{
	printf("Start: Task - Sensorverwaltung \n");
	abusdata AktorBusData;
	AktorBusData.i=0;
	int direction, inputticks, outputticks;
	while(true)
	{
		//printf("Will bewegen!\n");
		// regelmaessige Abfrage + weitersetzen
		semTake(semBinary_SteuerungToSimulation, WAIT_FOREVER); 
		abusdata AktorBusData = SteuerungToSimulation;
		semGive(semBinary_SteuerungToSimulation);

		//x-Achse
		direction = 0;
		if (AktorBusData.abits.axl)
			direction--;
		if (AktorBusData.abits.axr)
			direction++;
		if (AktorBusData.abits.axs)
			direction = direction * 2;
		towerPositionX += direction;
		if (towerPositionX < 0)
			towerPositionX = 0;
		if (towerPositionX > 9*sensorDistanceX)
			towerPositionX = 9*sensorDistanceX;
		
		//y-Achse
		direction = 0;
		if (AktorBusData.abits.ayu)
		{
			//printf("Fahre nach unten \n");
			direction++;
		}
		if (AktorBusData.abits.ayo)
		{
			//printf("Fahre nach oben \n");
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
		if ( (AktorBusData.abits.aealre == 1) && (AktorBusData.abits.aealra == 0) ){
			TickCountInput--;
		}
		if ( (AktorBusData.abits.aearra == 1) && (AktorBusData.abits.aearre == 0) ){
			TickCountOutput--;
			printf("TickCountOutput: %d\n", TickCountOutput);
		}
		
		taskDelay(Delay_Time_Simulation);
	}
}


void Simulation_Sensor(int id)
{
    int previousX = 0;
    int previousY = 0;
    int previousZ = 0;
    bool firstStepTaken = false;
    bool firstStepOutputTaken = false;
    bool boxOnLichttaster = false;
    
    
    MessageQSensorresult returnValue;
	returnValue.result.id = id;
	int triggerOffset;

	if(id < 10)
	{
		// unten
		if(id < 5)
		{
			triggerOffset = (id*2+1)*sensorDistanceY;
		}
		// oben
		else
		{
			triggerOffset = (id-5)*2*sensorDistanceY;
		}
	}
	// X-Achse
	else if(id < 20)
	{
		triggerOffset = (id-10)*sensorDistanceX;
	}
	// Z-Achse
	else if(id < 23)
	{
		//Z=0: Arm im Regal drinne
		triggerOffset = (id-20)*sensorDistanceZ;
	}
	// Lichtkram
	else
	{
		//TODO:Lichtsensoren
		//returnValue.result.value = 
	}
	printf("Start: Task - Sensor #%d mit offset: %d\n", id, triggerOffset);



	while(1)
	{
		if(id < 10)// Y-Sensoren
		{
			returnValue.result.value = triggersY(triggerOffset);
		}
		else if(id < 20)// X-Achse
		{
			returnValue.result.value = triggersX(triggerOffset);
		}
		else if(id < 23)// Z-Achse
		{	//Z=0: Arm im Regal drinne
			returnValue.result.value = triggersZ(triggerOffset);
		}

		else
		{
            // Lichttaster   Tower
            if(id == 25)
            {
            	// hat sich zum vorigen step nicht bewegt auf x und z achse
            	if(firstStepTaken && (previousX == towerPositionX) && (towerPositionZ == towerPositionZ))
            	{
            		returnValue.result.value = boxOnLichttaster;
            		// Ausgabe
					if(boxOnLichttaster)
					{
						
						// nach unten gefahren ... ablegen?
						if(previousY < towerPositionY)
						{
							// außen -> Ein ausgabe slots
							if(towerPositionZ == 2*sensorDistanceZ)
							{
								returnValue.result.value = false;  
								firstStepTaken = false;	
								TickCountOutput = Delay_Time_IO_Slots;
							}
							// innen -> im Regal
							else if(towerPositionZ == 0)
							{
								// da ist kein päckchen im regal an dieser stelle
								if(!belegungsMatrix[towerPositionX/sensorDistanceX][towerPositionY/sensorDistanceY])
								{
									returnValue.result.value = false;
									firstStepTaken = false;
								}
							}
						}
					}
					// Aufnahme
					else
					{
						// nach oben gefahren ... aufnahme?
						if(previousY > towerPositionY)
						{
							// außen -> Ein ausgabe slots
							if(towerPositionZ == 2*sensorDistanceZ)
							{
								// da ist ein päckchen auf dem eingaslot
								if(TickCountInput <= 0)
								{
									returnValue.result.value = true; 
									firstStepTaken = false;
									TickCountInput = Delay_Time_IO_Slots;
									boxOnLichttaster = true;
								}
							}
							// innen -> im Regal
							else
							{
								// da ist ein päckchen im regal an dieser stelle
								if(belegungsMatrix[towerPositionX/sensorDistanceX][towerPositionY/sensorDistanceY])
								{
									returnValue.result.value = true;
									firstStepTaken = false;
									boxOnLichttaster = true;
								}
							}
						}
					}
            	}
				// first step: von unten rangefahren?
                else if((towerPositionZ == 2*sensorDistanceZ || towerPositionZ == 0) && (towerPositionX != previousX || towerPositionY != previousY || towerPositionZ != previousZ))
				{
					firstStepTaken = true;
					returnValue.result.value = boxOnLichttaster;
				}
                else 
                {
					returnValue.result.value = boxOnLichttaster;
                	firstStepTaken = false;
                }
            }
            //TODO: TickCount(er) müssen noch zurückgesetzt werden
            else if (id == 23){ //Eingabe (links)
            	if (TickCountInput <= 0)
            		returnValue.result.value = true;
            	else 
            		returnValue.result.value = false;
            }
            else if (id == 24){ //Ausgabe (rechts)
				if (TickCountOutput > 0)
					returnValue.result.value = true;
				else 
					returnValue.result.value = false;
			}
		}
        previousX = towerPositionX;
        previousY = towerPositionY;
        previousZ = towerPositionZ;
        
		if((msgQSend(mesgQueueIdSensorCollector,&returnValue.charvalue, 1, WAIT_FOREVER, MSG_PRI_NORMAL)) == ERROR)
			printf("msgQSend in Sensor #%d failed\n", id);			
				
		taskDelay(Delay_Time_Simulation);
	}
    
}

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

void Simulation_SensorCollector(void){
	sbusdata sensorBusData;
	int i;
	MessageQSensorresult ValueToBus;
	
	printf("Start: Task - SensorCollector \n");
	while(1)
	{
		i=0;
		while ( (msgQNumMsgs(mesgQueueIdSensorCollector)>0) && i<26)
		{
			i++;
			if(msgQReceive(mesgQueueIdSensorCollector, &ValueToBus.charvalue, 1, WAIT_FOREVER) == ERROR)
				printf("msgQReceive in SensorCollector failed\n");
			else
			{
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
		
		if((msgQSend(mesgQueueIdSensorData, sensorBusData.smsg, sizeof(sensorBusData.smsg), NO_WAIT, MSG_PRI_NORMAL)) == ERROR)
			if(!(msgQNumMsgs(mesgQueueIdSensorData) > 2)) 
				printf("msgQSend in SensorCollector failed\n");		
		
		taskDelay(Delay_Time_Simulation);
	}
}
