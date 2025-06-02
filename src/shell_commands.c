
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


static void child_exec_logic(char **args, ShellState *state) {

    if (args[0] == NULL) _exit(EXIT_FAILURE); // Comando vazio

    if (strchr(args[0], '/')) {
        execv(args[0], args);
        perror(args[0]);
        _exit(126);
    } else {
        char exec_path[1024];
        for (int k = 0; k < state->path_count; k++) {
            snprintf(exec_path, sizeof(exec_path), "%s/%s", state->path_list[k], args[0]);
            execv(exec_path, args);
        }
        // Se todos os execv falharam
        fprintf(stderr, "%s: comando não encontrado\n", args[0]);
        _exit(127); // Código padrão para "comando não encontrado"
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

static void absolute_path(char **args){

    if (strchr(args[0], '/')) {

        execv(args[0], args);   // O comando tem uma barra -> é um caminho direto

        _exit(errno);
    }

    return;
}

void launch_job(CommandLine *cmd_line, ShellState *state, bool is_background) {
    int num_commands = cmd_line->num_commands;
    if (num_commands == 0) return;

    // --- INICIALIZAÇÃO DO JOB ---
    pid_t pids[num_commands];
    pid_t job_pgid = 0;
    int i;


    int infile_fd = STDIN_FILENO;
    int outfile_fd = STDOUT_FILENO;
    int prev_pipe_read_end = STDIN_FILENO;


    for (i = 0; i < num_commands; i++) {
        int current_pipe_fds[2] = {-1, -1};

        if (i < num_commands - 1) {
            if (pipe(current_pipe_fds) < 0) {
                perror("pipe");
                return;
            }
            outfile_fd = current_pipe_fds[1];
        } else {
            outfile_fd = STDOUT_FILENO;
        }

        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            if (current_pipe_fds[0] != -1) { close(current_pipe_fds[0]); close(current_pipe_fds[1]); }
            if (prev_pipe_read_end != STDIN_FILENO) close(prev_pipe_read_end);
            return;
        }

        if (pids[i] == 0) {
            pid_t current_pid = getpid();
            if (job_pgid == 0) {
                job_pgid = current_pid;
            }
            setpgid(current_pid, job_pgid);

            // Configurar STDIN
            if (prev_pipe_read_end != STDIN_FILENO) {
                if (dup2(prev_pipe_read_end, STDIN_FILENO) < 0) {
                    perror("dup2 stdin"); _exit(EXIT_FAILURE);
                }
                close(prev_pipe_read_end);
            }


            if (outfile_fd != STDOUT_FILENO) {
                if (dup2(outfile_fd, STDOUT_FILENO) < 0) {
                    perror("dup2 stdout"); _exit(EXIT_FAILURE);
                }
                close(outfile_fd);
            }

            if (current_pipe_fds[0] != -1) close(current_pipe_fds[0]);

            if (i == num_commands - 1) {
                redirect(cmd_line->commands[i].args);
            }

            child_exec_logic(cmd_line->commands[i].args, state);

        }

        if (job_pgid == 0) {
            job_pgid = pids[i];
        }
        setpgid(pids[i], job_pgid);


        if (prev_pipe_read_end != STDIN_FILENO) {
            close(prev_pipe_read_end);
        }
        if (outfile_fd != STDOUT_FILENO) {
            close(outfile_fd);
        }

        prev_pipe_read_end = current_pipe_fds[0];

        if (i < num_commands - 1) {
            prev_pipe_read_end = current_pipe_fds[0];
        } else {
            if(current_pipe_fds[0] != -1) close(current_pipe_fds[0]);
            prev_pipe_read_end = -1;
        }
    } // Fim do loop de fork

    // --- PAI ESPERA (se for foreground) ---
    if (!is_background) {

        for (int j = 0; j < num_commands; j++) {
            int status;

            if (waitpid(pids[j], &status, WUNTRACED) > 0) {
                if (WIFEXITED(status)) {
                    int exit_code = WEXITSTATUS(status);
                    if (j == num_commands - 1 && exit_code != 0 && exit_code != 127) {
                        fprintf(stderr, "Pipeline terminou com erro (código %d)\n", exit_code);
                    }
                } else if (WIFSIGNALED(status)) {

                } else if (WIFSTOPPED(status)) {

                }
            } else if (errno != ECHILD) {
                perror("waitpid no pipeline");
            }
        }

    } else {

        printf("Job em background iniciado com PGID: %d\n", job_pgid);

    }
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
