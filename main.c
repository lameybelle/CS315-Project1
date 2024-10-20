#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include "command.h"
#include "string_parser.h"

#define _GNU_SOURCE

void interactiveMode();
void fileMode(char** argv);
void commandCheck(command_line phrase);
int parameterError(command_line phrase, int quantity);

int main(int argc, char** argv) {
	if (argc == 1) {
		interactiveMode();
	} else {
		FILE* exists = fopen(argv[2], "r");
		char* flag = argv[1];
		if (strncmp("-f", flag, (size_t)2) == 0 && exists != NULL) {
			fclose(exists);
			fileMode(argv);
		} else {
			printf("Error can't enter file mode\n");
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

void interactiveMode() {
	//take in info
	
	//line info
	int exit = 0;
	size_t len = 128;
	char* line_buf = malloc(len);

	command_line large_token_buffer;
	command_line small_token_buffer;

	while (!exit) {
		printf(">>> ");
		getline(&line_buf, &len, stdin);

		large_token_buffer = str_filler (line_buf, ";");

		for (int i =0; large_token_buffer.command_list[i] != NULL; i++) {
			small_token_buffer = str_filler(large_token_buffer.command_list[i], " ");
			
			if (strcmp(small_token_buffer.command_list[0], "exit") == 0) {
				exit = 1;
				//printf("\n");
				//free_command_line(&small_token_buffer);
				//memset(&small_token_buffer, 0, 0);
				//free_command_line(&large_token_buffer);
				//memset(&large_token_buffer, 0, 0);
				//break;
			} else {
				commandCheck(small_token_buffer);
			}

			free_command_line(&small_token_buffer);
			memset(&small_token_buffer, 0, 0);
		}
		free_command_line(&large_token_buffer);
		memset(&large_token_buffer, 0, 0);

	}
	free(line_buf);
}

void fileMode(char** argv) {
	FILE* inFPtr = fopen(argv[2], "r");
	FILE* output = fopen("output.txt", "w");

	freopen("output.txt", "w", stdout);
	size_t len = 128;
	char* line_buf = malloc(len);

	command_line large_token_buffer;
	command_line small_token_buffer;

	while (getline(&line_buf, &len, inFPtr) != -1) {
		//Line
		large_token_buffer = str_filler(line_buf, ";");

		for (int i = 0; large_token_buffer.command_list[i] != NULL; i++) {
			//line segment
			small_token_buffer = str_filler(large_token_buffer.command_list[i], " "); 
			 
			//command with any parameters, if any
			commandCheck(small_token_buffer);

			//free smaller tokens
			free_command_line(&small_token_buffer);
			memset(&small_token_buffer, 0, 0);
		}
		//free large token
		free_command_line(&large_token_buffer);
		memset(&large_token_buffer, 0, 0);
	}
	fclose(inFPtr);
	fclose(output);
	free(line_buf);
}

void commandCheck(command_line phrase) {
	//no parameter: ls, pwd
	//one parameter: mkdir, cd, rm, cat
	//two parameter: cp, mv
	//
	//errors:
	//Error! Unrecognized command: *token*
	//Error! Unsupported parameters for command: *token*
	
	char* command = phrase.command_list[0];

	if (strcmp(command, "ls") == 0) {
		if (parameterError(phrase, 0)) {
			listDir();
		}
	} else if (strcmp(command, "pwd") == 0) {
		if (parameterError(phrase, 0)) {
			showCurrentDir();
		}
	} else if(strcmp(command, "mkdir") == 0) {
		if (parameterError(phrase, 1)) {
			makeDir(phrase.command_list[1]);
		}
	} else if(strcmp(command, "cd") == 0) {
		if (parameterError(phrase, 1)) {
			changeDir(phrase.command_list[1]);
		}
	} else if(strcmp(command, "cp") == 0) {
		if (parameterError(phrase, 2)) {
			copyFile(phrase.command_list[1], phrase.command_list[2]);
		}
	} else if(strcmp(command, "mv") == 0) {
		if (parameterError(phrase, 2)) {
			moveFile(phrase.command_list[1], phrase.command_list[2]);
		}
	} else if(strcmp(command, "rm") == 0) {
		if (parameterError(phrase, 1)){
			deleteFile(phrase.command_list[1]);
		}
	} else if(strcmp(command, "cat") == 0) {
		if (parameterError(phrase, 1)) {
			displayFile(phrase.command_list[1]);
		}
	} else {
		printf("Error! Unrecognized command: %s\n", command);
	}	
}

int parameterError(command_line phrase, int quantity) {
	if (quantity != (phrase.num_token - 2)) {
		printf("Error! Unsupported parameters for command: %s\n", phrase.command_list[0]);
		return 0;
	}
	return 1;
}

