#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_COMMAND_SIZE 100

int main(void)
{
    char *input = (char *)malloc(MAX_COMMAND_SIZE*sizeof(char));
    int length = 0;
    int i = 0;

    //Main loop
    while(1){

    	system("clear");
    	printf("OStaskShell:~$ ");
    	fgets (input, MAX_COMMAND_SIZE * sizeof(char) , stdin);
    	length = strlen(input);

    	for (i = 0; i < length; i++) {
    		if (input[i] == ' ') {
    			printf("\n");
    		}
    		else{
    			printf("%c", input[i]);
    		}
    	}


    }
    free(input);


    return 0;
}
