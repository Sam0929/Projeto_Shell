#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/shell_commands.h"
#include "../include/read_parse.h"
#include "../include/free_memory.h"
#include "../include/intro.h"


//Main

int main(int argc, char *argv[]) {

    ShellState *state = malloc(sizeof(ShellState));  // inicializando a struct para o path
    state->path_list = NULL;
    state->path_count = 0;

    if (argc > 1) {
        update_path(state, argv);
    }

    exibir_intro_terminal();

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
                exiting(cmd_line, state);
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
                update_path(state, (args));
                is_built_in = 1;
            }
        }

        if(!is_built_in && cmd_line->num_commands == 1){
            exec_command(state, cmd_line->commands[0].args);
        }
        else if (!is_built_in && cmd_line->num_commands > 1 && cmd_line->flag == 0){
            execute_pipe(cmd_line, state);
        }
        else if (!is_built_in && cmd_line->num_commands > 1 && cmd_line->flag == 1){
            execute_parallel(cmd_line, state);
        }

        free_command_line(cmd_line);  // Necessario para garantir que nenhum vazamento de memória ocorra.

    }

}

