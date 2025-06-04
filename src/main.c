#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell_commands.h"
#include "read_parse.h"
#include "free_memory.h"

//Main

int main(int argc, char *argv[]) {

    ShellState state = {NULL, 0};  // inicializando a struct para o path

    if (argc > 1) {
        update_path(&state, argv);
    }

    printf("SamTum Terminal\n\n");
    printf("Welcome To SamTum Terminal\n");
    printf("Type \"help\" for more things!\n\n");

    while(1) {

        CommandLine* cmd_line = reading();

        // for (int i = 0; args[i] != NULL; i++){          // para testes
        //     printf("%s", args[i]);
        // }
        // print_command_line_details(cmd_line);  // DEBUG

        if (cmd_line == NULL || cmd_line->num_commands == 0) {
            if (cmd_line) {
                free_command_line(cmd_line);
            }
            continue;
         }

        int is_built_in = 0;

        if(cmd_line->num_commands == 1){

            char **args = cmd_line->commands[0].args;

            if (strcmp((args[0]), "exit") == 0){
                free_command_line(cmd_line);
                free_state(&state);
                exiting();
                is_built_in = 1;
            }
            else if (strcmp((args[0]), "cd") == 0){
                cd((args));
                is_built_in = 1;
            }
            else if (strcmp((args[0]), "help") == 0){
                help();
                is_built_in = 1;
            }
            else if (strcmp((args[0]), "path") == 0) {
                update_path(&state, (args));
                is_built_in = 1;
            }
        }

        if(!is_built_in && cmd_line->num_commands == 1){

            exec_command(&state, cmd_line->commands[0].args);    //DESENVOLVER: tratamento de erros reportados pelo processo pai
            // Implementar funcao para executar comandos com pipe e paralelo
        }
        else if (!is_built_in && cmd_line->num_commands > 1 && cmd_line->flag == 0){
            execute_pipe(cmd_line, &state);
        }
        else if (!is_built_in && cmd_line->num_commands > 1 && cmd_line->flag == 1){
            execute_parallel(cmd_line, &state);
        }

        free_command_line(cmd_line);  // Necessario para garantir que nenhum vazamento de memória ocorra.

    }

}

