#include "OStask.h"



void  INThandler(int sig){

     signal(sig, SIG_IGN);

     printf("\n\nShell will now terminate.\n\n");
     exit(0);
}



int cd(char *pth){

	int result;
    char path[1000];
    strcpy(path,pth);


    char cwd[256];
    getcwd(cwd,sizeof(cwd));

    strcat(cwd,"/");
    strcat(cwd,path);
    result = chdir(cwd);

    return result;
  }


void execute(char **argv, int method){

     pid_t  pid;
     int    status;

     if ((pid = fork()) < 0) {     /* fork a child process           */
          printf("\n*** ERROR: Forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {          /* for the child process:         */
          if (execvp(*argv, argv) < 0) {     /* execute the command  */
               printf("\n*** ERROR: Command not found or execution failed\n");
               exit(1);
          }
          exit(0);
     }
     else {                                  /* for the parent:      */
    	 if(method == BACKGROUND){
    		 printf("\nProcess with pid [%d] started.\n",pid);
    	 }
    	 else{
          while (wait(&status) != pid);       /* wait for completion  */
    	 }
     }
}

/**
* @brief Implements the endless loop of the shell.
*
* @details  The function has a main loop which prints the shell prompt and waits
* for a command input by the user. In the loop, the command is parsed into tokens by using
* space as the seperating character. Then, the appropriate function is called to run this
* command. If the & symbol is found in the last token, main() informs the execute() function
* that the command has to be executed in the background.
*
**/

int main(void)
{
	signal(SIGINT, INThandler);  //Needed for the signal handler function

    char *input = (char *)malloc(MAX_COMMAND_SIZE*sizeof(char)); //user input
    char *token[MAX_PARSED_TOKENS]; //array of strings which holds the parsed tokens
    char cwd[256]; //current working directory

    int token_length[5]; //holds the length of each of the 5 tokens
    int i;	//used for counting in "for" loops or indexing in arrays
    int length = 0;	//Holds the length of the user input
    int current_token_num; //Holds the index of the token being processed
    int char_count; //holds the index of the token character being processed
    int found_space;  //flag, true when a space is found. Used to find more than one spaces
    int active_tokens;  //holds how many of the 5 tokens have non-zero length
    int cd_result; //  holds the result (success or not) of the "cd" function

//TODO add Tab support in spaces removal


    system("clear");  //Clears screen for the new shell to start

    while(1){  //Here starts the main loop

    	getcwd(cwd,sizeof(cwd));  //Gets current working directory
    	printf("OStaskShell:%s$ ", cwd );  //Prints shell prompt

    	fgets (input, MAX_COMMAND_SIZE * sizeof(char) , stdin); //Reads user input
    	length = strlen(input); //Gets the length of the user input

        //tokens initialization
        for (i=0; i<MAX_PARSED_TOKENS; i++){
        	token[i] = (char*) malloc(MAX_TOKEN_SIZE);
        	token_length[i] = 0;
        }

    	//parsing command and possible parameters
    	found_space = 1;
    	current_token_num = 0;
    	char_count = 0;
    	for (i = 0; i < length; i++) {
    		if (current_token_num>4){  //if user entered more than 4 parameters, only 4 are stored
    			printf("\n*** ERROR: Overnumbered parameters, only 4 where stored\n");
    			break;
    		}
    		if (input[i] == ' ') {
    			if (!found_space){      //if multiple spaces, then we ignore them
    				current_token_num++;
    				char_count = 0;
    				found_space = 1;
    			}
    		}
    		else{
    			found_space = 0;
    			token[current_token_num][char_count] = input[i];
    			char_count++;
    			token_length[current_token_num] = char_count;
    		}
    	}
    	token_length[current_token_num]--;

    	//find active tokens, add NULL to last parameter
    	active_tokens = 0;
    	for(i=0; i<MAX_PARSED_TOKENS; i++){
    		if (token_length[i] != 0){
    			active_tokens++;
    			token[i][token_length[i]] = '\0';
    			token_length[i]++;
    		}
    	}
    	token[active_tokens] = (char *)0;


    	// check if command is "cd" and run it
    	if (!strcmp(token[0],"cd")){
    		cd_result = cd(token[1]);
    		if (cd_result == -1){
    			printf("\n*** ERROR: Cannot change to that directory\n");
    		}
    	}

    	// check if command is exit and run it
    	else if (!strcmp(token[0],"exit")){
    		exit(0);
    	}

    	// when a binary needs to be executed
    	else{
    		if(token[active_tokens-1][token_length[active_tokens-1]-2] == '&'){ //if last symbol is &

    			token[active_tokens-1][token_length[active_tokens-1]-2] = '\0'; //make it NULL
    			if(token_length[active_tokens-1] == 2){ // if the last token stores only & character
    				token[active_tokens-1] = (char *)0; // make it the last argument by NULLing it.
    			}
    			execute(token,BACKGROUND); //1 to execute in background
    		}
    		else{
    			execute(token,FOREGROUND); //0 to execute in foreground
    		}
    	}

    	printf("\n");
    }
    return 0;
}
