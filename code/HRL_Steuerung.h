#ifndef DOUBLE_INCLUDE_CHECK_HRL_STEUERUNG
#define DOUBLE_INCLUDE_CHECK_HRL_STEUERUNG

#include "simulation.h"

/*
 * HRL_Steuerung.h
 *
 *  Created on: 27.06.2016
 *      Author: Zauberer
 */

#define PositionXinput 1;
#define PositionYinput 1; 
#define PositionXOutput 8;
#define PositionYOutput 1; 

// Msg-Queue
MSG_Q_ID mesgQueueIdCmd;
#define MSG_Q_CMD_MAX_Messages 200
typedef struct {
	UINT highprio 	: 1; 
	UINT cmd 		: 1; //shutdown?
	UINT x			: 4; //1-10 (16)
	UINT y			: 3; //1-5  (8)
} commandbits;
typedef union{
	char charvalue[2];
	commandbits bits;
} cmdQdata;

//--------------------------------------

void HRL_Steuerung_init();

#endif /*HRL_STEUERUNG*/
