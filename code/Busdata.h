#ifndef BUSDATA_H_
#define BUSDATA_H_

/*
 * Busdata.h
 *
 *  Created on: 05.01.2010
 *      Author: Oliver Jack
 */

#define UINT unsigned int

typedef struct { // Sensoren
	UINT sYu :5;
	UINT sYo :5;
	UINT sX :10;
	UINT sZ :3;
	UINT sL :2; // Lichtschranken
	UINT sLt :1; // Lichttaster
} Sensorgroups;

typedef struct {
	// Tastsensoren
	UINT y1u :1;
	UINT y2u :1;
	UINT y3u :1;
	UINT y4u :1;
	UINT y5u :1;
	UINT y1o :1;
	UINT y2o :1;
	UINT y3o :1;
	UINT y4o :1;
	UINT y5o :1;
	UINT x1 :1;
	UINT x2 :1;
	UINT x3 :1;
	UINT x4 :1;
	UINT x5 :1;
	UINT x6 :1;
	UINT x7 :1;
	UINT x8 :1;
	UINT x9 :1;
	UINT x10 :1;
	UINT zR :1;
	UINT zM :1;
	UINT zEA :1;
	// Lichtsensoren
	UINT lL :1; //id 23
	UINT lR :1;	//id 24
	UINT lT :1; //id 25
} Sensorbits;

typedef union {
	unsigned long l; // Test und Initialisierung
	char smsg[sizeof(Sensorbits)]; // Für MessageQueue
	Sensorbits sbits;
	Sensorgroups sgroups;
	//UINT index : 26;
} sbusdata;

//---------------------------------------------------------
//--------------Aktoren------------------------------------
//---------------------------------------------------------

typedef struct {
	UINT ax :3;
	UINT ay :2;
	UINT az :2;
	UINT aea :4;
} Aktorgroups;

typedef struct { // Aktoren
	UINT axl :1;
	UINT axr :1;
	UINT axs :1;
	UINT ayo :1;
	UINT ayu :1;
	UINT azh :1;
	UINT azv :1;
	UINT aearre :1; // E/A-Slot rechts rein
	UINT aearra :1;
	UINT aealre :1;
	UINT aealra :1;
} Aktorbits;

typedef union {
	unsigned int i; // Test und Initialisierung
	char amsg[sizeof(Aktorbits)]; // Für MessageQueue
	Aktorbits abits;
	Aktorgroups agroups;
} abusdata;

#endif /* BUSDATA_H_ */
