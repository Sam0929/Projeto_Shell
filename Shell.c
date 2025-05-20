#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#include "shell.h"

#define COMMAND_LENGTH 100

char **parsing (char input[100]);

void execute_parallel_commands(char *input); // Declaração da função para executar comandos em paralelo
int execute_pipeline(char *input); // Declaração da função para executar comandos em pipeline

char **parsing (char input[100]){


    char **args = malloc(50 * sizeof(char *));  // Lista de argumentos
    char *token = strtok(input, " \t\n");

    int i = 0;

    while (token != NULL) {

        args[i++] = strdup(token);
        token = strtok(NULL, " \t\n");
    }
    if (i != 0){
        for (int i = 0; args[0][i]; i++){
            args [0][i] = tolower(args[0][i]);  // Padronizando o comando a ser reconhecido
        }
    }

    args[i] = NULL;

    return args;

}



int main(int argc, char *argv[]) {

    int found = 0;  // flag comando encontrado

    printf("SamTum Terminal\n\n");
    printf("Welcome To SamTum Terminal\n");
    printf("Type \"help\" for more things!\n\n");

    while(1) {

        found = 0;

        printf(">>> ");

        //char **args = reading(); subistituido pelo codigo abaixo para suportar comandos em paralelo
        char input[1024];
        fgets(input, sizeof(input), stdin);

        if (strchr(input, '&') != NULL) {
            execute_parallel_commands(input);
        }
        if (strchr(input, '|') != NULL) {
            execute_pipeline(input);
        }

        char **args = parsing(input);
        // fim da substituição

        if (args[0] == NULL){
            free(args);
            continue;
        }

        if (strcmp(args[0], "exit") == 0){
            exiting(args);
            found = 1;
        }else if (strcmp(args[0], "cd") == 0){
            cd(args);
            found = 1;
        }
        else if (strcmp(args[0], "help") == 0){
            help();
            found = 1;
        }
        else if (strcmp(args[0], "ls") == 0  ||
                 strcmp(args[0], "pwd") == 0 ||
                 strcmp(args[0], "cat") == 0 ){

            exec_command(args);
            found = 1;
        }


        if (!found) {
            printf("Invalid command. Type 'help' for a list of valid commands.\n");
        }
    }

}

void execute_parallel_commands(char *input) {
    char *commands[50];
    int num_cmds = 0;

    // Separar comandos pelo &
    char *ptr = strtok(input, "&");
    while (ptr != NULL && num_cmds < 50) {
        // Remover espaços extras
        while (*ptr == ' ') ptr++;

        size_t len = strlen(ptr);
        while (len > 0 && (ptr[len - 1] == ' ' || ptr[len - 1] == '\n')) {
            ptr[len - 1] = '\0';
            len--;
        }

        if (strlen(ptr) > 0) {
            commands[num_cmds] = strdup(ptr);
            num_cmds++;
        }

        ptr = strtok(NULL, "&");
    }

    // Criar filhos
    for (int i = 0; i < num_cmds; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            char **args = parsing(commands[i]);

            if (args[0] != NULL) {
                if (strcmp(args[0], "exit") == 0) exiting(args);
                else if (strcmp(args[0], "cd") == 0) cd(args);
                else if (strcmp(args[0], "help") == 0) help();
                else exec_command(args);
            }

            for (int j = 0; args[j] != NULL; j++) free(args[j]);
            free(args);

            exit(0);
        }
    }

    for (int i = 0; i < num_cmds; i++) {
        wait(NULL);
    }

    for (int i = 0; i < num_cmds; i++) {
        free(commands[i]);
    }
}

int execute_pipeline(char *input) {
    char *commands[10];  // Máximo de 10 comandos encadeados
    int num_cmds = 0;

    // Separar por '|'
    char *token = strtok(input, "|");
    while (token != NULL && num_cmds < 10) {
        while (*token == ' ') token++;  // Remover espaços à esquerda
        commands[num_cmds++] = token;
        token = strtok(NULL, "|");
    }

    int pipefd[2], prev_fd = -1;

    for (int i = 0; i < num_cmds; i++) {
        pipe(pipefd);

        pid_t pid = fork();

        if (pid == 0) {
            // Filho
            if (i < num_cmds - 1) {
                // Se não for o último, redireciona stdout para pipe de saída
                dup2(pipefd[1], STDOUT_FILENO);
            }

            if (i > 0) {
                // Se não for o primeiro, redireciona stdin para a entrada anterior
                dup2(prev_fd, STDIN_FILENO);
            }

            // Fecha descritores de pipe no processo filho
            close(pipefd[0]);
            close(pipefd[1]);
            if (prev_fd != -1) close(prev_fd);

            char **args = parsing(commands[i]);

            execvp(args[0], args);
            perror("Erro no execvp");
            exit(1);
        }

        // Pai: fecha descritores que não vai usar
        if (prev_fd != -1) close(prev_fd);
        close(pipefd[1]);

        prev_fd = pipefd[0];  // O próximo comando vai ler deste pipe
    }

    // Espera todos os filhos
    for (int i = 0; i < num_cmds; i++) {
        wait(NULL);
    }

    return 0;
}

