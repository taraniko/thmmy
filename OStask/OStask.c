#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


#define MAX_COMMAND_SIZE 100
#define MAX_TOKEN_SIZE 50
#define MAX_PARSED_TOKENS 5

//static pid_t pid;

//TODO fix ls & instead of ls&  ISSUE




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


void  execute(char **argv, int background)
{
     pid_t  pid;


     int    status;

     printf("\nToken[0] = %s\n", argv[0]);
     printf("Token[1] = %s\n", argv[1]);

     if ((pid = fork()) < 0) {     /* fork a child process           */
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {          /* for the child process:         */
          if (execvp(*argv, argv) < 0) {     /* execute the command  */
               printf("*** ERROR: exec failed\n");
               exit(1);
          }
          exit(0);
     }
     else {                                  /* for the parent:      */
    	 if(background){
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
    int token_length[5];
    int i;
    int length = 0;
    int current_token_num;
    int char_count;
    int found_space;
    int active_tokens;
    int cd_result;
    char cwd[256];




    strcat(cwd,"/");
    chdir(cwd);
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
    			token[i][token_length[i]] =NULL;
    			token_length[i]++;
    		}
    	}
    	token[active_tokens] = (char *)0;

        printf("\nToken[0] = %s\n", token[0]);
        printf("Token[1] = %s\n", token[1]);
        printf("Token[1] length = %d", token_length[1]);

//    	printf("\n\nActive tokens: %d", active_tokens);
//
//    	for( i=0; i<active_tokens; i++){
//    		printf("\nToken[%d] = %s",i,token[i]);
//    	}
//
//    	for( i=0; i<MAX_PARSED_TOKENS; i++){
//    	    printf("\nToken[%d] length = %d",i,token_length[i]);
//    	}

    	// check if command is "cd"
    	if (!strcmp(token[0],"cd")){
    		cd_result = cd(token[1]);
    		if (cd_result == -1){
    			printf("Cannot change to that directory");
    		}
    	}

    	// check if command is exit
    	else if (!strcmp(token[0],"exit")){
    		exit(0);
    	}

    	else{
    		//TODO & MUST BE REMOVED
    		if(token[active_tokens-1][token_length[active_tokens-1]-2] == '&'){
    			printf("Mphka\n");
    			token[active_tokens-1][token_length[active_tokens-1]-2] = '\0';
    			printf("KAi to allaksa\n");
    			execute(token,1); //1 to execute in background
    		}
    		else{
    			execute(token,0);
    		}
    	}



    	//

    	printf("\n");
    }
    free(input);


    return 0;
}
