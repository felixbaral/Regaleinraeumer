#include "readcommand.h"

/*
 * readcommand.c
 *
 *  Created on: 13.01.2010
 *      Author: Oliver Jack
 */


command readcommand() {
	int x, y;
	char cmd[20];
	command cmd_struct;
	cmd_struct.par1 = 0;
	cmd_struct.par2 = 0;
	cmd_struct.parse_ok = 0;
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
		} else {
			cmd_struct.parse_ok = false;
		}
	}
	return cmd_struct;
}

