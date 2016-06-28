#include "HRL_Steuerung.h"

/*
 * HRL_Steuerung.c
 *
 *  Created on: 27.06.2016
 *      Author: Zauberer
 */

#define DontMove 0
#define Z_IO 3
#define Z_Middle 2
#define Z_Inside 1
#define Carry_Get 1;
#define Carry_Give 0;
#define IO_

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
	int x;
	int y;
	int z;
	int input;
	int output;
	int carry;
} NextMovement;

typedef union{
	NextMovement move;
	char charvalue[sizeof(NextMovement)+1];
}NextMovementUNION;

MSG_Q_ID mesgQueueIdNextMovement;
MSG_Q_ID mesgQueueIdAktorDataPush;

void HRL_Steuerung_AktorDataPush();
void HRL_Steuerung_GetNewJob();
NextMovement HRL_Steuerung_GetNewJob_StopState();
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

void HRL_Steuerung_GetNewJob()
{
	//obere Prio zuerst //danach normal
	NextMovement next3D;
	cmdQdata cmdData;
	
	if(msgQReceive(mesgQueueIdCmd, cmdData.charvalue, 1, 350) == ERROR) {//etwa 5 Sekunden Timeout
		if( msgQNumMsgs(mesgQueueIdCmd) == 0){
			// Pause-Modus einleiten - Timeout 
			TurmPause = true;
			if (!TurmPause){
				next3D = HRL_Steuerung_GetNewJob_StopState();
				next3D.x = PositionXinput;
				next3D.y = PositionYinput;
				HRL_Steuerung_GetNewJob_Qsend(next3D);
			}
		}
	}
	else{
		if (cmdData.bits.highprio){
			belegungsMatrix[cmdData.bits.x][cmdData.bits.y] = cmdData.bits.cmd;
		}else if (cmdData.bits.cmd){//insert xy
			next3D = HRL_Steuerung_GetNewJob_StopState();
			next3D.x = PositionXinput;
			next3D.y = PositionYinput;
			HRL_Steuerung_GetNewJob_Qsend(next3D);
			
			next3D = HRL_Steuerung_GetNewJob_StopState();
			next3D.z = 3;
			next3D.input = 2;
			HRL_Steuerung_GetNewJob_Qsend(next3D);
			
			next3D = HRL_Steuerung_GetNewJob_StopState();
			next3D.carry = 1;
			next3D.input = 1;
			HRL_Steuerung_GetNewJob_Qsend(next3D);
			
		}else{
			//add remove
		}
	}		
}

NextMovement HRL_Steuerung_GetNewJob_StopState(){
	NextMovement returnValue; // 0=stop für den Motor; 0<sensorwert-1
	returnValue.input=0;
	returnValue.output=0;
	returnValue.x=0;
	returnValue.y=0;
	returnValue.z=0;

	return returnValue;
}

void HRL_Steuerung_GetNewJob_Qsend(NextMovement move){
	NextMovementUNION transmit;
	transmit.move = move;
	if((msgQSend(mesgQueueIdNextMovement, transmit.charvalue, sizeof(transmit.charvalue), WAIT_FOREVER, MSG_PRI_NORMAL)) == ERROR)
		printf("msgQSend in HRL_Steuerung_GetNewJob failed\n");		
}


