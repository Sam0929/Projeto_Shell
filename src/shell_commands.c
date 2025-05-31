
// Include
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#include "shell_commands.h"

// Intern Prototype

static void redirect (char **args);
static void absolute_path (char **args);


// Comandos Shell

void cd(char **args) { //Muda o diretório de trabalho.

    if (args[1] == NULL) {

        const char *home_dir = getenv("HOME");
        if (home_dir == NULL) {
            fprintf(stderr, "cd: variável HOME não definida\n");
            return;
        }
        if (chdir(home_dir) == -1) {
            perror("cd");
        }

    } else if (args[2] != NULL) { // "cd dir1 dir2" e um erro
        fprintf(stderr, "cd: muitos argumentos\n");
    }
    else {
        if (chdir(args[1]) == -1) {
            perror("cd");
        }
    }
}

void update_path(ShellState *state, char **args) {

    if (args[1] == NULL) {
                                                                // Nenhum argumento extra: mostrar os caminhos atuais
        printf("Caminhos atuais:\n");

        for (int i = 0; i < state->path_count; i++) {
            printf("%s\n", state->path_list[i]);
        }

        return;
    }

    for (int i = 0; i < state->path_count; i++) {                     // Libera os caminhos antigos
        free(state->path_list[i]);
    }

    free(state->path_list);

    int count = 0;                                                   // Conta os novos caminhos
    while (args[count + 1] != NULL) count++;


    state->path_list = malloc(count * sizeof(char *));              // Aloca e copia os novos caminhos
    state->path_count = count;

    for (int i = 0; i < count; i++) {
        state->path_list[i] = strdup(args[i + 1]);
    }
}

void exec_command (ShellState *state, char **args) {

    pid_t pid;

    pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0){

        // =========== FILHO ===========

        char exec_path[1024];

        redirect(args);                     // Verifica se deve haver um redirecionamento do saida padrao do processo filho

        absolute_path(args);                // Verifica se um caminho absoluto foi passado

        for (int i = 0; i < state->path_count; i++) {

            snprintf(exec_path, sizeof(exec_path), "%s/%s", state->path_list[i], args[0]);
            execv(exec_path, args);                            // tenta executar
        }

        fprintf(stderr, "%s: comando não encontrado\n", args[0]);
        _exit(127); // Código de saída padrão para "comando não encontrado"
    }
    else{

    // =========== Pai ===========

    int status;

    waitpid(pid, &status, WUNTRACED); // WUNTRACED reporta também se o filho foi parado

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        if (exit_code != 0 && exit_code != 127) { // 127 é nosso "comando não encontrado"
            fprintf(stderr, "Comando '%s' terminou com erro (código %d): %s\n",
                    args[0], exit_code, strerror(exit_code));
        }
    } else if (WIFSIGNALED(status)) {
        int signal_num = WTERMSIG(status);
        fprintf(stderr, "Comando '%s' terminou devido ao sinal %d (%s)\n",
                args[0], signal_num, strsignal(signal_num));
    } else if (WIFSTOPPED(status)) {
        int signal_num = WSTOPSIG(status);
        fprintf(stderr, "Comando '%s' foi parado pelo sinal %d (%s)\n",
                args[0], signal_num, strsignal(signal_num));
        // AQUI é onde você veria SIGTTIN (sinal 21 geralmente)
        // Para um shell simples, você pode apenas reportar ou tentar continuar/terminar o filho.
        // Para job control completo, você adicionaria à lista de jobs parados.
    }

    }
}

static void redirect (char **args){

    int redirect = -1;

    for (int i = 0; args[i]; i++) {
        if (strcmp(args[i], ">") == 0) {
            redirect = i;
        }
    }

    if (redirect != -1){
        fflush(stdout);
        args[redirect] = NULL;
        if (freopen(args[redirect + 1], "w", stdout) == NULL){
              perror("Erro ao redirecionar saída");
              _exit(errno);
        }
    }

    return;
}

// void exec_pipe (char **args){

//     int pipe = -1;

//     return;

// }

static void absolute_path(char **args){

    if (strchr(args[0], '/')) {

        execv(args[0], args);   // O comando tem uma barra -> é um caminho direto

        _exit(errno);
    }

    return;
}

void help() {

    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║                    PUCC SHELL - AJUDA                ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("Comandos internos:\n");
    printf("--------------------------------------------------------\n");
    printf("  help\n");
    printf("    → Mostra esta tela de ajuda.\n\n");

    printf("  exit\n");
    printf("    → Encerra o shell.\n\n");

    printf("  cd <diretório>\n");
    printf("    → Altera o diretório atual para o especificado.\n");
    printf("    → Exemplo: cd /home/usuario\n\n");

    printf("  path [<caminho1> <caminho2> ...]\n");
    printf("    → Define os diretórios de busca para comandos externos.\n");
    printf("    → Se nenhum caminho for informado, mostra os caminhos atuais.\n");
    printf("    → Exemplo: path /bin /usr/bin\n");
    printf("    → Exemplo: path\n\n");

    printf("Comandos externos:\n");
    printf("--------------------------------------------------------\n");
    printf("  <comando> [argumentos]\n");
    printf("    → Executa um programa localizado em um dos caminhos definidos com 'path'.\n");
    printf("    → Exemplo: ls -l\n");
    printf("    → Exemplo: cat arquivo.txt\n\n");

    printf("Observações:\n");
    printf("--------------------------------------------------------\n");
    printf("  - Os comandos são separados por espaços (como em qualquer terminal Unix).\n");
    printf("  - Ainda não há suporte para redirecionamentos (>, >>) ou pipes (|).\n");
    printf("  - Pressione Ctrl+C para encerrar comandos em execução (se necessário).\n");
    printf("  - Este shell é um projeto educacional com funcionalidades básicas.\n\n");

    printf("════════════════════════════════════════════════════════\n");
    printf("Digite 'help' a qualquer momento para rever esta ajuda.\n");
    printf("════════════════════════════════════════════════════════\n\n");

}

void exiting(){

    printf("\nExiting...\n\n");

    exit(0);

}

//  Fim dos Comandos
