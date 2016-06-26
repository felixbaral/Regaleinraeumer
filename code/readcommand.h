/*
 * readcommand.h
 *
 *  Created on: 13.01.2010
 *      Author: Oliver Jack
 */

#ifndef READCOMMAND_H_
#define READCOMMAND_H_

typedef enum {false, true} bool;
typedef struct {
	bool parse_ok;
	char cmd[20];
	int par1;
	int par2;
} command;

command readcommand(char *cmd);

#endif /* READCOMMAND_H_ */
