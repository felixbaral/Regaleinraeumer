#include <stdio.h>
#include <readcommand.h>
#include "simulation.h"

main(void){
	printf("Hallo Andreas \n");
	
	command testcmd;
	char testchar[20];
	int zahl;
	
	//testcmd = readcommand(testchar);
	Sensorverwaltung();
	fflush(stdout);
	//scanf("%d", &zahl);
	//printf("%d", zahl);
	if(scanf("%c", testchar)){
		printf("%c", testchar);
	}
	
}
