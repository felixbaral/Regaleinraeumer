/*
 * readcommand.c
 *
 *  Created on: 13.01.2010
 *      Author: Oliver Jack
 */

#include <stdio.h>
#include "readcommand.h"
#include <string.h>

command readcommand() {
	int x, y;
	char cmd[20];
	command cmd_struct;
	printf("Bitte Kommando eingeben: ");
	fflush(stdout);
	if (scanf("%s %d %d", cmd, &x, &y)) {
		if (strcmp(cmd, "insert") == 0 || strcmp(cmd, "remove") == 0 || 
			strcmp(cmd, "vsetspace") == 0 || strcmp(cmd, "clearspace") == 0)
		{
			cmd_struct.parse_ok = true;
			strcpy(cmd_struct.cmd, cmd);
			cmd_struct.par1 = x;
			cmd_struct.par2 = y;
		}
	  } else {
			cmd_struct.parse_ok = false;
		}
		return cmd_struct;
}

