// Include

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <readline/readline.h>           //Biblioteca para suportar comandos de um terminal real, como seta pra cima ou pra baixo, historico de comandos, ctrl a, ctrl e, etc
#include <readline/history.h>

#include "../include/read_parse.h"
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define RESET   "\033[0m"

// Functions to read and parse

CommandLine* reading(){

    printf(BOLDGREEN);

    char *linha = readline(">>> ");            //Readline para ler o input do usuario

    printf(RESET);
    fflush(stdout);

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
    // 2. Definir se existe um pipe ou paralelo
    char *separator = strchr(line, '|');
    if (separator == NULL){
        separator = strchr(line, '&');
        if(separator != NULL){
            cmd_line->flag = 1;
        }
    }
    else{
        cmd_line->flag = 0;
    }
    // 3. Contar quantos comandos existem
    int num_separators = 0;
    int num_cmds = 0;

    if (separator != NULL){
        for (const char *p = line; (p = strchr(p, *separator)) != NULL; p++) {  // contagem de pipes, p recebe um ponteiro para o primeiro |, strchar retorna null quando nao existem mais |
            num_separators++;
        }
        num_cmds = num_separators + 1;
    }
    else if (num_cmds == 0 && strlen(line) > 0){
        num_cmds = 1;
    }
    else if (num_cmds == 0 && strlen(line) == 0) {           //comando vazio
        free(cmd_line);
        return NULL;
    }
    // 4. Alocar memoria para os comandos
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
    // 5. Se houver separadores, separa a string em comandos e os comandos em argumentos
    int i = 0;
    if (separator != NULL){
        char delim[2];
        delim[0] = *separator;  // delim agora é "|" ou "&"
        delim[1] = '\0';

        char *saveptr;

        char *segment = strtok_r(line_copy, delim, &saveptr); // Necessario strtok_r porque usaremos strtok em parsing_commands

        while (segment != NULL && i < cmd_line->num_commands) {

            char *current_segment = segment;
            while (isspace((unsigned char)*current_segment)) {
                current_segment++;
            }
            char *end = current_segment + strlen(current_segment) - 1;
            while (end > current_segment && isspace((unsigned char)*end)) {
                end--;
            }
            *(end + 1) = '\0'; // Termina a string após o último caractere não-espaço

            if (strlen(current_segment) == 0) {             // Se o usuario digitar cmd1 | | cmd 2
                segment = strtok_r(NULL, delim, &saveptr); // Pega o próximo
                continue;
            }

            cmd_line->commands[i].args = parsing_commands(current_segment);

            if (cmd_line->commands[i].args == NULL) {
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
            segment = strtok_r(NULL, delim, &saveptr);
        }
    }
    // 5. Se nao houver sepadores, so ha um comando, entao separa o comando em argumentos
    else{
        cmd_line->commands[i].args = parsing_commands(line_copy);
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
    }
    cmd_line->num_commands = i;

    free(line_copy);
    return cmd_line;
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


// DEBUG

void print_command_line_details(const CommandLine *cmd_line) {
    if (cmd_line == NULL) {
        printf("DEBUG: CommandLine é NULL.\n");
        fflush(stdout); // Garante que a saída seja impressa imediatamente
        return;
    }

    printf("DEBUG: --- Detalhes da CommandLine ---\n");
    printf("DEBUG: Número total de comandos no pipe: %d\n", cmd_line->num_commands);

    if (cmd_line->num_commands == 0) {
        printf("DEBUG: Nenhum comando para exibir (num_commands é 0).\n");
        printf("DEBUG: --- Fim dos Detalhes da CommandLine ---\n");
        fflush(stdout);
        return;
    }

    if (cmd_line->commands == NULL) {
        printf("DEBUG: Array 'commands' é NULL, embora num_commands seja %d (estado inconsistente!).\n", cmd_line->num_commands);
        printf("DEBUG: --- Fim dos Detalhes da CommandLine ---\n");
        fflush(stdout);
        return;
    }

    for (int i = 0; i < cmd_line->num_commands; i++) {
        printf("DEBUG:   Comando %d:\n", i); // Usando índice 0-based para consistência com arrays

        if (cmd_line->commands[i].args == NULL) {
            printf("DEBUG:     args para o Comando %d é NULL.\n", i);
            continue; // Passa para o próximo comando no pipe
        }

        char **current_args = cmd_line->commands[i].args;
        for (int j = 0; ; j++) { // Loop infinito, quebra quando encontrar o NULL
            if (current_args[j] != NULL) {
                printf("DEBUG:     args[%d]: \"%s\"\n", j, current_args[j]);
            } else {
                printf("DEBUG:     args[%d]: (NULL)\n", j); // Imprime a representação do NULL terminador
                break; // Sai do loop de argumentos para este comando
            }
        }
    }
    printf("DEBUG: --- Fim dos Detalhes da CommandLine ---\n");
    fflush(stdout); // Garante que a saída seja impressa imediatamente
}


