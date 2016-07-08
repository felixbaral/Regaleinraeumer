#include "visualisierung.h"

void visualisierung();

void visualisierung_init(){
	if ((msgQvisualisierung = msgQCreate(2,sizeof(UIdata),MSG_Q_FIFO))	== NULL)
		printf("msgQCreate in visualisierung_init failed\n");
	

	
	taskSpawn("visualisierung",Priority_Visualisierung,0x100,2000,(FUNCPTR)visualisierung,0,0,0,0,0,0,0,0,0,0);
}

void visualisierung(){
	int xtemp;
	int ztemp;
	char belegt;
	int y;
	int x;
	UIdataUnion output;
	 	
	while(1){
		//printf("Visualisierung will visualisieren! \n\n\n");
		if(msgQReceive(msgQvisualisierung, output.charvalue, sizeof(output.charvalue), WAIT_FOREVER) == ERROR) 
			printf("msgQReceive in visualisierung failed\n");
		else{
			
			printf("\n\n\n\n");
			printf("    0  1  2  3  4  5  6  7  8  9\n");
			for (y = 0; y < (2*towerHeight); y++) {  // für jede Zeile
				if ( output.data.towerY == y){		 //Zeilenanfang mit Pfeil für yPos vom Tower
					printf("=>");
				}
				else 
					printf("| ");			
				if( y%2 == 1){				// für zwischenzeile mit boden
					for (x = 0; x < towerWidth; x++) {
						printf("+--");
					}
					printf("+\n"); //width + 1
				}
				else{				//Füllzeilen (in den Belegung angezeigt wird)
					for (x = 0; x < towerWidth; x++) {
						if(output.data.matrix[x][y/2] == true){
							belegt = '#';
						}
						else belegt = ' ';
						printf("|%c%c", belegt, belegt);
					}
					printf("| %d\n", 4-y/2);
				}		
			}
			
			printf("\n===");
			for (xtemp = 0; xtemp < towerWidth; xtemp++) {
				if (xtemp == output.data.towerX) {
					printf("/\\=");
				} else {
					printf("===");
				}
			}
			printf("\n\n Z-Posi:  ");
			
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
