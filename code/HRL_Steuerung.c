#include "HRL_Steuerung.h"

/*
 * HRL_Steuerung.c
 *
 *  Created on: 27.06.2016
 *      Author: Zauberer
 */
#define DontCare (-1)
#define Z_IO 2
#define Z_Middle 1
#define Z_Inside 0
#define Carry_Get 1
#define Carry_Give 0
#define IO_GetBreak 1
#define IO_GetFree 0

bool shutdown_HRL_Steuerung;
bool belegungsMatrix[10][5];
int lastSensorX;
int lastSensorY;
int lastSensorZ;
int lastOutputState;
int lastInputState;
int lastCarryState;

typedef struct {
	int x;
	int y;
	int z;
	int IO;
	int carry;
} NextMovement;

typedef union{
	NextMovement move;
	char charvalue[sizeof(NextMovement)+1];
}NextMovementUNION;

MSG_Q_ID mesgQueueIdNextMovement;
MSG_Q_ID mesgQueueIdAktorDataPush;

void HRL_Steuerung_AktorDataPush();
void HRL_Steuerung_Movement();
void HRL_Steuerung_Movement_GetSensorBusData();
void HRL_Steuerung_Movement_GetSensorBusData_ERROR(char* msg);
void HRL_Steuerung_GetNewJob();
NextMovement HRL_Steuerung_GetNewJob_DontCareState();
void HRL_Steuerung_GetNewJob_Qsend();

int HRL_Steuerung_init(){
	printf("Start: HRL-Steuerung \n");
	bool abort = false;
	//TODO: Abbruch bei Fehlern? Ja
	
	if ((mesgQueueIdNextMovement = msgQCreate(5,3,MSG_Q_FIFO))	== NULL){
		printf("msgQCreate (NextMovement) in HRL_Steuerung_init failed\n");
		abort = true;
	}
	
	if ((mesgQueueIdCmd = msgQCreate(MSG_Q_CMD_MAX_Messages,1,MSG_Q_PRIORITY))	== NULL){
		printf("msgQCreate (CMD) in HRL_Steuerung_init failed\n");
		abort = true;
	}
	
	// Aktor Data Push
	if ((mesgQueueIdAktorDataPush = msgQCreate(2,1,MSG_Q_FIFO))	== NULL){
		printf("msgQCreate (AktorDataPush) in HRL_Steuerung_init failed\n");
		abort = true;
	}
	
	if (abort){
		return (-1);
	}
	else{
		taskSpawn("HRL_Steuerung_AktorDataPush",120,0x100,2000,(FUNCPTR)HRL_Steuerung_AktorDataPush,0,0,0,0,0,0,0,0,0,0);
		
		
		//TODO: linke und rehte Box bestimmen
	/*	if (PositionXinput < PositionXoutput){
			//test
		}*/
		
		
		return 0;
	}
		
	
}

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

void HRL_Steuerung_Movement(){
	while(1){
		HRL_Steuerung_Movement_GetSensorBusData();
		HRL_Steuerung_GetNewJob();
		
		NextMovementUNION nextmove;
		if(msgQReceive(mesgQueueIdNextMovement, nextmove.charvalue, sizeof(nextmove.charvalue), WAIT_FOREVER) == ERROR)
			printf("msgQReceive in HRL_Steuerung_Movement failed\n");	
	}
		
}

void HRL_Steuerung_Movement_GetSensorBusData(){
	sbusdata returnvalue;
	int i, errorcount;
	if(msgQReceive(mesgQueueIdSensorData, returnvalue.smsg, sizeof(returnvalue.smsg), WAIT_FOREVER) == ERROR){ 
		printf("msgQReceive in HRL_Steuerung_Movement_GetSensorBusData failed\n");
		returnvalue.l = 0;
	}
	else{
		
		// X-Achse
		errorcount= (-1);
		for (i = 0; i < 10; i++) {
			if ( (returnvalue.l & (1<(i+10)) ) == 0)
			{
				lastSensorX = i;
				errorcount++;
			}
		}
		if (errorcount>0){
			HRL_Steuerung_Movement_GetSensorBusData_ERROR("mehrere X-Sensoren ausgelöst");
		}
		
		// Y-Achse
		errorcount= (-1);
		//unten
		for (i = 0; i < 5; i++) {
			if ( (returnvalue.l & (1<i) ) == 0)
			{
				lastSensorY = i*2+1;
				errorcount++;
			}
		}
		//oben
		for (i = 0; i < 5; i++) {
			if ( (returnvalue.l & (1<(i+5)) ) == 0)
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
			if ( (returnvalue.l & (1<(i+20)) ) == 0)
			{
				lastSensorZ = i;
				errorcount++;
			}
		}
		if (errorcount>0){
			HRL_Steuerung_Movement_GetSensorBusData_ERROR("mehrere Z-Sensoren ausgelöst");
		}
		
		//licht
		if (!returnvalue.sbits.lT){
			
		}
		
	}
	//return returnvalue;
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
	//obere Prio zuerst //danach normal
	NextMovement next3D;
	cmdQdata cmdData;
	
	if(msgQReceive(mesgQueueIdCmd, cmdData.charvalue, 1, 350) == ERROR) {//etwa 5 Sekunden Timeout
		if( msgQNumMsgs(mesgQueueIdCmd) == 0){
			// Pause-Modus einleiten - Timeout 
			next3D = HRL_Steuerung_GetNewJob_DontCareState();
			next3D.x = PositionXinput;
			next3D.y = PositionYinput;
			HRL_Steuerung_GetNewJob_Qsend(next3D);
	
		}
	}
	else{
		if (cmdData.bits.highprio){
			belegungsMatrix[cmdData.bits.x][cmdData.bits.y] = cmdData.bits.cmd;
		}else if (cmdData.bits.cmd){//insert xy
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
			HRL_Steuerung_GetNewJob_Qsend(next3D);	
			
		}else{//remove xy
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
			HRL_Steuerung_GetNewJob_Qsend(next3D);
			// 5 - zum Auslader
			next3D = HRL_Steuerung_GetNewJob_DontCareState();
			next3D.x = PositionXOutput;
			next3D.y = PositionYOutput-1;
			HRL_Steuerung_GetNewJob_Qsend(next3D);
			// 6 - Arm ausfahren und auf Päckchen warten
			next3D = HRL_Steuerung_GetNewJob_DontCareState();
			next3D.z = Z_IO;
			next3D.IO = IO_GetFree;
			HRL_Steuerung_GetNewJob_Qsend(next3D);
			// 7 - Päckchen auf Arm fahren
			next3D = HRL_Steuerung_GetNewJob_DontCareState();
			next3D.carry = Carry_Give;
			next3D.IO = IO_GetBreak;
			next3D.y = PositionYOutput+1;
			HRL_Steuerung_GetNewJob_Qsend(next3D);
			// 8 - Arm einfahren
			next3D = HRL_Steuerung_GetNewJob_DontCareState();
			next3D.z = Z_Middle;
			HRL_Steuerung_GetNewJob_Qsend(next3D);
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

	return returnValue;
}

void HRL_Steuerung_GetNewJob_Qsend(NextMovement move){
	NextMovementUNION transmit;
	transmit.move = move;
	if((msgQSend(mesgQueueIdNextMovement, transmit.charvalue, sizeof(transmit.charvalue), WAIT_FOREVER, MSG_PRI_NORMAL)) == ERROR)
		printf("msgQSend in HRL_Steuerung_GetNewJob failed\n");		
}


