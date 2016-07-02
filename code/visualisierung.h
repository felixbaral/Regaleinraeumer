#ifndef DOUBLE_INCLUDE_CHECK_VISUALISIERUNG
#define DOUBLE_INCLUDE_CHECK_VISUALISIERUNG

#include "bool_types.h"
#include <stdio.h>
#include "msgQLib.h"
#include "config.h"


// Msg-Queue Steuerung->Visualisierung
MSG_Q_ID msgQvisualisierung;

typedef struct{
	int towerX;
	int towerY;
	int towerZ;
	bool input;
	bool output;
	bool carry;
	bool matrix[5][10]; //  [y][x]
} UIdata;
typedef union{
	UIdata data;
	char charvalue[sizeof(UIdata)];
	long double l; 
}UIdataUnion;

void visualisierung();

#endif /*VISUALISIERUNG */
