#ifndef DOUBLE_INCLUDE_CHECK_HRL_STEUERUNG
#define DOUBLE_INCLUDE_CHECK_HRL_STEUERUNG

#include "simulation.h"
#include "Busdata.h"
#include "config.h"
#include "bool_types.h"
#include "visualisierung.h"

/*
 * HRL_Steuerung.h
 *
 *  Created on: 27.06.2016
 *      Author: Zauberer
 */

// Msg-Queue Eingabe->GetNewJob
MSG_Q_ID mesgQueueIdCmd;

typedef struct {
	UINT highprio 	: 1; 
	UINT cmd 		: 1; //TODO:shutdown?
	UINT x			: 4; //1-10 (16)
	UINT y			: 3; //1-5  (8)
} commandbits;
typedef union{
	char charvalue[sizeof(commandbits)];
	commandbits bits;
} cmdQdata;

//--------------------------------------

int HRL_Steuerung_init();

#endif /*HRL_STEUERUNG*/
