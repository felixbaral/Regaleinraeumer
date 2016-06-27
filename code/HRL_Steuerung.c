#include "HRL_Steuerung.h"

/*
 * HRL_Steuerung.c
 *
 *  Created on: 27.06.2016
 *      Author: Zauberer
 */
/*
void HRL_Steuerung_AktorDataPush();

void HRL_Steuerung_init(){
	printf("Start: HRL-Stuerung \n");
	
	semBinary_AktorDataPush = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
	taskSpawn("HRL_Steuerung_AktorDataPush",120,0x100,2000,(FUNCPTR)HRL_Steuerung_AktorDataPush,0,0,0,0,0,0,0,0,0,0);
	
	
}

void HRL_Steuerung_AktorDataPush(){
	while (1){//!shutdown_HRL_Steuerung
		semTake(semBinary_AktorDataPush, WAIT_FOREVER); //Wait for new AktorData
		//Mutex wird an anderer Stelle freigegeben
		
		semTake(semBinary_SteuerungToSimulation, WAIT_FOREVER); 
		SteuerungToSimulation = AktorDataPush;
		semGive(semBinary_SteuerungToSimulation);
	}
}*/
