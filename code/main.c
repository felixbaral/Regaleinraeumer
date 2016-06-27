#include <stdio.h>
#include <readcommand.h>
//#include "simulation.h"
#include "HRL_Steuerung.h"


main(void){
	printf("Hallo Andreas \n");
	
	command testcmd;
	char testchar[20];
	int zahl;
	
	//testcmd = readcommand(testchar);
	Simulation_init();
	fflush(stdout);
	//scanf("%d", &zahl);
	//printf("%d", zahl);
	if(scanf("%c", testchar)){
		printf("%c", testchar);
	}
	
}
