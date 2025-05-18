#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#include "shell.h"

#define COMMAND_LENGTH 100

char **parsing (char input[100]);
char **reading ();

char **reading(){

    char input[1024];

    fgets(input, sizeof(input), stdin);

    return parsing (input);

}

char **parsing (char input[100]){


    char **args = malloc(50 * sizeof(char *));  // Lista de argumentos
    char *token = strtok(input, " \t\n");

    int i = 0;

    while (token != NULL) {

        args[i++] = strdup(token);
        token = strtok(NULL, " \t\n");
    }
    if (i != 0){
        for (int i = 0; args[0][i]; i++){
            args [0][i] = tolower(args[0][i]);  // Padronizando o comando a ser reconhecido
        }
    }

    args[i] = NULL;

    return args;

}

int main(int argc, char *argv[]) {

    int found = 0;  // flag comando encontrado

    printf("SamTum Terminal\n\n");
    printf("Welcome To SamTum Terminal\n");
    printf("Type \"help\" for more things!\n\n");

    while(1) {

        found = 0;

        printf(">>> ");

        char **args = reading();

        if (args[0] == NULL){
            free(args);
            continue;
        }

        if (strcmp(args[0], "exit") == 0){
            exiting(args);
            found = 1;
        }else if (strcmp(args[0], "cd") == 0){
            cd(args);
            found = 1;
        }
        else if (strcmp(args[0], "help") == 0){
            help();
            found = 1;
        }
        else if (strcmp(args[0], "ls") == 0  ||
                 strcmp(args[0], "pwd") == 0 ||
                 strcmp(args[0], "cat") == 0 ){

            exec_command(args);
            found = 1;
        }


        if (!found) {
            printf("Invalid command. Type 'help' for a list of valid commands.\n");
        }
    }

}

