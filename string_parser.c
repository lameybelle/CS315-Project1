/*
 * string_parser.c
 *
 *  Created on: Nov 25, 2020
 *      Author: gguan, Monil
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

#define _GUN_SOURCE

int count_token (char* buf, const char* delim)
{
	char *tkn, *saveptr1, *str1, *placeholder;
	int counter = 1;

	strtok_r(buf, "\n", &saveptr1);

	for (placeholder = str1 = strdup(buf);; str1 = NULL) {
		tkn = strtok_r(str1, delim, &saveptr1);
		if (tkn == NULL)
			break;
		counter++;
	}
	free(placeholder);
	return counter;
}

command_line str_filler (char* buf, const char* delim)
{
	int i;
	char *tkn, *saveptr1, *str1, *placeholder;
	command_line cmd;
	cmd.num_token = count_token(buf, delim);

	cmd.command_list = (char **)malloc(sizeof(char *) * cmd.num_token);
	for (i = 0, placeholder = str1 = strdup(buf);; i++, str1 = NULL) {
		tkn = strtok_r(str1, delim, &saveptr1);
		if (tkn == NULL)
			break;
		cmd.command_list[i] = strdup(tkn);
	}
	free(placeholder);
	cmd.command_list[cmd.num_token - 1] = NULL;
	return cmd;
}


void free_command_line(command_line* command)
{
	for (int i = 0; i < command->num_token; i++) {
		free(command->command_list[i]);
	}
	free(command->command_list);
}
