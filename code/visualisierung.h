#ifndef DOUBLE_INCLUDE_CHECK_VISUALISIERUNG
#define DOUBLE_INCLUDE_CHECK_VISUALISIERUNG

#include "bool_types.h"
#include <stdio.h>
#include "msgQLib.h"
#include "config.h"

// Msg-Queue Steuerung->Visualisierung
MSG_Q_ID msgQvisualisierung;

// Struct zur Uebergabe/Uebername der Turm-Position, Lichtschranken/Sensorstati und Belegung
typedef struct{
	int towerX;
	int towerY;
	int towerZ;
	bool input;
	bool output;
	bool carry;
	bool matrix[10][5]; //  [x][y]
} UIdata;
// Union fuer char Wert des Structs fuer msgQ
typedef union{
	UIdata data;
	char charvalue[sizeof(UIdata)];
	long double l; 
}UIdataUnion;

void visualisierung_init();

#endif /*VISUALISIERUNG */
