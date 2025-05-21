#ifndef SHELL_H_INCLUDED        //BIBLIOTECA PARA IMPLEMENTACAO DOS COMANDOS DO SHELL
#define SHELL_H_INCLUDED

// Struct para o path

typedef struct {
    char **path_list;
    int path_count;
} ShellState;

// Comandos a implementar

int cd(char **args) { //Muda o diretório de trabalho.

    int msg;

    msg = chdir(args [1]);

    if (msg == -1){
        // printf("Error Number: %d\n", errno);
        printf("Error Description: %s\n", strerror(errno));
        return -1;
    }

    return 0;
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

int exec_command (ShellState *state, char **args) {

    pid_t pid;

    pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0){

        char exec_path[1024];

        if (strchr(args[0], '/')) {

            execv(args[0], args);   // O comando tem uma barra -> é um caminho direto
            printf("%s: %s\n", args[0], strerror(errno));
        }

        else{

            for (int i = 0; i < state->path_count; i++) {
                snprintf(exec_path, sizeof(exec_path), "%s/%s", state->path_list[i], args[0]);
                execv(exec_path, args);                            // tenta executar
            }

            printf("%s: %s\n", args[0], strerror(errno));

            // fprintf(stderr, "Command not found in specified paths\n");

            // if (errno == ENOENT)
            //     _exit(127);  // comando não encontrado
            // else
            //     _exit(126);  // não executável ou outro erro
        }
    }
    else{
        // Pai: espera o filho terminar, tratando interrupções por sinais
        int status;
        pid_t w;

        do {
            w = waitpid(pid, &status, 0);
        } while (w == -1 && errno == EINTR);

        if (w == -1) {
            perror("waitpid falhou");
            return -1;
        }

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
                                                     //    printf("Comando terminou com código %d\n", exit_code);
            return exit_code;
        }
        else if (WIFSIGNALED(status)) {
            int sig = WTERMSIG(status);
            fprintf(stderr, "Processo filho terminou por sinal %d\n", sig);
            return 128 + sig;                           // convenção de shells para sinal
        }

        // Casos raros (ex.: parou por trap)
        return -1;

    }
}

//  Fim dos comandos a implementar

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

void exiting(char **args){

    printf("\nExiting...\n\n");

    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
    free(args);

    exit(0);

}

//  Fim dos Comandos testes

#endif //SHELL_H_INCLUDED