/*
 * OStask.h
 *
 *
 * Author: Taras Nikos
 */

#ifndef OSTASK_H_
#define OSTASK_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_COMMAND_SIZE 100 /*!< Maximum allowed command size. */
#define MAX_TOKEN_SIZE 50    /*!< Maximum allowed size of each of the parsed tokens. */
#define MAX_PARSED_TOKENS 5  /*!< Maximum number of parsed tokens.*/
#define BACKGROUND 1         /*!< Used when a binary needs to be executed in the background */
#define FOREGROUND 0         /*!< Used when a binary needs to be executed in the foreground */

/**
* @brief Handles the SIGINT signal caused by Ctrl-C combination
*
* @details The function catches the Ctrl-C combination
* and prints a message before terminating the shell.
*
* @param sig [int] : The signal number
**/
void  INThandler(int sig);

/**
* @brief Implements the "Change directory" command of the shell.
*
* @details The function uses "chdir" c command to provide the functionality of
* "cd" command in the shell. Returns 0 when directory changes successfully, and -1
* if directory could not be changed.
*
* @param pth [char *] : This is the new path chosen by the user, via the cd command.
*
* @return result : 0 for success, -1 for error
**/

int cd(char *pth);

/**
* @brief Executes a command in the foreground or the background
*
* @details The function forks a new child process. This new child process uses "execvp"
* c command to execute the command stored at the zero position of the argv argument. If
* the command needs to be executed in the foreground, the functions forces the parent
* process to wait until the child process exits. If the command has to be executed in the
* background, the parent process continues the shell loop.
*
* @param argv [char **] : This is the array of tokens parsed by the user command.
* Includes the command that the user needs to execute and the list of parameters
* to be passed. Terminates with a NULL token.
*
* @param method [int] : Can receive two values, BACKGROUND or FOREGROUND. This is needed
* to force the execution to the background or the foreground.
**/
void execute(char **argv, int method);


#endif /* OSTASK_H_ */
