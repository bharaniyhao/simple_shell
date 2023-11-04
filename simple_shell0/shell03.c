#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

/**
 * check_command - checks if a command exists in PATH
 * @command: the command to check
 * @path: the PATH environment variable
 * Return: 1 if found, 0 if not found
 */
int check_command(char *command, char *path)
{
    char *token;
    char *dir;
    char *path_copy;

    path_copy = strdup(path);
    token = strtok(path_copy, ":");

    while (token != NULL)
    {
        dir = strcat(token, "/");
        dir = strcat(dir, command);
        if (access(dir, X_OK) == 0)
        {
            free(path_copy);
            return (1);
        }
        token = strtok(NULL, ":");
    }

    free(path_copy);
    return (0);
}

/**
 * main - Entry point of the shell
 * @argc: The argument count
 * @argv: The argument vector
 * @env: The environment variables
 * Return: 0 on success, otherwise an error code
 */
int main(int argc, char **argv, char **env)
{
    char *command = NULL;
    size_t bufsize = 0;
    ssize_t characters;
    int status;
    char *path = getenv("PATH");

    while (1)
    {
        printf("$ "); // Display the prompt
        characters = getline(&command, &bufsize, stdin);

        if (characters == -1) // Handle "end of file" (Ctrl+D)
        {
            printf("\n");
            free(command);
            exit(0);
        }

        command[characters - 1] = '\0'; // Remove the newline character

        if (check_command(command, path))
        {
            pid_t child_pid = fork();
            if (child_pid == -1)
            {
                perror("fork");
            }
            if (child_pid == 0)
            {
                // Child process
                if (execve(command, argv, env) == -1)
                {
                    perror("execve");
                    free(command);
                    exit(1);
                }
            }
            else
            {
                // Parent process
                wait(&status);
            }
        }
        else
        {
            fprintf(stderr, "%s: command not found\n", command);
        }
    }

    free(command);
    return 0;
}
