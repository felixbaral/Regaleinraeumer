#include <stdio.h> 			//printf
#include "HRL_Steuerung.h"	// Initialisierungroutine
#include "simulation.h"		// Initialisierungroutine
#include "readcommand.h"	// Bibliothek zur Eingabeerfassung
#include "visualisierung.h"	// Initialisierung der Konsolenausgabe



void main_user_input();
void main_init();

main(void){
	printf("System starting \n");
	
	
	visualisierung_init();	
	if ( Simulation_init() == (-1) ){	// msgQueue- und Semaphoreerzeugungsfehler abfangen
		printf("Simulation_init fehlgeschlagen");
		return 0;
	}
	else if ( HRL_Steuerung_init() == (-1) ){	// msgQueue- und Semaphoreerzeugungsfehler abfangen
		printf("HRL_Steuerung_init fehlgeschlagen");
		return 0;
	}	
	main_user_input();		// User-Engabemodus aktivieren
}

void main_user_input(){
	command cmd;	// Variable in der eingelesene Befehle geschrieben werden
	cmdQdata cmdQ;	// Struktur für msgQueue - an Steuerung -
	static bool belegung[10][5];	// lokal genutzte Belegungsmatrix des Hochregallagers
	int x, y;
	for (x = 0; x < 10; x++) {		// Initialisierung der Bool-Arrays für Grundbelegung (0)
		for (y = 0; y < 5; y++) {
			belegung[x][y]=false;
			belegungsMatrix[x][y]=false;
		}
	}
	taskPrioritySet(taskIdSelf(), Priority_Main ); //eigene Prio runter 
	
	
	while(1){
		cmd = readcommand();	// auf Eingabe von User warten
		cmd.par2=(towerHeight-1)-cmd.par2;	// Y-Wert invertieren um logische Regalanordnung zu gewaehrleisten
		
		if (cmd.parse_ok){	// Eingabe OK?
			if ( (cmd.par1 < 0) || (cmd.par1 > 9) ){
				printf("x-Adressierung außerhalb von 0 bis 9 \nUngueltige Eingabe ! \n");
				cmd.parse_ok = false;
			}
			else if ( (cmd.par2 < 0) || (cmd.par2 > 4) ){
				printf("y-Adressierung außerhalb von 0 bis 4 \nUngueltige Eingabe ! \n");
				cmd.parse_ok = false;
			}
			else {
				// Überprüfung ob Insert-Befehl nicht auf volles Fach zeigt
				if ( (strcmp(cmd.cmd, "insert") == 0) && (belegung[cmd.par1][cmd.par2]) ){	
					printf("insert nicht möglich - angegebene Position[%d][%d] ist belegt\nUngueltige Eingabe ! \n", cmd.par1, (towerHeight-1)-cmd.par2);
					cmd.parse_ok=false;
				}
				// Überprüfung ob Remove-Befehle nicht auf leeres Fach zeigt
				else if ( (strcmp(cmd.cmd, "remove")==0) && (belegung[cmd.par1][cmd.par2] == false) ){
					printf("remove nicht möglich - angegebene Position[%d][%d] ist leer\nUngueltige Eingabe ! \n", cmd.par1, (towerHeight-1)-cmd.par2);
					cmd.parse_ok=false;
				}
			}
		}
		else{
			// Grundlegender Fehler bei scanf
			printf("Ein Befehl + 2 Koordinaten \nUngueltige Eingabe ! \n");
		}
		// Befehl ist ok, wird weiter verarbeitet und in Struct fuer msgQ uebertragen 
		if (cmd.parse_ok) {
			cmdQ.bits.x = cmd.par1;
			cmdQ.bits.y = cmd.par2;			
			if ( strcmp(cmd.cmd, "vsetspace") == 0 ){
				belegung[cmd.par1][cmd.par2] = true;
				cmdQ.bits.highprio=1;		// erhoehte Prioritaet fuer Admin-Befehl
				cmdQ.bits.cmd=1;				
			}
			else if (strcmp(cmd.cmd, "clearspace") == 0){
				belegung[cmd.par1][cmd.par2] = false;
				cmdQ.bits.highprio=1;		// erhoehte Prioritaet fuer Admin-Befehl
				cmdQ.bits.cmd=0;					
			}
			else if (strcmp(cmd.cmd, "insert") == 0){
				belegung[cmd.par1][cmd.par2] = true;
				cmdQ.bits.highprio=0;		// regulaere Prioritaet fuer regulaere Befehle
				cmdQ.bits.cmd=1;
			}
			else if (strcmp(cmd.cmd, "remove") == 0){
				belegung[cmd.par1][cmd.par2] = false;
				cmdQ.bits.highprio=0;		// regulaere Prioritaet fuer regulaere Befehle
				cmdQ.bits.cmd=0;
			}			
			// Uebertragen des Befehls in msgQ an HRL-Steuerung(getNewJob) mit entsprechender Prioritaet
			if((msgQSend(mesgQueueIdCmd, cmdQ.charvalue, sizeof(cmdQ.charvalue), WAIT_FOREVER, cmdQ.bits.highprio)) == ERROR)
				printf("msgQSend in User-Input failed\n");			
		}
	}
}
