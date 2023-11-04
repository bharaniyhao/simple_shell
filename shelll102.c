#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

// ... (rest of your code)

/**
 * execute_file_commands - execute commands from a file
 * @filename: the name of the file containing commands
 * @env: environment variables
 * @exit_status: pointer to store exit status of the last command
 */
void execute_file_commands(char *filename, char **env, int *exit_status) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    
    while ((read = getline(&line, &len, file)) != -1) {
        if (line[read - 1] == '\n') {
            line[read - 1] = '\0'; // Remove the newline character
        }

        if (check_command(line, getenv("PATH"))) {
            char **args = malloc(sizeof(char *) * 32);
            char *subtoken = strtok(line, " ");
            int arg_count = 0;

            while (subtoken != NULL) {
                args[arg_count] = subtoken;
                subtoken = strtok(NULL, " ");
                arg_count++;
            }

            args[arg_count] = NULL;

            // Replace variables in the command
            replace_variables(args[0], *exit_status, getpid());

            execute_command(args[0], args, env, exit_status);
            free(args);
        } else {
            fprintf(stderr, "%s: command not found\n", line);
            *exit_status = 127; // Set a standard exit status for command not found
        }
    }

    free(line);
    fclose(file);
}

/**
 * main - Entry point of the shell
 * @argc: The argument count
 * @argv: The argument vector
 * @env: The environment variables
 * Return: 0 on success, otherwise an error code
 */
int main(int argc, char **argv, char **env) {
    if (argc > 1) {
        // If a filename is provided as a command-line argument, execute commands from the file
        execute_file_commands(argv[1], env, &last_exit_status);
        return last_exit_status;
    } else {
        // Enter interactive mode with a prompt
        while (1) {
            printf("$ "); // Display the prompt
            // ... (rest of your interactive mode code, as before)
        }
    }
}

