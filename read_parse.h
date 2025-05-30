#ifndef READ_PARSE_H_INCLUDED           // BIBLIOTECA PARA I/O
#define READ_PARSE_H_INCLUDED

// Struct

typedef struct {
    char **args; // Argumentos para execvp (ex: {"ls", "-l", NULL})
} ParsedCommand;

typedef struct {
    ParsedCommand *commands; // Array de comandos
    int num_commands;        // Número de comandos no pipeline (1 se não for pipeline)
} CommandLine;

// Prototype

CommandLine* reading ();
CommandLine* parse_line(const char *line);
void free_command_line(CommandLine *cmd_line);
char **parsing_commands (char *input);


// Functions to read and parse

CommandLine* reading(){

    char *linha = readline(">>> ");

    if (linha == NULL) {
        // Ctrl+D
        printf("\n");
        exit(0);
    }
    if (strlen(linha) > 0) {
        add_history(linha);  // adiciona ao historico se nao for vazia
    }

    CommandLine *cmd_data = parse_line(linha);

    free (linha);

    return cmd_data;
}

char **parsing_commands (char *input){

    char **args = malloc(50 * sizeof(char *));       // Lista de argumentos

    if (!args) {
        perror("Falha ao alocar argumentos!");
        return NULL;
    }
    const char *delimiters = " \t\n";
    char *token = strtok(input, delimiters);        // " \t\n" // strtok encontra o delimitador e substitui ele por \0

    int i = 0;

    while (token != NULL && i < 49) {

        args[i] = strdup(token);                  //Copia a string até o primeiro \0 para args

        if (!args[i]) {
            perror("Falha ao duplicar token!");
            for (int k = 0; k < i; k++) {
                free(args[k]);
            }
            free(args);
            return NULL;
        }

        token = strtok(NULL, delimiters);
        i++;
    }

    args[i] = NULL;

    //  for (int i = 0; args[i] != NULL; i++){          // para testes
    //     printf("string [%d] = %s",i, args[i]);
    //  }
    //  fflush(stdout);

    return args;

}

CommandLine* parse_line(const char *line) {

    if (line == NULL || strlen(line) == 0) {
        return NULL;
    }
    // 1. Alocar a estrutura CommandLine
    CommandLine *cmd_line = malloc(sizeof(CommandLine));
    if (!cmd_line) {
        perror("Falha ao alocar CommandLine");
        return NULL;
    }
    cmd_line->commands = NULL;
    cmd_line->num_commands = 0;

    int num_pipes = 0;
    for (const char *p = line; (p = strchr(p, '|')) != NULL; p++) {  // contagem de pipes, p recebe um ponteiro para o primeiro |, strchar retorna null quando nao existem mais |
        num_pipes++;
    }
    int num_cmds = num_pipes + 1;

    if (num_cmds == 0 && strlen(line) > 0) {                   // sem pipe mas ainda existe um comando
        num_cmds = 1;
    } else if (num_cmds == 0 && strlen(line) == 0) {           //comando vazio
        free(cmd_line);

        return NULL;
    }

    cmd_line->num_commands = num_cmds;
    cmd_line->commands = malloc(num_cmds * sizeof(ParsedCommand));

    if (!cmd_line->commands) {
        perror("Falha ao alocar array de ParsedCommand");
        free(cmd_line);
        return NULL;
    }

    char *line_copy = strdup(line); // Trabalhar em uma cópia para strtok_r ou strtok

    if (!line_copy) {
        perror("strdup_line_copy");
        free(cmd_line->commands);
        free(cmd_line);
        return NULL;
    }

    char *saveptr; // Para strtok_r, se preferir (mais seguro)
    int i = 0;

    char *segment = strtok(line_copy, "|");

    while (segment != NULL && i < cmd_line->num_commands) {
        cmd_line->commands[i].args = parsing_commands(segment);
        if (cmd_line->commands[i].args == NULL) {
            // Erro na tokenização do segmento, liberar tudo
            for (int k = 0; k < i; k++) {
                for(int j=0; cmd_line->commands[k].args[j] != NULL; j++) free(cmd_line->commands[k].args[j]);
                free(cmd_line->commands[k].args);
            }
            free(cmd_line->commands);
            free(cmd_line);
            free(line_copy);
            return NULL;
        }
        i++;
        segment = strtok(NULL, "|");
    }
    // Se strtok retornou menos segmentos que o esperado (ex: "cmd | | cmd2"), precisa tratar
    cmd_line->num_commands = i; // Atualiza o número real de comandos parseados

    free(line_copy);
    return cmd_line;
}

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


#endif   //READ_PARSE_H_INCLUDED