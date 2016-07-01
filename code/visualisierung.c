#include "visualisierung.h"


void visualisiere(MSG_Q_ID msgQid){
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
		if(msgQReceive(msgQid, output.charvalue, sizeof(output.charvalue), WAIT_FOREVER) == ERROR) 
			printf("msgQReceive in visualisierung failed\n");
		
		
		printf("\n\n\n");
		
		for (y = 0; y < (2*towerHeight); y++) {  // f�r jede Zeile
			if ( output.data.towerY == y){		 //Zeilenanfang mit Pfeil f�r yPos vom Tower
				printf("=>");
			}
			else 
				printf("| ");			
			if( y%2 == 1){				// f�r zwischenzeile mit boden
				for (x = 0; x < towerWidth; x++) {
					printf("+--");
				}
				printf("+\n"); //width + 1
			}
			else{				//F�llzeilen (in den Belegung angezeigt wird)
				for (x = 0; x < towerWidth; x++) {
					if(output.data.matrix[y/2][x] == true){
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
			printf("|X|");
		}
		else printf("|_|");
	}
}
