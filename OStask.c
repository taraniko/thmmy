#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_COMMAND_SIZE 100
#define MAX_TOKEN_SIZE 20
#define MAX_PARSED_TOKENS 5

int main(void)
{
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

    //tokens initialization
    for (i=0; i<MAX_PARSED_TOKENS; i++){
    	token[i] = (char*) malloc(MAX_TOKEN_SIZE);
    	token_length[i] = 0;
    }


    //Main loop
    while(1){

    	system("clear");
    	printf("OStaskShell:~$ ");

    	fgets (input, MAX_COMMAND_SIZE * sizeof(char) , stdin);
    	length = strlen(input);

    	//TODO add support for & symbol

    	//parsing command and possible parameters
    	found_space = 1;
    	current_token_num = 0;
    	char_count = 0;
    	for (i = 0; i < length; i++) {
    		if (current_token_num>4){
    			printf("\nOvernumbered parameters, only 4 where stored\n");
    			error=1;
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
//    	for(i=0; i<MAX_PARSED_TOKENS; i++){
//    		token[i][token_length[i]] = '\0';
//    		if (token_length[i] != 0){
//    			active_tokens++;
//    		}
//    	}



//    	printf("\n\nActive tokens: %d", active_tokens);
//
//    	for( i=0; i<MAX_PARSED_TOKENS; i++){
//    		printf("\nToken[%d] = %s",i,token[i]);
//    	}
//
    	for( i=0; i<MAX_PARSED_TOKENS; i++){
    	    printf("\nToken[%d] length = %d",i,token_length[i]);
    	}

    	printf("\n");
    }
    free(input);


    return 0;
}
