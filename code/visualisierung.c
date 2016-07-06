#include "visualisierung.h"

void visualisierung_init(){
	if ((msgQvisualisierung = msgQCreate(2,sizeof(UIdata),MSG_Q_FIFO))	== NULL)
		printf("msgQCreate in visualisierung_init failed\n");
	

	
	taskSpawn("visualisierung",Priority_Visualisierung,0x100,2000,(FUNCPTR)visualisierung,0,0,0,0,0,0,0,0,0,0);
}

void visualisierung(){
	/* lokale Variable */
	int xtemp;
	int ztemp;
	char belegt;
	int y;
	int x;
	
	UIdataUnion output;
	
	/* Testparam*/
	/*belegung[4][2] = true;
	xpos[1]= true;
	ypos[2]= true;
	ypos[6]= false;
	ypos[7]= false;
	ypos[8]= false;
	zpos[2]= true;*/
	 	
	while(1){
		//printf("Visualisierung will visualisieren! \n\n\n");
		if(msgQReceive(msgQvisualisierung, output.charvalue, sizeof(output.charvalue), WAIT_FOREVER) == ERROR) 
			printf("msgQReceive in visualisierung failed\n");
		else{
			
			printf("\n\n\n\n");
			
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
					printf("|\n");
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
