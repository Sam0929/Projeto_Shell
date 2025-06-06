
// Include
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "shell_commands.h"
#include "free_memory.h"

// Intern Prototype

static void redirect (char **args);

// Comandos Shell

void cd(char **args) {

    if (args[1] == NULL) {
        return;
    } else if (args[2] != NULL) { // "cd dir1 dir2" e um erro
        fprintf(stderr, "cd: Muitos argumentos\n");
    }
    else {
        if (chdir(args[1]) == -1) {
            perror("cd");
        }
    }
}

void update_path(ShellState *state, char **args) {
    // Caso 1: Nenhum argumento extra, apenas mostrar os caminhos atuais.
    if (args[1] == NULL) {
        printf("Caminhos atuais:\n");
        if (state->path_count == 0) {
            printf("(nenhum caminho definido)\n");
        } else {
            for (int i = 0; i < state->path_count; i++) {
                printf("%s\n", state->path_list[i]);
            }
        }
        return;
    }
    // Caso 2: Atualizar a lista de caminhos.

    // Contar os novos caminhos
    int new_count = 0;
    while (args[new_count + 1] != NULL) {
        new_count++;
    }

    // Alocar memória para a nova lista de caminhos
    char **new_path_list = malloc(new_count * sizeof(char *));
    if (new_path_list == NULL) {
        perror("Erro ao alocar o path! (1)");
        return;
    }

    // Copiar os novos caminhos para a nova lista
    int i;
    for (i = 0; i < new_count; i++) {

        new_path_list[i] = strdup(args[i + 1]);

        if (new_path_list[i] == NULL) {
            perror("Erro ao alocar o path! (2)");
            for (int j = 0; j < i; j++) {
                free(new_path_list[j]);
            }
            free(new_path_list);
            return; // Retorna sem alterar o estado antigo
        }
    }

    // Se tudo deu certo:
    // 1. Libera a lista de caminhos antiga
    free_path_list(state);

    // 2. Atribui a nova lista e o novo contador ao estado
    state->path_list = new_path_list;
    state->path_count = new_count;

}

static void child_exec_logic(char **args, ShellState *state) {
    if (args[0] == NULL) _exit(EXIT_FAILURE);


    if (strchr(args[0], '/')) {
        execv(args[0], args);
    }

    // Tentativa com PATH
    char exec_path[1024];
    for (int k = 0; k < state->path_count; k++) {
        snprintf(exec_path, sizeof(exec_path), "%s/%s", state->path_list[k], args[0]);
        execv(exec_path, args); // Se bem-sucedido, não retorna
    }

    // Se chegou aqui, nenhum execv teve sucesso
    fprintf(stderr, "%s: Comando não encontrado\n", args[0]);
    _exit(127);
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

        redirect(args);                     // Verifica se deve haver um redirecionamento do saida padrao do processo filho

        child_exec_logic(args, state);
    }
    else{

    // =========== Pai ===========

    int status;

    waitpid(pid, &status, WUNTRACED);

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        if (exit_code != 0 && exit_code != 127) { // 127 é "comando não encontrado"
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
    }

    }
}

void execute_pipe(CommandLine *cmd_line, ShellState *state) {
    int num_cmds = cmd_line->num_commands;
    int pipes[num_cmds - 1][2];
    pid_t pids[num_cmds];

    // Criar todos os pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_cmds; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) {
            // === FILHO ===
            char **args = cmd_line->commands[i].args;

            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i < num_cmds - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Fechar todos os pipes (não usados)
            for (int j = 0; j < num_cmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Verificar redirecionamento e path (como no exec_command)
            if (i == num_cmds - 1){                   // Somente se for o ultimo comando
                redirect(args);
            }
            child_exec_logic(args, state);
        }
    }

    // === PAI ===
    // Fechar todos os pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Aguardar todos os filhos
    for (int i = 0; i < num_cmds; i++) {
        int status;
        waitpid(pids[i], &status, WUNTRACED);

        char *cmd = cmd_line->commands[i].args[0];

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0 && exit_code != 127) {
                fprintf(stderr, "Comando '%s' falhou com código %d: %s\n", cmd, exit_code, strerror(exit_code));
            }
        } else if (WIFSIGNALED(status)) {
            int sig = WTERMSIG(status);
            fprintf(stderr, "Comando '%s' terminou com sinal %d (%s)\n", cmd, sig, strsignal(sig));
        } else if (WIFSTOPPED(status)) {
            int sig = WSTOPSIG(status);
            fprintf(stderr, "Comando '%s' foi parado com sinal %d (%s)\n", cmd, sig, strsignal(sig));
        }
    }
}

void execute_parallel (CommandLine *cmd_line, ShellState *state){

    int num_cmds = cmd_line->num_commands;
    pid_t pids[num_cmds];

    for (int i = 0; i < num_cmds; i++) {

        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) {
            // === FILHO ===
            char **args = cmd_line->commands[i].args;

            redirect(args); // Aqui o redirecionamento nao precisa de restricoes

            child_exec_logic(args, state);
        }
    }
    // === PAI ===
    // Aguardar todos os filhos
    for (int i = 0; i < num_cmds; i++) {
        int status;
        waitpid(pids[i], &status, WUNTRACED);

        char *cmd = cmd_line->commands[i].args[0];

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0 && exit_code != 127) {
                fprintf(stderr, "Comando '%s' falhou com código %d: %s\n", cmd, exit_code, strerror(exit_code));
            }
        } else if (WIFSIGNALED(status)) {
            int sig = WTERMSIG(status);
            fprintf(stderr, "Comando '%s' terminou com sinal %d (%s)\n", cmd, sig, strsignal(sig));
        } else if (WIFSTOPPED(status)) {
            int sig = WSTOPSIG(status);
            fprintf(stderr, "Comando '%s' foi parado com sinal %d (%s)\n", cmd, sig, strsignal(sig));
        }
    }
}

static void redirect (char **args){

    int redirect = -1;

    for (int i = 0; args[i]; i++) {
        if (strcmp(args[i], ">") == 0) {
            redirect = i;
            break;
        }
    }
    if (redirect != -1) {

        fflush(stdout);
        args[redirect] = NULL; // Remove '>' dos argumentos para o execv

        if (args[redirect + 1] == NULL) {
            fprintf(stderr, "Erro: Nome de arquivo ausente para redirecionamento '>'\n");
            _exit(EXIT_FAILURE);
        }

        if (freopen(args[redirect + 1], "w", stdout) == NULL) {
            perror("Erro ao redirecionar saída");
            _exit(errno);
        }
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
    printf("    → Exemplo: pwd\n\n");


    printf("Para comandos com pipe:\n");
    printf("    → Digite cmd1 | cmd2 | cmd3\n");
    printf("    → E possivel redirecionar a saida do ultimo comando.\n");
    printf("    → Exemplo: cmd1 | cmd 2 | cmd 3 > arquivo.txt\n\n");

    printf("Para comandos em paralelo:\n");
    printf("    → Digite cmd1 & cmd2 & cmd3\n");
    printf("    → E possivel redirecionar a saida de qualquer comando.\n");
    printf("    → Exemplo: cmd1 > arquivo1.txt & cmd 2 & cmd 3 > arquivo2.txt\n\n");

    printf("Observações:\n");
    printf("--------------------------------------------------------\n");
    printf("  - Os comandos são separados por espaços (como em qualquer terminal Unix).\n");
    printf("  - Pressione Ctrl+D para encerrar comandos em execução (se necessário).\n\n");


    printf("════════════════════════════════════════════════════════\n");
    printf("Digite 'help' a qualquer momento para rever esta ajuda.\n");
    printf("════════════════════════════════════════════════════════\n\n");

}

void exiting(CommandLine *cmd_line, ShellState *state){

    printf("\nExiting...\n\n");
    free_command_line(cmd_line);
    free_path_list(state);
    free(state);

    exit(0);

}

//  Fim dos Comandos
