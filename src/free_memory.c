// Includes

#include <stdlib.h>

#include "free_memory.h" //

//

void free_command_line(CommandLine *cmd_line) {

    if (!cmd_line) {
        return;
    }
    for (int i = 0; i < cmd_line->num_commands; i++) {
        if (cmd_line->commands[i].args) {
            char **arg_ptr = cmd_line->commands[i].args;
            for (int j = 0; arg_ptr[j] != NULL; j++) {
                free(arg_ptr[j]);
            }
            free(cmd_line->commands[i].args);
        }
    }
    free(cmd_line->commands);
    free(cmd_line);
}

void free_state (ShellState *state){
    if (!state) {
        return;
    }
    if (state->path_count != 0){

        for (int i = 0; i < state->path_count; i++) {
            free(state->path_list[i]);
        }

        free(state->path_list);
    }
}