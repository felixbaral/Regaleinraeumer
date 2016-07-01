#include "stdio.h"
#include "visualisierung.h"

main (void){
	bool belegung[height][width];
	bool xpos[width]; 
	bool ypos[height]; 
	bool zpos[depth]; 
	bool eingang = false; 
	bool ausgang = false;
	
	visualisiere(belegung[height][width], xpos[width], ypos[height], zpos[depth], eingang, ausgang);
}
