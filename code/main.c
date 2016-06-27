#include <stdio.h>
#include <readcommand.h>
//#include "simulation.h"
#include "HRL_Steuerung.h"


main(void){
	printf("Hallo Andreas \n");
	
	command testcmd;
	char testchar[20];
	int zahl;
	
	MSG_Q_ID testQ;
	if ((testQ = msgQCreate(MSG_Q_CMD_MAX_Messages,1,MSG_Q_PRIORITY))	== NULL)
		printf("msgQCreate (CMD) in HRL_Steuerung_init failed\n");
	char tastchar = 1;
	if((msgQSend(testQ, &tastchar, 1, WAIT_FOREVER, MSG_PRI_NORMAL)) == ERROR)
		printf("msgQSend in SensorCollector failed\n");		
	zahl = msgQReceive(testQ, &tastchar, 1, 100);
	printf("%d \n", zahl);
	
	zahl = msgQReceive(testQ, &tastchar, 1, 350);
	printf("%d \n", zahl);
	
	
	
	//testcmd = readcommand(testchar);
	//Simulation_init();
	printf("%s \n",testcmd.cmd);
	while(1){
		taskDelay(1);
	}
	
}
