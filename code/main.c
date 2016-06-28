#include <stdio.h>
#include "HRL_Steuerung.h"
#include "simulation.h"
#include "readcommand.h"


void main_init_all();
void main_user_input();

main(void){
	printf("Hallo Andreas \n");
	
	main_init_all();
	
	main_user_input();
	
}

void main_init_all(){
	//Simulation_init();
	HRL_Steuerung_init();
	
}

void main_user_input(){
	command cmd;
	cmdQdata cmdQ;
	
	taskPrioritySet(taskIdSelf(), 150 ); //eigene Prio runter 
	
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
			else if (strcmp(cmd, "clearspace") == 0){
				cmdQ.bits.highprio=1;
				cmdQ.bits.cmd=0;
				//printf("Nach nächstem Job: - Belegung - (%d - %d) \n", cmd.par1, cmd.par2);
							
			}
			else if (strcmp(cmd, "insert") == 0){
				cmdQ.bits.highprio=0;
				cmdQ.bits.cmd=1;
				//printf("In Queue aufgenommen: + Einlagerung + (%d - %d) \n", cmd.par1, cmd.par2);
			}
			else if (strcmp(cmd, "remove") == 0){
				cmdQ.bits.highprio=0;
				cmdQ.bits.cmd=0;
				//printf("In Queue aufgenommen: - Auslagerung - (%d - %d) \n", cmd.par1, cmd.par2);
			}
			
			if((msgQSend(mesgQueueIdCmd, cmdQ.charvalue, 2, WAIT_FOREVER, cmdQ.bits.highprio)) == ERROR)
				printf("msgQSend in User-Input failed\n");			
		}
		taskDelay(1);
	}
}
