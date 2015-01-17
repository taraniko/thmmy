#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


#define MAX_COMMAND_SIZE 100
#define MAX_TOKEN_SIZE 20
#define MAX_PARSED_TOKENS 5

//void sig_handler(int signo)
//{
//  if (signo == SIGINT)
//    printf("User asked for termination.\n");
//  	printf("The shell is now terminating...\n\n\n");
//  	exit(0);
//}

void  INThandler(int sig)
{

     signal(sig, SIG_IGN);
     printf("\n\nUser asked for termination.\n");
     printf("The shell is now terminating...\n\n\n");
     exit(0);
}


int main(void)
{
	signal(SIGINT, INThandler);
    char *input = (char *)malloc(MAX_COMMAND_SIZE*sizeof(char));
    char *token[MAX_PARSED_TOKENS];
    int token_length[5];
    int i;
    int length = 0;
    int current_token_num;
    int char_count;
    int found_space;
    int active_tokens;
    int error = 0;



    system("clear");

    //Main loop
    while(1){


    	printf("OStaskShell:~$ ");

    	fgets (input, MAX_COMMAND_SIZE * sizeof(char) , stdin);
    	length = strlen(input);

        //tokens initialization
        for (i=0; i<MAX_PARSED_TOKENS; i++){
        	token[i] = (char*) malloc(MAX_TOKEN_SIZE);
        	token_length[i] = 0;
        }

    	//TODO add support for & symbol

    	//parsing command and possible parameters
    	found_space = 1;
    	current_token_num = 0;
    	char_count = 0;
    	for (i = 0; i < length; i++) {
    		if (current_token_num>4){
    			printf("\nOvernumbered parameters, only 4 where stored\n");

    			break;
    			//current_token_num = 4;
    			//found_space = 1;
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

    	active_tokens = 0;
    	for(i=0; i<MAX_PARSED_TOKENS; i++){
    		if (token_length[i] != 0){
    			active_tokens++;
    			token[i][token_length[i]] = NULL;
    			token_length[i]++;
    		}
    	}



    	printf("\n\nActive tokens: %d", active_tokens);

    	for( i=0; i<active_tokens; i++){
    		printf("\nToken[%d] = %s",i,token[i]);
    	}

    	for( i=0; i<MAX_PARSED_TOKENS; i++){
    	    printf("\nToken[%d] length = %d",i,token_length[i]);
    	}

    	printf("\n");
    }
    free(input);


    return 0;
}
