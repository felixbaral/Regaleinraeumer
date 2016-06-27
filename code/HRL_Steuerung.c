#include "HRL_Steuerung.h"

/*
 * HRL_Steuerung.c
 *
 *  Created on: 27.06.2016
 *      Author: Zauberer
 */

bool shutdown_HRL_Steuerung;
bool TurmPause = false;
bool belegungsMatrix[10][5];
int lastSensorX;
int lastSensorY;
int lastSensorZ;
int lastOutputState;
int lastInputState;
int lastCarryState;

typedef struct {
	UINT x;
	UINT y;
	UINT z;
	UINT input;
	UINT output;
	UINT carry;
} NextMovement;

typedef union{
	NextMovement move;
	char charvalue[sizeof(NextMovement)+1];
}NextMovementUNION;

MSG_Q_ID mesgQueueIdNextMovement;
MSG_Q_ID mesgQueueIdAktorDataPush;
abusdata AktorDataPush;


void HRL_Steuerung_AktorDataPush();
void HRL_Steuerung_GetNewJob();
NextMovement HRL_Steuerung_GetNewJob_StopState();
void HRL_Steuerung_GetNewJob_Qsend();

void HRL_Steuerung_init(){
	printf("Start: HRL-Steuerung \n");
	
	// Aktor Data Push
	taskSpawn("HRL_Steuerung_AktorDataPush",120,0x100,2000,(FUNCPTR)HRL_Steuerung_AktorDataPush,0,0,0,0,0,0,0,0,0,0);
	
	if ((mesgQueueIdNextMovement = msgQCreate(5,3,MSG_Q_FIFO))	== NULL)
		printf("msgQCreate (NextMovement) in HRL_Steuerung_init failed\n");
	
	if ((mesgQueueIdCmd = msgQCreate(MSG_Q_CMD_MAX_Messages,1,MSG_Q_PRIORITY))	== NULL)
		printf("msgQCreate (CMD) in HRL_Steuerung_init failed\n");
	
	if ((mesgQueueIdAktorDataPush = msgQCreate(2,1,MSG_Q_FIFO))	== NULL)
		printf("msgQCreate (AktorDataPush) in HRL_Steuerung_init failed\n");
}

void HRL_Steuerung_AktorDataPush(){
	while (!shutdown_HRL_Steuerung){
	//	if(msgQReceive(mesgQueueIdAktorDataPush, &ValueToBus.charvalue, 1, WAIT_FOREVER) == ERROR) 
			printf("msgQReceive in SensorCollector failed\n");
		
		semTake(semBinary_SteuerungToSimulation, WAIT_FOREVER); 
		SteuerungToSimulation = AktorDataPush;
		semGive(semBinary_SteuerungToSimulation);
	}
}

void HRL_Steuerung_GetNewJob()
{
	//obere Prio zuerst //danach normal
	NextMovement next3D;
	cmdQdata cmdData;
	
	if(msgQReceive(mesgQueueIdCmd, cmdData.charvalue, 1, 350) == ERROR) {//etwa 5 Sekunden Timeout
		if( msgQNumMsgs(mesgQueueIdCmd) == 0){
			// Pause-Modus einleiten - Timeout 
			TurmPause = true;
			next3D = HRL_Steuerung_GetNewJob_StopState();
			next3D.x = PositionXinput;
			next3D.y = PositionYinput;
			HRL_Steuerung_GetNewJob_Qsend(next3D);
		}
	}
	else{
		if (cmdData.bits.highprio){
			belegungsMatrix[cmdData.bits.x][cmdData.bits.y] = cmdData.bits.cmd;
		}else if (cmdData.bits.cmd){//insert xy
			/*next3D = HRL_Steuerung_GetNewJob_StopState();
			next3D.x = PositionXinput;
			next3D.y = PositionYinput;
			HRL_Steuerung_GetNewJob_Qsend(next3D);
			
			next3D.x = PositionXinput;
			next3D.y = PositionYinput;
			HRL_Steuerung_GetNewJob_Qsend(next3D);*/
		}else{
			//add remove
		}
	}		
}

NextMovement HRL_Steuerung_GetNewJob_StopState(){ //TODO: müsste eigentlich nach Fall berechnet werden - besprechen
	NextMovement returnValue;
	returnValue.input=lastInputState;
	returnValue.output=lastOutputState;
	returnValue.x=lastSensorX;
	returnValue.y=lastSensorY;
	returnValue.z=lastSensorZ;

	return returnValue;
}

void HRL_Steuerung_GetNewJob_Qsend(NextMovement move){
	NextMovementUNION transmit;
	transmit.move = move;
	if((msgQSend(mesgQueueIdNextMovement, transmit.charvalue, sizeof(transmit.charvalue), WAIT_FOREVER, MSG_PRI_NORMAL)) == ERROR)
		printf("msgQSend in HRL_Steuerung_GetNewJob failed\n");		
}


