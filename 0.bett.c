#include "holberton.h"
/**
 * main - Entry point of the shell program
 *
 * Return: 0 on success, otherwise an error code
 */
int main(void)
{
    char *user_input;

    while (1)
    {
        user_input = read_user_input();
        if (user_input == NULL)
            break;
        
        process_user_input(user_input);
        free(user_input);
    }

    return (0);
}
