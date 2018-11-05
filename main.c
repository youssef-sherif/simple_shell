#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
* Constant definitions
*/
#define MAX_INSTR_SIZE 20
#define ARG_SIZE 20
#define MAX_NO_OF_ARGS 10
#define MAX_PATH_SIZE 20


/*
*  Functions prototypes
*/

void log_process_handler(int sig, siginfo_t siginfo);
char * read_instr(int *in_background, int *has_args);
char * read_path();
char ** read_args(char * instr, int *in_background, int has_args);
int is_buit_in(char *instr);
void exec_built_in(char * instr);
pid_t terminate();

/*
*  Functions definitions
*/

void log_process_handler(int sig, siginfo_t siginfo)
{

    FILE * fp;
    fp = fopen( "processes", "a" );

    fprintf(fp, "logging process ... \n");

    fclose(fp);

}

pid_t terminate()
{
    pid_t pid = wait(NULL);

    if(pid == -1)
    {
        printf("No running processes \n");
        return pid;
    }

    FILE * fp;
    fp = fopen( "terminated_processes", "a" );

    fprintf(fp, "terminated process %d ... \n", pid);

    fclose(fp);

    printf("Process %d terminated \n", pid);

    return pid;
}

char * read_path()
{
    char * path = (char *) malloc(sizeof(char) * MAX_PATH_SIZE);

    int c, i = 0;

    while(1)
    {
        c = getchar();

        // Ignore white spaces
        if(c == ' ')
        {
            if(i != 0)
            {
                break;
            }
            continue;
        }

        else if(c == '\n')
        {
            break;
        }

        path[i] = c;
        path[++i] = '\0';
    }

    return path;
}

int is_built_in(char * instr)
{
    if(!strcmp(instr, "exit\0"))
    {
        return 1;
    }
    else if(!strcmp(instr, "cd"))
    {
        return 1;
    }

    return 0;
}

void exec_built_in(char * instr)
{
    if(!strcmp(instr, "exit\0"))
    {
        pid_t pid = 0;
        exit(1);
    }
    else if(!strcmp(instr, "cd"))
    {
        char * path = read_path();
        if( chdir(path) != 0 )
        {
            printf("Unkown directory: %s \n", path);
        }
        free(path);
    }
}

char * read_instr(int * in_background, int * has_args)
{
    char * instr = (char *) malloc(sizeof(char) * MAX_INSTR_SIZE);

    int c, i = 0;

    while(1)
    {
        c = getchar();

        // Ignore white spaces
        if(c == ' ')
        {
            if(i != 0)
            {
                *has_args = 1;

                break;
            }
        }

        // Check in background
        else if(c == '&')
        {
            *has_args = 0;
            *in_background = 1;

            break;
        }

        else if(c == '\n')
        {
            *has_args = 0;

            break;
        }
        else
        {
            instr[i] = c;
            i++;
        }
    }
    instr[i] = '\0';

    return instr;
}

char ** read_args(char * instr, int *in_background, int has_args)
{
    char ** args = (char **) malloc( sizeof(char *) * MAX_NO_OF_ARGS );
    args[0] = (char *) malloc( sizeof(char) * ARG_SIZE );
    strcpy(args[0], instr);

    if(*in_background || !has_args)
    {
        return args;
    }

    int c = ' ' , i = 0, j = 0;

    while(1)
    {
        // Ignore white spaces
        if(c == ' ')
        {
            c = getchar();
            if(c == '\n')
            {
                break;
            }

            else if(c != ' ')
            {
                if(c == '&')
                {
                    *in_background = 1;
                    break;
                }
                i++;
                j = 0;
                args[i] = (char *) malloc( sizeof(char) * ARG_SIZE );
            }
            continue;
        }

        else if(c == '\n')
        {
            break;
        }

        args[i][j] = c;
        args[i][++j] = '\0';

        c = getchar();

        // Check in background
        if(c == '&')
        {
            *in_background = 1;
            break;
        }
    }

    return args;
}


int main()
{
    char *instr;
    char ** args;

    struct sigaction sa;
    struct siginfo_t * si;

    sa.sa_handler = log_process_handler;

    int in_background = 0, i, built_in = 0, has_args = 0, status = 0;

    printf("Welcome to Simple Shell! \n");
    printf(">>");

    while(1)
    {
        has_args = 0;
        in_background = 0;

        instr = read_instr(&in_background, &has_args);

        built_in = is_built_in(instr);

        if(built_in)
        {
            exec_built_in(instr);
            printf(">>");

            continue;
        }

        args = read_args(instr, &in_background, has_args);

        pid_t childId = fork();

        sigaction(SIGCHLD, &sa, NULL);

        if(childId < 0)
        {
            printf("Error forking parent \n");
        }
        else if(childId == 0)
        {
            // Child process

            if( execvp(instr, args) == -1 )
            {
                if(strcmp(instr, "\0")){
                    printf("Instruction not found: %s \n" , instr);
                }
                printf(">>");
            }

        }
        else
        {
            // Parent process

            if( !in_background )
            {
                pid_t pid = 0;
                while(pid != -1)
                {
                    pid = terminate();
                }
                printf(">>");
            }
        }

        free(instr);
        for(i = 0; i < MAX_NO_OF_ARGS; i++)
        {
            args[i] = NULL;
        }
        free(args);
    }

    return 0;
}
