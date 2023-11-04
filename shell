#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define BUFSIZE 1024

/**
 * _getchar - Read a character from standard input
 * Return: The character read, or EOF on failure
 */
int _getchar(void)
{
    static char buf[BUFSIZE];
    static size_t index = 0;
    static ssize_t size = 0;

    if (index == size || size == 0)
    {
        size = read(STDIN_FILENO, buf, BUFSIZE);
        index = 0;
    }

    if (size == -1 || (size == 0 && index == 0))
        return EOF;

    return buf[index++];
}

/**
 * read_line - Read a line from standard input
 * Return: A pointer to the read line, or NULL on failure
 */
char *read_line(void)
{
    char *line = NULL;
    size_t bufsize = 0;
    ssize_t characters;

    while (1)
    {
        int c = _getchar();

        if (c == EOF)
        {
            free(line);
            return NULL;
        }

        if (c == '\n')
        {
            if (line == NULL)
                line = strdup("");
            return line;
        }

        line = realloc(line, bufsize + 2);
        if (line == NULL)
            return NULL;

        line[bufsize] = (char)c;
        line[bufsize + 1] = '\0';
        bufsize++;
    }
}

int main(void)
{
    char *command;
    int status;

    while (1)
    {
        printf("$ "); // Display the prompt
        command = read_line();

        if (command == NULL)
        {
            printf("\n");
            exit(0);
        }

        if (strcmp(command, "exit") == 0)
        {
            free(command);
            exit(0);
        }
        else
        {
            pid_t child_pid = fork();
            if (child_pid == -1)
            {
                perror("fork");
            }
            if (child_pid == 0)
            {
                // Child process
                if (execlp(command, command, NULL) == -1)
                {
                    perror("execlp");
                    free(command);
                    exit(1);
                }
            }
            else
            {
                // Parent process
                wait(&status);
                free(command);
            }
        }
    }

    return 0;
}

