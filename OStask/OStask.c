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



void  INThandler(int sig)
{

     signal(sig, SIG_IGN);
     printf("\n\nUser asked for termination.\n");
     printf("The shell is now terminating...\n\n\n");
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


void execute(char **argv, int method)
{
     pid_t  pid;
     int    status;

     if ((pid = fork()) < 0) {     /* fork a child process           */
          printf("*** ERROR: Forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {          /* for the child process:         */
          if (execvp(*argv, argv) < 0) {     /* execute the command  */
               printf("*** ERROR: Command not found or execution failed\n");
               exit(1);
          }
          exit(0);
     }
     else {                                  /* for the parent:      */
    	 if(method == BACKGROUND){
    		 printf("\nProcess with pid [%d] started.",pid);
    	 }
    	 else{
          while (wait(&status) != pid);       /* wait for completion  */
    	 }
     }
}


int main(void)
{
	signal(SIGINT, INThandler);

    char *input = (char *)malloc(MAX_COMMAND_SIZE*sizeof(char));
    char *token[MAX_PARSED_TOKENS];
    char cwd[256];

    int token_length[5];
    int i;
    int length = 0;
    int current_token_num;
    int char_count;
    int found_space;
    int active_tokens;
    int cd_result;


    system("clear");

    //Main loop
    while(1){

    	getcwd(cwd,sizeof(cwd));
    	printf("OStaskShell:%s$ ", cwd );

    	fgets (input, MAX_COMMAND_SIZE * sizeof(char) , stdin);
    	length = strlen(input);

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
    		if (current_token_num>4){
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


    	// check if command is "cd"
    	if (!strcmp(token[0],"cd")){
    		cd_result = cd(token[1]);
    		if (cd_result == -1){
    			printf("*** ERROR: Cannot change to that directory\n");
    		}
    	}

    	// check if command is exit
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



    	//

    	printf("\n");
    }
    free(input);


    return 0;
}
