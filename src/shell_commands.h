#ifndef SHELL_COMMANDS_H        //BIBLIOTECA PARA IMPLEMENTACAO DOS COMANDOS DO SHELL
#define SHELL_COMMANDS_H

// Include

#include <stdbool.h>
#include "read_parse.h"


// Struct para o path

typedef struct {
    char **path_list;
    int path_count;
} ShellState;

// Prototype

void cd(char **args);
void update_path(ShellState *state, char **args);
void launch_job (CommandLine *cmd_line, ShellState *state, bool is_background);
void help();
void exiting();

// Fim

#endif //SHELL_COMMANDS_H INCLUDED