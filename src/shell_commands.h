#ifndef SHELL_COMMANDS_H        //BIBLIOTECA PARA IMPLEMENTACAO DOS COMANDOS DO SHELL
#define SHELL_COMMANDS_H

// Struct para o path

typedef struct {
    char **path_list;
    int path_count;
} ShellState;

// Prototype

void cd(char **args);
void update_path(ShellState *state, char **args);
void exec_command (ShellState *state, char **args);
void help();
void exiting();

// Fim

#endif //SHELL_COMMANDS_H INCLUDED