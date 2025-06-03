#ifndef READ_PARSE_H           // BIBLIOTECA PARA I/O
#define READ_PARSE_H

// Struct command

typedef struct {
    char **args; // Argumentos para execvp (ex: {"ls", "-l", NULL})
} ParsedCommand;

typedef struct {
    ParsedCommand *commands; // Array de comandos
    int num_commands;        // Número de comandos no pipeline (1 se não for pipeline)
    int flag;
} CommandLine;


// Prototype

CommandLine* reading ();
CommandLine* parse_line(const char *line);
char **parsing_commands (char *input);
void print_command_line_details(const CommandLine *cmd_line);

#endif