#include "visualisierung.h"

void visualisierung();

void visualisierung_init(){
	// Erzeugung der msgQ in die die HRL-Steuerung die Statusupdates sendet
	if ((msgQvisualisierung = msgQCreate(2,sizeof(UIdata),MSG_Q_FIFO))	== NULL)
		printf("msgQCreate in visualisierung_init failed\n");	
	// Spawn des Visualisierungs-Tasks
	taskSpawn("visualisierung",Priority_Visualisierung,0x100,2000,(FUNCPTR)visualisierung,0,0,0,0,0,0,0,0,0,0);
}

void visualisierung(){
	int xtemp;		// Turmposition auf X
	int ztemp;		// Turmposition auf Y
	char belegungszeichen; // belegte Regalfaecher
	int y;			// Laufvariable
	int x;			// Laufvariable
	UIdataUnion output;	// Datenstruct aus msgQ
	 	
	while(1){
		// Warten auf neue Daten zur Visualisierung - Selbsblockade
		if(msgQReceive(msgQvisualisierung, output.charvalue, sizeof(output.charvalue), WAIT_FOREVER) == ERROR) 
			printf("msgQReceive in visualisierung failed\n");
		else{			
			printf("\n\n\n\n");
			printf("    0  1  2  3  4  5  6  7  8  9\n");
			for (y = 0; y < (2*towerHeight); y++) {  // für jede Zeile
				if ( output.data.towerY == y){		 // Zeilenanfang mit Pfeil für y-Position vom Turm
					printf("=>");
				}
				else 
					printf("| ");			
				if( y%2 == 1){				// für zwischenzeile mit boden
					for (x = 0; x < towerWidth; x++) {
						printf("+--");
					}
					printf("+\n"); // abschließendes "+" zum Schließen des letzten Regalfachs
				}
				else{				// Füllzeilen (in denen Belegung angezeigt wird)
					for (x = 0; x < towerWidth; x++) {
						if(output.data.matrix[x][y/2] == true){
							belegungszeichen = '#';
						}
						else belegungszeichen = ' ';
						printf("|%c%c", belegungszeichen, belegungszeichen);
					}
					printf("| %d\n", (towerHeight-1)-y/2);	// Nummerierung invertieren
				}		
			}
			// X-Achse mit /\ zur Anzeige des Turms
			printf("\n===");
			for (xtemp = 0; xtemp < towerWidth; xtemp++) {
				if (xtemp == output.data.towerX) {
					printf("/\\=");
				} else {
					printf("===");
				}
			}
			// Z-Position des Schlittens auf dem Ausleger des Turms
			printf("\n\n Z-Position:  ");
			for (ztemp = 0; ztemp < towerDepth; ztemp++) {
				if(ztemp == output.data.towerZ){
					if (output.data.carry == true){
						printf("|X|");
					}
					else
						printf("|_|");
					
				}
				else printf("___");
			}
			// Ein- und Ausgabe-Slot-Darstellung
			printf("\n\n Eingang:  ");
			if(output.data.input == true){
				printf("|X|  Ausgang:  ");
			}
			else printf("|_|  Ausgang:  ");
			if(output.data.output == true){
				printf("|X|\n");
			}
			else printf("|_|\n");
		}
	}
}
