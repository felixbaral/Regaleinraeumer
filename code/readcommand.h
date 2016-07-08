#ifndef DOUBLE_INCLUDE_CHECK_READCOMMAND
#define DOUBLE_INCLUDE_CHECK_READCOMMAND

/*
 * readcommand.h
 *
 *  Created on: 13.01.2010
 *      Author: Captian Jack
 */

#include <string.h> //compare
#include <stdio.h>
#include "bool_types.h"

typedef struct {
	bool parse_ok;
	char cmd[20];
	int par1;
	int par2;
} command;

command readcommand();

#endif /* DOUBLE_INCLUDE_CHECK_READCOMMAND_H_ */
