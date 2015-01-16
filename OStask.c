#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(void)
{
    char *str = (char *)malloc(100*sizeof(char));
    char *tok = NULL;
    int len = 0;
    printf("Gimme a string bitch: ");
    fgets (str, 100*sizeof(char), stdin);
    len = strlen(str);
    printf("\nLength: %d\n", len);

    printf("string before strtok(): %s\n", str);
    tok = strtok(str, " ");
    while (tok) {
        printf("Token: %s\n", tok);
        tok = strtok(NULL, " ");
    }

    printf("Print mangled string after strtok()\n");
    int i;
    printf("New LENGTH = %d\n\n",len);
    for (i = 0; i < len; i++) {
        if (str[i] == '\0') {
            printf("\n");
        } else {
            printf("%c", str[i]);
        }
    }
    printf("\n");
    free(str);

    return 0;
}
