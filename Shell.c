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

//Main

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

        CommandLine* List_of_Cmds = reading();

        // for (int i = 0; args[i] != NULL; i++){          // para testes
        //     printf("%s", args[i]);
        // }

        if (List_of_Cmds == NULL){

            continue;
        }
        if (strcmp((List_of_Cmds->commands[0].args[0]), "exit") == 0){
            exiting();
            found = 1;
        }else if (strcmp((List_of_Cmds->commands[0].args[0]), "cd") == 0){
            cd((List_of_Cmds->commands[0].args));
            found = 1;
        }
        else if (strcmp((List_of_Cmds->commands[0].args[0]), "clear") == 0){
            system("clear");
            found = 1;
        }
        else if (strcmp((List_of_Cmds->commands[0].args[0]), "help") == 0){
            help();
            found = 1;
        }
        else if (strcmp((List_of_Cmds->commands[0].args[0]), "path") == 0) {
            update_path(&state, (List_of_Cmds->commands[0].args));
            found = 1;
        }
        else {
            exec_command(&state, (List_of_Cmds->commands[0].args));    //DESENVOLVER: tratamento de erros reportados pelo processo pai
            found = 1;
        }

        free_command_line(List_of_Cmds);  // Necessario para garantir que nenhum vazamento de memória ocorra.

        if (!found) {
            printf("Invalid command. Type 'help' for a list of valid commands.\n");
        }
    }

}

