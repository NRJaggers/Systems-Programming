#include <stdio.h>
#include <string.h>

#define BUFFERSIZE 100 

//Given args, returns each token in arg_tokens
char** tokenizeArgs(char *args, char **arg_tokens)
{
    int num_args = 0; //num of arguments entered
    char *token = strtok(args, " ");

    while(token != NULL)
    {
        arg_tokens[num_args] = token;
        token = strtok(NULL, " ");
        num_args++;
    }
    return arg_tokens;
}

int getArgLen(char **arg_tokens)
{
    int i = 0;
    while (arg_tokens[i])
    {
        i++;
    }
    return i;
}

int main()
{
    char args[BUFFERSIZE];
    char args_cpy[BUFFERSIZE];
    char *arg_tokens[3] = {0};
    fgets(args, BUFFERSIZE, stdin);
    strcpy(args_cpy, args);
    printf("Cool string: %s\n", args);
    tokenizeArgs(args, arg_tokens);
    printf("0: %s\n", arg_tokens[0]);
    printf("1: %s\n", arg_tokens[1]);
    printf("Cool string: %s\n", args);
    printf("Len: %d\n",getArgLen(arg_tokens));

    return 0;
}
