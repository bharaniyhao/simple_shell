#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void)
{
    char *command;
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

        pid_t child_pid = fork();
        if (child_pid == -1)
        {
            perror("fork");
        }
        if (child_pid == 0)
        {
          
            if (execve(command, NULL, NULL) == -1)
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
    return (0);
}

