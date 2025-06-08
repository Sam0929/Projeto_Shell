#ifndef SHELL_COMMANDS_H        //BIBLIOTECA PARA IMPLEMENTACAO DOS COMANDOS DO SHELL
#define SHELL_COMMANDS_H

// Include

#include "read_parse.h"

// Struct para o path

typedef struct {
    char **path_list;
    int path_count;
} ShellState;

// Prototype

void cd(char **args);
void update_path(ShellState *state, char **args);
void exec_command (ShellState *state, char **args);
void execute_pipe(CommandLine *cmd_line, ShellState *state);
void execute_parallel (CommandLine *cmd_line, ShellState *state);
void help();
void exiting(CommandLine *cmd_line, ShellState *state);
void execute_shell_script(const char *filename);

// Fim

#endif //SHELL_COMMANDS_H INCLUDED