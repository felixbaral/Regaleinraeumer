#include <stdio.h> //printf
#include "HRL_Steuerung.h"
#include "simulation.h"
#include "readcommand.h"

//TODO: delete
#include "visualisierung.h"


void main_user_input();

main(void){
	printf("Hallo Andreas \n");
	
	//TODO: visu test
	//visualisiere(belegung, xpos, ypos, zpos, eingang, ausgang);
	
	visualisierung_init();
	
	if ( Simulation_init() == (-1) ){
		printf("Simulation_init fehlgeschlagen");
		return 0;
	}
	else if ( HRL_Steuerung_init() == (-1) ){
		printf("HRL_Steuerung_init fehlgeschlagen");
		return 0;
	}
	
	main_user_input();
}

void main_user_input(){
	command cmd;
	cmdQdata cmdQ;
	
	taskPrioritySet(taskIdSelf(), Priority_Main ); //eigene Prio runter 
	
	while(1){
		cmd = readcommand();
		if (cmd.parse_ok) {
			
			cmdQ.bits.x = cmd.par1;
			cmdQ.bits.y = cmd.par2;			
			if ( strcmp(cmd.cmd, "vsetspace") == 0 ){
				cmdQ.bits.highprio=1;
				cmdQ.bits.cmd=1;	
				//printf("Nach nächstem Job: + Belegung + (%d - %d) \n", cmd.par1, cmd.par2);
			}
			else if (strcmp(cmd.cmd, "clearspace") == 0){
				cmdQ.bits.highprio=1;
				cmdQ.bits.cmd=0;
				//printf("Nach nächstem Job: - Belegung - (%d - %d) \n", cmd.par1, cmd.par2);
							
			}
			else if (strcmp(cmd.cmd, "insert") == 0){
				cmdQ.bits.highprio=0;
				cmdQ.bits.cmd=1;
				//printf("In Queue aufgenommen: + Einlagerung + (%d - %d) \n", cmd.par1, cmd.par2);
			}
			else if (strcmp(cmd.cmd, "remove") == 0){
				cmdQ.bits.highprio=0;
				cmdQ.bits.cmd=0;
				//printf("In Queue aufgenommen: - Auslagerung - (%d - %d) \n", cmd.par1, cmd.par2);
			}
			
			if((msgQSend(mesgQueueIdCmd, cmdQ.charvalue, sizeof(cmdQ.charvalue), WAIT_FOREVER, cmdQ.bits.highprio)) == ERROR)
				printf("msgQSend in User-Input failed\n");			
		}
		taskDelay(1);
	}
}
