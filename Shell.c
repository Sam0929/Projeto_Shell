#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>           //Biblioteca para suportar comandos de um terminal real, como seta pra cima ou pra baixo, historico de comandos, ctrl a, ctrl e, etc
#include <readline/history.h>


#include "shell.h"
#include "read_parse.h"

#define COMMAND_LENGTH 100

int main(int argc, char *argv[]) {

    ShellState state = {NULL, 0};  // inicializando a struct para o path

    int found = 0;  // flag comando encontrado

    if (argc > 1) {
        update_path(&state, argv);
    }

    printf("SamTum Terminal\n\n");
    printf("Welcome To SamTum Terminal\n");
    printf("Type \"help\" for more things!\n\n");

    while(1) {

        found = 0;

        char **args = reading();

        // for (int i = 0; args[i] != NULL; i++){          // para testes
        //     printf("%s", args[i]);
        // }

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
        else if (strcmp(args[0], "clear") == 0){
            system("clear");
            found = 1;
        }
        else if (strcmp(args[0], "help") == 0){
            help();
            found = 1;
        }
        else if (strcmp(args[0], "path") == 0) {
            update_path(&state, args);
            found = 1;
        }
        else {

            exec_command(&state, args);
            found = 1;
        }


        if (!found) {
            printf("Invalid command. Type 'help' for a list of valid commands.\n");
        }
    }

}

