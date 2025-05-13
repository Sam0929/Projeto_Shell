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
    void (*action)(char **args);  
} Command;

char **parsing (char input[100]);
char **read ();

char **read(){

    char input[1024];

    fgets(input, sizeof(input), stdin);

    for (int i = 0; input[i]; i++) {    //Padronizando a entrada de dados
        input[i] = tolower(input[i]);
    }

    return parsing (input);
    
}

char **parsing (char input[100]){
    

    char **args = (char**) malloc(50 * sizeof(char));  // Lista de argumentos
    char *token = strtok(input, " \t\n");
    int i = 0;

    while (token != NULL) {

        args[i++] = strdup(token);
        token = strtok(NULL, " \t\n");
    }

    args[i] = NULL;
    
    return args;

}


// Main function
int main(int argc, char *argv[]) {

    Command commands[] = {

        {"cd", cd},
        {"path", path},
        {"pwd", pwd},
        {"cat", cat},
        {"ls", ls},
        {"teste", teste},
        {"help", help},

    };

    int num_commands = sizeof(commands) / sizeof(commands[0]);  // Number of commands

    int found = 0;  // flag comando encontrado

    printf("SamTum Terminal\n\n");
    printf("Welcome To SamTum Terminal\n");
    printf("Type \"help\" for more things!\n\n");

    while(1) {

        printf(">>> ");

        char **args = read();

        if (args[0] == NULL) {
            // Comando vazio (ex: enter sozinho)
            free(args);
            continue;
        }

        // Comando para sair

        if (strcmp(args[0], "exit") == 0) {

            printf("\nExiting...\n\n");

            for (int i = 0; args[i]; i++) free(args[i]);

            free(args);

            return 0;
        }

        

        for (int i = 0; i < num_commands; i++) {

            if (strcmp(args[0], commands[i].command) == 0) {
                commands[i].action(args);  
                found = 1;
                break;
            }
        }

        if (!found) {
            printf("Invalid command. Type 'help' for a list of valid commands.\n");
        }
    }

}

