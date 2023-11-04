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
 * execute_command - execute a command
 * @command: the command to execute
 * @argv: argument vector
 * @env: environment variables
 */
void execute_command(char *command, char **argv, char **env)
{
    pid_t child_pid;
    int status;

    child_pid = fork();
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
            exit(1);
        }
    }
    else
    {
        // Parent process
        wait(&status);
    }
}

/**
 * print_env - print the environment variables
 * @env: environment variables
 */
void print_env(char **env)
{
    int i = 0;
    while (env[i] != NULL)
    {
        printf("%s\n", env[i]);
        i++;
    }
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

        if (strcmp(command, "exit") == 0)
        {
            free(command);
            exit(0);
        }
        else if (strcmp(command, "env") == 0)
        {
            print_env(env);
        }
        else if (check_command(command, path))
        {
            execute_command(command, argv, env);
        }
        else
        {
            fprintf(stderr, "%s: command not found\n", command);
        }
    }

    free(command);
    return 0;
}

