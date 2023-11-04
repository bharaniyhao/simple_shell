#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(void)
{
    char *command = NULL;
    size_t bufsize = 0;
    ssize_t characters;
    int status;

    while (1)
    {
        printf("$ "); 
        characters = getline(&command, &bufsize, stdin);

        if (characters == -1) 
        {
            printf("\n");
            free(command);
            exit(0);
        }

        command[characters - 1] = '\0';

        char *args[10]; 
        int arg_count = 0;

        char *token = strtok(command, " ");
        while (token != NULL && arg_count < 10)
        {
            args[arg_count] = token;
            token = strtok(NULL, " ");
            arg_count++;
        }

        args[arg_count] = NULL;

        pid_t child_pid = fork();
        if (child_pid == -1)
        {
            perror("fork");
        }
        if (child_pid == 0)
        {
           
            if (execve(args[0], args, NULL) == -1)
            {
                perror("execve");
                free(command);
                exit(1);
            }
        }
        else
        {
           
            wait(&status);
        }
    }

    free(command);
    return 0;
}

