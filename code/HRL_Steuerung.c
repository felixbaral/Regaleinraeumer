#include "HRL_Steuerung.h"

/*
 * HRL_Steuerung.c
 *
 *  Created on: 27.06.2016
 *      Author: Zauberer
 */

// defines für Zustände für weniger Logik-Verwirrung
#define DontCare (-1)
#define Z_IO 2
#define Z_Middle 1
#define Z_Inside 0
#define Carry_Get 1
#define Carry_Give 0
#define IO_GetBreak 1
#define IO_GetFree 0

// Speicherung virtueller Positionen
int lastSensorX;
int lastSensorY;
int lastSensorZ;
int lastOutputState;
int lastInputState;
int lastCarryState;

// Struktur für Kommandos
typedef struct {
	int x;
	int y;
	int z;
	int IO;
	int carry;
	int allocation;
} NextMovement;
typedef union{
	NextMovement move;
	char charvalue[sizeof(NextMovement)];
}NextMovementUNION;

// msgQ Id's
MSG_Q_ID mesgQueueIdNextMovement;		// Bewegungswarteschlange für einzelne Jobs
MSG_Q_ID mesgQueueIdAktorDataPush;		// Aktordatenwarteschlange für asynchronen Push

void HRL_Steuerung_AktorDataPush();
void HRL_Steuerung_Movement();
void HRL_Steuerung_Movement_GetSensorBusData();
void HRL_Steuerung_Movement_GetSensorBusData_ERROR(char* msg);
void HRL_Steuerung_GetNewJob();
NextMovement HRL_Steuerung_GetNewJob_DontCareState();
void HRL_Steuerung_GetNewJob_Qsend();

//-----------------------------------------------------------------------------------------

int HRL_Steuerung_init(){
	printf("Start: HRL-Steuerung \n");
	bool abort = false; //abbruch für Fehlschlag bei msgQ-Erstellung
	
	// msgQ für Bewgungsabläufe (aus Jobs generiert)
	if ((mesgQueueIdNextMovement = msgQCreate(10,sizeof(NextMovement),MSG_Q_FIFO))	== NULL){
		printf("msgQCreate (NextMovement) in HRL_Steuerung_init failed\n");
		abort = true;
	}
	
	// msgQ für Kommandos (User-Input)
	if ((mesgQueueIdCmd = msgQCreate(MSG_Q_CMD_MAX_Messages,sizeof(commandbits),MSG_Q_PRIORITY))	== NULL){
		printf("msgQCreate (CMD) in HRL_Steuerung_init failed\n");
		abort = true;
	}
	
	// msgQ die an AktorDataPush übergeben werden
	if ( (mesgQueueIdAktorDataPush = msgQCreate(2, sizeof(Aktorbits), MSG_Q_FIFO)) == NULL){
		printf("msgQCreate (AktorDataPush) in HRL_Steuerung_init failed\n");
		abort = true;
	}
	
	if (abort){
		return (-1);
	}
	else{
		// erstelle Task für Aktor-Daten und Stuerung
		taskSpawn("HRL_Steuerung_AktorDataPush",Priority_HRL_Steuerung,0x100,2000,(FUNCPTR)HRL_Steuerung_AktorDataPush,0,0,0,0,0,0,0,0,0,0);
		taskSpawn("HRL_Steuerung_Movement",Priority_HRL_Steuerung,0x100,2000,(FUNCPTR)HRL_Steuerung_Movement,0,0,0,0,0,0,0,0,0,0);
				
		return 0;
	}
}

/*
 * Aktor-Daten werden durch globale Variable übergeben
 * davor Pufferung in msgQ
 */
void HRL_Steuerung_AktorDataPush(){
	abusdata transmit;
	while (1){
		if(msgQReceive(mesgQueueIdAktorDataPush, transmit.amsg, sizeof(transmit.amsg), WAIT_FOREVER) == ERROR) 
			printf("msgQReceive in HRL_Steuerung_AktorDataPush failed\n");
		else{
			semTake(semBinary_SteuerungToSimulation, WAIT_FOREVER); 
			SteuerungToSimulation = transmit;
			semGive(semBinary_SteuerungToSimulation);
		}
	}
}

/*
 * Steuerung holt sich die nächsten Movement-Ziele und steuert die Aktoren, entsprechend die Ziele zu erreichen
 */
void HRL_Steuerung_Movement(){
	NextMovementUNION nextmove; 	// Ziel der nächsten Bewegungs-Phase
	bool waitForSensor;				// Schleifen-Abbruch falls Ziel einer Phase noch nicht errreicht ist
	abusdata aktorData;				// Aktor-Daten für Übertragung am Zyklusende
	
	while(1){
		// auf einen neuen Auftrag warten
		HRL_Steuerung_GetNewJob(); 	//auto pause (blockiert diesen Task, falls es nichts zu tun gibt)
		
		while(msgQNumMsgs(mesgQueueIdNextMovement) > 0){ //wenn es einen neuen Auftrag gibt, so werden alle Movement-Ziele der Reihe nach abgearbeitet
			
			//printf("i got %d job(s) to move my ass\n", msgQNumMsgs(mesgQueueIdNextMovement));
			if(msgQReceive(mesgQueueIdNextMovement, nextmove.charvalue, sizeof(nextmove.charvalue), NO_WAIT) == ERROR)
				printf("msgQ (NextMove) Receive in HRL_Steuerung_Movement failed\n");	
			
			waitForSensor = true; 							// reset Schleifen-Abbruch vor neuem Schleifendurchlauf 
			while (waitForSensor){
				HRL_Steuerung_Movement_GetSensorBusData(); 	// warte auf neue Sensor-Daten und verarbeite diese
				aktorData.i=0; 								// alle Motoren aus			
				
				waitForSensor = false;						// warten abschalten (falls etwas nicht stimmt, wird es aktiviert)
				
				if (nextmove.move.allocation != DontCare){	// Belegungsänderung-Auftrag prüfen
					belegungsMatrix[lastSensorX][lastSensorY/2]=nextmove.move.allocation; 
					nextmove.move.allocation = DontCare;	// Auftrag später nicht nochmal durchführen
				}
				
				//  X
				if ( (lastSensorX != nextmove.move.x) && (nextmove.move.x != DontCare) ){ 		// Stimmt der X-Wert noch nicht und soll er auch
					waitForSensor = true;
					
					if ( ((lastSensorX-nextmove.move.x)*(lastSensorX-nextmove.move.x)) >= 4)	
						aktorData.abits.axs = 1;
					else
						aktorData.abits.axs = 0;
					
					if (lastSensorX < nextmove.move.x){
						aktorData.abits.axr = 1;
						aktorData.abits.axl = 0;
					}
					else{ 
						aktorData.abits.axr = 0;
						aktorData.abits.axl = 1;
					}
											
				}
				
				//  Y
				if ( (lastSensorY != nextmove.move.y) && (nextmove.move.y != DontCare) ){
					waitForSensor = true;
					if (lastSensorY < nextmove.move.y){
						aktorData.abits.ayu = 1;
						aktorData.abits.ayo = 0;
					}
					else{
						aktorData.abits.ayu = 0;
						aktorData.abits.ayo = 1;						
					}						
				}
				
				
				//  Z
				if ( (lastSensorZ != nextmove.move.z) && (nextmove.move.z != DontCare) ){
					waitForSensor = true;
					if (lastSensorZ < nextmove.move.z){
						aktorData.abits.azv = 1;
						aktorData.abits.azh = 0;
					}
					else{
						aktorData.abits.azv = 0;
						aktorData.abits.azh = 1;			
					}
				}
				
				//  Licht
				// Lichtschranke
				if (nextmove.move.IO != DontCare){
					if ( (lastSensorX == PositionXinput) ){ //wir sind beim Input
						if( (nextmove.move.IO == IO_GetBreak)  && (lastInputState != IO_GetBreak) ){
							aktorData.abits.aealre=1;
							aktorData.abits.aealra=0;
							//printf("Input-Slot soll voll werden \n");
							waitForSensor = true;
						}
					}
					else if ( (lastSensorX == PositionXoutput) ){ //wir sind beim Output
						//printf("lastOutputState: %d \n", lastOutputState);
						//printf("nextMoveIO: %d \n", nextmove.move.IO);
						if( (nextmove.move.IO == IO_GetFree) && (lastOutputState !=  IO_GetFree) ) {
							aktorData.abits.aearra=1;
							aktorData.abits.aearre=0;
							//printf("Output-Slot soll leer werden \n");
							waitForSensor = true;
						}
					}
				}//end Lichtschranke
				
				
				// Licht-Taster
				if ( (lastCarryState != nextmove.move.carry) && (nextmove.move.carry != DontCare) ){
					if ( (nextmove.move.carry == Carry_Get) && (!waitForSensor) ) {
						aktorData.abits.ayu = 0;
						aktorData.abits.ayo = 1;
					}
					if ( (nextmove.move.carry == Carry_Give) && (!waitForSensor) ) {
						aktorData.abits.ayu = 1;
						aktorData.abits.ayo = 0;
					}
					waitForSensor = true;
				}
				
				
								//sende noch Daten
				if((msgQSend(mesgQueueIdAktorDataPush, aktorData.amsg, sizeof(aktorData.amsg), WAIT_FOREVER, MSG_PRI_NORMAL)) == ERROR)
					printf("msgQSend to AktorDataPush failed\n");			
			}//end waitForSensor
		}//MovementQ
	}//while(1)	
}


void HRL_Steuerung_Movement_GetSensorBusData(){
	sbusdata returnvalue;
	UIdataUnion visu;
	static unsigned long int oldData;
	int i, errorcount;
	if(msgQReceive(mesgQueueIdSensorData, returnvalue.smsg, sizeof(returnvalue.smsg), WAIT_FOREVER) == ERROR){ 
		printf("msgQReceive in HRL_Steuerung_Movement_GetSensorBusData failed\n");
		returnvalue.l = 0;
	}
	else{
		// X-Achse
		errorcount= (-1);
		for (i = 0; i < 10; i++) {
			if ( (returnvalue.l & (1<<(i+10)) ) == 0)
			{
				lastSensorX = i;
				errorcount++;
				//printf("X sensor ausgelöst: %d %d\n", i,errorcount);
			}
		}
		if (errorcount>0){
			HRL_Steuerung_Movement_GetSensorBusData_ERROR("mehrere X-Sensoren ausgelöst");
		}
		
		// Y-Achse
		errorcount= (-1);
		//unten
		for (i = 0; i < 5; i++) {
			if ( (returnvalue.l & (1<<i) ) == 0)
			{
				lastSensorY = i*2+1;
				errorcount++;
			}
		}
		//oben
		for (i = 0; i < 5; i++) {
			if ( (returnvalue.l & (1<<(i+5)) ) == 0)
			{
				lastSensorY = i*2;
				errorcount++;
			}
		}
		if (errorcount>0){
			HRL_Steuerung_Movement_GetSensorBusData_ERROR("mehrere Y-Sensoren ausgelöst");
		}
		
		// Z-Achse
		errorcount= (-1);
		for (i = 0; i < 3; i++) {
			if ( (returnvalue.l & (1<<(i+20)) ) == 0)
			{
				lastSensorZ = i;
				errorcount++;
			}
		}
		if (errorcount>0){
			HRL_Steuerung_Movement_GetSensorBusData_ERROR("mehrere Z-Sensoren ausgelöst");
		}
		
		//licht
		if (!returnvalue.sbits.lL){
			lastInputState = 1;
		}
		else 
			lastInputState = 0;
		
		if (!returnvalue.sbits.lR){
			lastOutputState = 1;
		}
		else 
			lastOutputState = 0;
				
		if (!returnvalue.sbits.lT){
			lastCarryState = 1;
		}
		else 
			lastCarryState = 0;
				
		
		if (returnvalue.l != oldData ){ //Flimmer Reduzierung
			oldData = returnvalue.l;
		
			// Daten an Visualisierung senden
			visu.data.towerX=lastSensorX;
			visu.data.towerY=lastSensorY;
			visu.data.towerZ=lastSensorZ;
			visu.data.carry = lastCarryState;
			visu.data.input = lastInputState;
			visu.data.output = lastOutputState;
			memcpy(visu.data.matrix, belegungsMatrix, sizeof(belegungsMatrix));
			
			if((msgQSend(msgQvisualisierung, visu.charvalue, sizeof(visu.charvalue), NO_WAIT, MSG_PRI_NORMAL)) == ERROR)
				printf("msgQSend Visualisierung übertragen: übersprungen\n");			
		}
	}
}

void HRL_Steuerung_Movement_GetSensorBusData_ERROR(char* msg){
	printf("Sensoric ERROR: %s \nSystem Stop\n", msg);
	abusdata transmit;
	transmit.i=0;
	msgQReceive(mesgQueueIdAktorDataPush, transmit.amsg, sizeof(transmit.amsg), WAIT_FOREVER);
	taskDelete(taskIdSelf());
}

void HRL_Steuerung_GetNewJob()
{
	UIdataUnion visu;
	NextMovement next3D;
	cmdQdata cmdData;
	static int integrityCheckCounter;
	static int pause;
	int timeout[2] = {350, WAIT_FOREVER};
	int i,j;
	bool foundFreeSpace;
	//printf("Go and get a new Job, Bitch!\n");
	if(msgQReceive(mesgQueueIdCmd, cmdData.charvalue, sizeof(cmdData.charvalue), timeout[pause]) == ERROR) {//etwa 5 Sekunden Timeout
		if( msgQNumMsgs(mesgQueueIdCmd) == 0){
			//printf("Searching Job! %d\n", zusatz);
			if (!zusatz){
				pause = 1;
				// Pause-Modus einleiten - Timeout
				pause = true;
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.x = PositionXinput;
				next3D.y = PositionYinput+1;
				HRL_Steuerung_GetNewJob_Qsend(next3D);
			}
			else{
				cmdData.bits.highprio=0;
				cmdData.bits.cmd=1;
				for (j=0; j<(towerHeight); j++){
					for (i=0; i<towerWidth; i++){
						if (!belegungsMatrix[i][j]){
							cmdData.bits.x=i;
							cmdData.bits.y=j;
							if((msgQSend(mesgQueueIdCmd,cmdData.charvalue,  sizeof(cmdData.charvalue), NO_WAIT, MSG_PRI_NORMAL)) == ERROR)
								printf("msgQSend Zusatztask funktioniert nicht\n");	
							printf("foudn zusatzjob\n");
							return;
						}
					}
				}
			}
		}
	}
	else{
		pause = 0;
		integrityCheckCounter--;
		if (integrityCheckCounter < -5){ //Überlauf verhindern
			integrityCheckCounter = -1;
		}
		if (cmdData.bits.highprio){
			integrityCheckCounter = msgQNumMsgs(mesgQueueIdCmd);
			belegungsMatrix[cmdData.bits.x][cmdData.bits.y] = cmdData.bits.cmd;
			
			visu.data.towerX=lastSensorX;
			visu.data.towerY=lastSensorY;
			visu.data.towerZ=lastSensorZ;
			visu.data.carry = lastCarryState;
			visu.data.input = lastInputState;
			visu.data.output = lastOutputState;
			memcpy(visu.data.matrix, belegungsMatrix, sizeof(belegungsMatrix));
			if((msgQSend(msgQvisualisierung, visu.charvalue, sizeof(visu.charvalue), NO_WAIT, MSG_PRI_NORMAL)) == ERROR)
				printf("msgQSend Visualisierung übertragen: übersprungen\n");	

			
		}else if (cmdData.bits.cmd){//insert xy
			//printf("insert Job erkannt");
			if ( (integrityCheckCounter >= 0) && (belegungsMatrix[cmdData.bits.x][cmdData.bits.y] == true) ){
				printf("ungueltiger insert-Befehl entfernt\n");
			}
			else{
				// 1 - zum Einlader
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.x = PositionXinput;
				next3D.y = PositionYinput+1;
				HRL_Steuerung_GetNewJob_Qsend(next3D);
				
				// 2 - Arm ausfahren und auf Päckchen warten
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.z = Z_IO;
				next3D.IO = IO_GetBreak;
				HRL_Steuerung_GetNewJob_Qsend(next3D);
				
				// 3 - Päckchen auf Arm fahren
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.carry = Carry_Get;
				next3D.IO = IO_GetFree;
				next3D.y = PositionYinput-1; 
				HRL_Steuerung_GetNewJob_Qsend(next3D);
				
				// 4 - Arm einfahren
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.z = Z_Middle;
				HRL_Steuerung_GetNewJob_Qsend(next3D);
				
				// 5 - zu Einlagerungsstelle fahren (y-oben)
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.x = cmdData.bits.x;
				next3D.y = cmdData.bits.y*2;
				HRL_Steuerung_GetNewJob_Qsend(next3D);
				
				// 6 - Arm ausfahren
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.z = Z_Inside;
				HRL_Steuerung_GetNewJob_Qsend(next3D);
				
				// 7 - in Box absenken (y-unten)
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.y = cmdData.bits.y*2+1;
				HRL_Steuerung_GetNewJob_Qsend(next3D);
				
				// 8 - Arm einfahren
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.z = Z_Middle;
				next3D.allocation = 1;
				HRL_Steuerung_GetNewJob_Qsend(next3D);	
			}
		}else{//remove xy
			//printf("remove Job erkannt");
			if ( (integrityCheckCounter >= 0) && (belegungsMatrix[cmdData.bits.x][cmdData.bits.y] == false) ){
				printf("ungueltiger output-Befehl entfernt\n");
			}
			else{
				// 1 - zu Einlagerungsstelle fahren (y-unten)
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.x = cmdData.bits.x;
				next3D.y = cmdData.bits.y*2+1;
				HRL_Steuerung_GetNewJob_Qsend(next3D);
				
				// 2 - Arm ausfahren
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.z = Z_Inside;
				HRL_Steuerung_GetNewJob_Qsend(next3D);
				
				// 3 - in Box heben (y-oben)
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.y = cmdData.bits.y*2;
				HRL_Steuerung_GetNewJob_Qsend(next3D);
				
				// 4 - Arm einfahren
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.z = Z_Middle;
				next3D.allocation = 0;
				HRL_Steuerung_GetNewJob_Qsend(next3D);
				
				// 5 - zum Auslader
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.x = PositionXoutput;
				next3D.y = PositionYoutput-1;
				HRL_Steuerung_GetNewJob_Qsend(next3D);
				
				// 6 - Arm ausfahren und auf freien Slot warten
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.z = Z_IO;
				next3D.IO = IO_GetFree;
				HRL_Steuerung_GetNewJob_Qsend(next3D);
				
				// 7 - Päckchen von Arm fahren
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.carry = Carry_Give;
				next3D.IO = IO_GetBreak;
				next3D.y = PositionYoutput+1;
				HRL_Steuerung_GetNewJob_Qsend(next3D);
				
				// 8 - Arm einfahren
				next3D = HRL_Steuerung_GetNewJob_DontCareState();
				next3D.z = Z_Middle;
				HRL_Steuerung_GetNewJob_Qsend(next3D);
			}
		}
	}		
}

NextMovement HRL_Steuerung_GetNewJob_DontCareState(){
	NextMovement returnValue; // 0=stop für den Motor; 0<sensorwert-1
	returnValue.carry = DontCare;
	returnValue.IO= DontCare;
	returnValue.x=  DontCare;
	returnValue.y=  DontCare;
	returnValue.z=  DontCare;
	returnValue.allocation = DontCare;

	return returnValue;
}

void HRL_Steuerung_GetNewJob_Qsend(NextMovement move){
	NextMovementUNION transmit;
	transmit.move = move;
	//printf
	if((msgQSend(mesgQueueIdNextMovement, transmit.charvalue, sizeof(transmit.charvalue), WAIT_FOREVER, MSG_PRI_NORMAL)) == ERROR)
		printf("msgQSend in HRL_Steuerung_GetNewJob failed\n");		
}


