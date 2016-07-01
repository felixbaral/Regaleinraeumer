#include <stdio.h>
#include "visualisierung.h"

void visualisiere(bool belegung[height][width], bool xpos[width], bool ypos[2*height], bool zpos[depth], bool eingang, bool ausgang){
	/* lokale Variable */
	int xtemp;
	int ztemp;
	char belegt;
	int y;
	int x;
	/* Testparam
	belegung[1][1] = true;
	xpos[1]= true;
	ypos[2]= true;
	zpos[2]= true;
	 */		
	printf("\n\n\n");
	
	for (y = 0; y < 2*height; y++) {
		if ( ypos[y] == true){
			printf("=>");
		}
		else printf("| ");
		if( y%2 == 1){
			for (x = 0; x < width; x++) {
				printf("+--");
			}
			printf("+\n");
		}
		else{

			for (x = 0; x < width; x++) {
				if(belegung[y/2][x] == true){
					belegt = '#';
				}
				else belegt = ' ';
				printf("|%c%c", belegt, belegt);
			}
			printf("|\n");
		}		
	}

	printf("\n===");
	for (xtemp = 0; xtemp < width; xtemp++) {
		if (xpos[xtemp]==true) {
			printf("/\\=");
		} else {
			printf("===");
		}
	}
	printf("\n\n Z-Position:  ");

	for (ztemp = 0; ztemp < depth; ztemp++) {
		if(zpos[ztemp]==true){
			printf("|X|");
		}
		else printf("|_|");
	}
	printf("\n\n Eingang:  ");
	if(eingang == true){
		printf("|X|  Ausgang:  ");
	}
	else printf("|_|  Ausgang:  ");
	if(ausgang == true){
		printf("|X|");
	}
	else printf("|_|");
}
