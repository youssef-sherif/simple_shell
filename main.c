#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* Constant definitions
*/
#define CMD_SIZE 10
#define ARG_SIZE 5
#define MAX_NO_OF_ARGS 10

/*
*  Structs
*/

struct Command
{
   char *  cmd;
   char ** args;
   int     type;
} command;

/*
*  Functions prototypes
*/
char * read_cmd(struct Command * command);
char ** read_args(char * cmd);

/*
*  Functions definitions
*/
char * read_cmd(struct Command * command)
{
    char *  cmd  = malloc(sizeof(char) * CMD_SIZE);

    if(!cmd)
    {
        printf("Error");
        exit(EXIT_FAILURE);
    }

    int c, i = 0;

    while(1)
    {
        c = getchar();

        if(c == ' ')
        {
            if(strlen(cmd) == 0)
            {
                continue;
            }
            command->args = read_args(cmd);
            return cmd;
        }
        else if(c == '\n')
        {
            return cmd;
        }

        cmd[i] = c;

        i++;
    }

    return cmd;
}

char ** read_args(char * cmd)
{
    char ** args = malloc(sizeof(char **) * MAX_NO_OF_ARGS);

    int c = ' ', i = -1, j = 0;
    while(1)
    {
        if(c == ' ')
        {
            c = getchar();
            if(c != ' ')
            {
                i++;
                j = 0;
            }
        }
        else if(c == '\n')
        {
            return args;
        }
        else
        {
            c = getchar();
        }

        args[i] = malloc(sizeof(char *) * ARG_SIZE);
        args[i][j] = c;

        printf("%d %d %c \n", i, j, args[i][j]);

        j++;
    }

    return args;
}

int main()
{
    struct Command command;

    do
    {
        printf("$");
        command.cmd = read_cmd( &command );

    } while( strcmp(command.cmd, "exit\0") );

    exit(EXIT_SUCCESS);

    return 0;
}
