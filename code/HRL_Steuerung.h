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

// Struktur der Kommando-Übergabe
typedef struct {
	UINT highprio 	: 1; 
	UINT cmd 		: 1; 
	UINT x			: 4; //0-9 (16)
	UINT y			: 3; //0-4  (8)
} commandbits;
// Union für Char-Wert an msgQ
typedef union{
	char charvalue[sizeof(commandbits)];
	commandbits bits;
} cmdQdata;

//--------------------------------------

int HRL_Steuerung_init();

#endif /*HRL_STEUERUNG*/
