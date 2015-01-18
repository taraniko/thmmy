/*
 * OStask.h
 *
 *
 * Author: Taras Nick
 */

#ifndef OSTASK_H_
#define OSTASK_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_COMMAND_SIZE 100
#define MAX_TOKEN_SIZE 50
#define MAX_PARSED_TOKENS 5
#define BACKGROUND 1
#define FOREGROUND 0

void  INThandler(int sig);

int cd(char *pth);

void execute(char **argv, int method);


#endif /* OSTASK_H_ */
