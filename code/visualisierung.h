#ifndef DOUBLE_INCLUDE_CHECK_VISUALISIERUNG
#define DOUBLE_INCLUDE_CHECK_VISUALISIERUNG

#include "bool_types.h"
#include <stdio.h>
#include "HRL_Steuerung.h"
#include "msgQLib.h"

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
	char charvalue[sizeof(UIdata)+1];
}UIdataUnion;

void visualisiere(MSG_Q_ID msgQid);

#endif /*VISUALISIERUNG */
