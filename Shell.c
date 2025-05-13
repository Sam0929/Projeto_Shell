#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#include "shell.h"

#define COMMAND_LENGTH 100

typedef struct {
    const char *command;
    void (*action)();  
} Command;


// Main function
int main(int argc, char *argv[]) {

    char entry[COMMAND_LENGTH];

    Command commands[] = {
        {"cd", cd},
        {"path", path},
        {"pwd", pwd},
        {"cat", cat},
        {"ls", ls},
        {"teste", teste},
        {"help", help},
        {"exit", exit_program},
    };

    int num_commands = sizeof(commands) / sizeof(commands[0]);  // Number of commands

    printf("SamTum Terminal\n\n");
    printf("Welcome To SamTum Terminal\n");
    printf("Type \"help\" for more things!\n\n");

    while(1) {

        printf(">>> ");
        scanf("%s", entry);  

    
        for (int i = 0; entry[i]; i++) {
            entry[i] = tolower(entry[i]);
        }

        int found = 0;

        for (int i = 0; i < num_commands; i++) {
            if (strcmp(entry, commands[i].command) == 0) {
                commands[i].action();  
                found = 1;
                break;
            }
        }

        if (!found) {
            printf("Invalid command. Type 'help' for a list of valid commands.\n");
        }
    }

    return 0;
}

