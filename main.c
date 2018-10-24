#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* Constant definitions
*/
#define MAX_INSTR_SIZE 20
#define ARG_SIZE 5
#define MAX_NO_OF_ARGS 10


/*
*  Functions prototypes
*/

void log_process(int pid);
char * read_instr(int *in_background);
char ** read_args(int in_background);

/*
*  Functions definitions
*/

void log_process(int sig)
{
    printf("logging...\n");
}

char * read_instr(int * in_background)
{
    char * instr = (char *) malloc(sizeof(char) * MAX_INSTR_SIZE);

    int c, i = 0, has_args;

    do
    {
        c = getchar();

        if(c == ' ')
        {
            if(i != 0)
            {
                break;
            }
        }
        else if(c == '&')
        {
            *in_background = 1;
        }
        else
        {
            instr[i] = c;

            i++;
        }
    } while(c != '\n');

    return instr;
}

char ** read_args(int in_background)
{
    char ** args = (char **) malloc(sizeof(char *) * MAX_NO_OF_ARGS);
    int c, i = -1, j = 0;

    if(in_background)
    {
        args[0] = (char *) realloc(args[0], sizeof(char) * ARG_SIZE );
        return args;
    }

    do {
        c = getchar();

        // Ignore white spaces
        if(c == ' ')
        {
            do {
                c = getchar();
            } while(c == ' ');
            i++;
            j = 0;
        }

        if(i == -1)
        {
            i = 0;
        }

        args[i] = (char *) realloc(args[0], sizeof(char) * ARG_SIZE );
        args[i][j] = c;
        printf("arg[%d][%d] = %c \n", i, j, args[i][j]);

        j++;

    } while(c != '\n');

    return args;
}

void main()
{
    char *instr;
    char ** args;

    int in_background, status;

    do
    {
        in_background = 0;
        status = 0;
        printf("\n$");

        instr = read_instr(&in_background);
        args = read_args(in_background);

        printf("instruction: %s \n background: %d \n", instr, in_background);

        pid_t childId = fork();

        signal(SIGCHLD, log_process);

        if(childId < 0)
        {
            printf("Error forking parent\n");
        }
        else if(childId == 0)
        {
            // Child process

            if( execvp(instr, args) == -1 )
            {
                printf("Instruction not found: %s" , instr);
            }
        }
        else
        {
            // Parent process

            if( !in_background )
            {
                wait(&status);
            }
        }

        free(instr);
        free(args);

    } while( strcmp( instr, "exit\0" ) );

    exit(EXIT_SUCCESS);

}
