#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno>

// Define a structure to hold alias key-value pairs
struct Alias {
    char *name;
    char *value;
};

/**
 * check_command - checks if a command exists in PATH
 * @command: the command to check
 * @path: the PATH environment variable
 * Return: 1 if found, 0 if not found
 */
int check_command(char *command, char *path) {
    char *token;
    char *dir;
    char *path_copy;

    path_copy = strdup(path);
    token = path_copy;

    while (*token != '\0') {
        if (*token == ':') {
            *token = '\0';
            dir = strcat(token, "/");
            dir = strcat(dir, command);
            if (access(dir, X_OK) == 0) {
                free(path_copy);
                return 1;
            }
            token++;
        } else {
            token++;
        }
    }

    free(path_copy);
    return 0;
}

/**
 * execute_command - execute a command
 * @command: the command to execute
 * @argv: argument vector
 * @env: environment variables
 */
void execute_command(char *command, char **argv, char **env) {
    pid_t child_pid;
    int status;

    child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
    }
    if (child_pid == 0) {
        // Child process
        if (execve(command, argv, env) == -1) {
            perror("execve");
            exit(1);
        }
    } else {
        // Parent process
        wait(&status);
    }
}

/**
 * print_aliases - print aliases in the specified format
 * @aliases: array of alias structures
 * @alias_count: number of aliases
 * @alias_names: names of aliases to print (NULL for all)
 */
void print_aliases(struct Alias *aliases, int alias_count, char **alias_names) {
    for (int i = 0; i < alias_count; i++) {
        if (alias_names == NULL || alias_names[0] == NULL) {
            printf("%s='%s'\n", aliases[i].name, aliases[i].value);
        } else {
            for (int j = 0; alias_names[j] != NULL; j++) {
                if (strcmp(alias_names[j], aliases[i].name) == 0) {
                    printf("%s='%s'\n", aliases[i].name, aliases[i].value);
                    break;
                }
            }
        }
    }
}

/**
 * add_alias - add or update an alias in the alias array
 * @aliases: array of alias structures
 * @alias_count: number of aliases
 * @name: alias name
 * @value: alias value
 */
void add_alias(struct Alias *aliases, int *alias_count, char *name, char *value) {
    for (int i = 0; i < *alias_count; i++) {
        if (strcmp(aliases[i].name, name) == 0) {
            free(aliases[i].value);
            aliases[i].value = strdup(value);
            return;
        }
    }

    aliases[*alias_count].name = strdup(name);
    aliases[*alias_count].value = strdup(value);
    (*alias_count)++;
}

/**
 * main - Entry point of the shell
 * @argc: The argument count
 * @argv: The argument vector
 * @env: The environment variables
 * Return: 0 on success, otherwise an error code
 */
int main(int argc, char **argv, char **env) {
    char *command = NULL;
    size_t bufsize = 0;
    ssize_t characters;
    char *path = getenv("PATH");
    int last_exit_status = 0;

    // Define an array to store alias key-value pairs
    struct Alias aliases[32];
    int alias_count = 0;

    while (1) {
        printf("$ "); // Display the prompt
        characters = getline(&command, &bufsize, stdin);

        if (characters == -1) // Handle "end of file" (Ctrl+D)
        {
            printf("\n");
            free(command);
            exit(0);
        }

        command[characters - 1] = '\0'; // Remove the newline character

        char *token = strtok(command, ";");
        while (token != NULL) {
            if (token[0] != ' ') {
                if (strcmp(token, "exit") == 0) {
                    char *status_str = token + 4;
                    int status = atoi(status_str);
                    free(command);
                    exit(status);
                } else if (strncmp(token, "setenv", 6) == 0) {
                    char *env_cmd = token + 6;
                    char *variable = strtok(env_cmd, " ");
                    char *value = strtok(NULL, " ");

                    if (variable != NULL && value != NULL) {
                        if (setenv(variable, value, 1) == -1) {
                            perror("setenv");
                        }
                    } else {
                        fprintf(stderr, "Usage: setenv VARIABLE VALUE\n");
                    }
                } else if (strncmp(token, "unsetenv", 8) == 0) {
                    char *env_cmd = token + 8;
                    char *variable = strtok(env_cmd, " ");

                    if (variable != NULL) {
                        if (unsetenv(variable) == -1) {
                            perror("unsetenv");
                        }
                    } else {
                        fprintf(stderr, "Usage: unsetenv VARIABLE\n");
                    }
                } else if (strncmp(token, "cd", 2) == 0) {
                    char *dir_cmd = token + 2;
                    char *directory = strtok(dir_cmd, " ");

                    if (directory == NULL)
                        directory = getenv("HOME");

                    char *cwd = getcwd(NULL, 0);

                    if (chdir(directory) == -1) {
                        fprintf(stderr, "cd: %s: %s\n", directory, strerror(errno));
                    } else {
                        setenv("OLDPWD", cwd, 1);
                        setenv("PWD", getcwd(NULL, 0), 1);
                    }

                    free(cwd);
                } else if (strncmp(token, "alias", 5) == 0) {
                    char *alias_cmd = token + 5;

                    if (alias_cmd[0] == '\0' || alias_cmd[0] == ' ') {
                        // Print all aliases
                        print_aliases(aliases, alias_count, NULL);
                    } else {
                        // Parse and process alias commands
                        char *alias_name = strtok(alias_cmd, "=' ");
                        char *alias_value = strtok(NULL, "=' ");

                        if (alias_name != NULL && alias_value != NULL) {
                            // Define or update an alias
                            add_alias(aliases, &alias_count, alias_name, alias_value);
                        } else {
                            // Print specific aliases
                            char *alias_names[32];
                            int alias_names_count = 0;
                            char *alias_names_cmd = strtok(alias_cmd, " ");

                            while (alias_names_cmd != NULL) {
                                alias_names[alias_names_count] = alias_names_cmd;
                                alias_names_count++;
                                alias_names_cmd = strtok(NULL, " ");
                            }
                            alias_names[alias_names_count] = NULL;

                            print_aliases(aliases, alias_count, alias_names);
                        }
                    }
                } else if (check_command(token, path)) {
                    char **args = malloc(sizeof(char *) * 32);
                    char *subtoken = strtok(token, " ");
                    int arg_count = 0;

                    while (subtoken != NULL) {
                        args[arg_count] = subtoken;
                        subtoken = strtok(NULL, " ");
                        arg_count++;
                    }

                    args[arg_count] = NULL;

                    execute_command(args[0], args, env);
                    free(args);
                } else {
                    fprintf(stderr, "%s: command not found\n", token);
                }
            }
            token = strtok(NULL, ";");
        }
    }

    // Free memory for alias values
    for (int i = 0; i < alias_count; i++) {
        free(aliases[i].name);
        free(aliases[i].value);
    }

    free(command);
    return 0;
}
