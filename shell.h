#ifndef SHELL_H_INCLUDED        //BIBLIOTECA PARA IMPLEMENTACAO DOS COMANDOS DO SHELL
#define SHELL_H_INCLUDED

// Prototype

void replace_ls_exa(char **args);
void redirect(char **args);
void absolute_path(char **args);

// Struct para o path

typedef struct {
    char **path_list;
    int path_count;
} ShellState;

// Comandos a implementar

void cd(char **args) { //Muda o diretório de trabalho.

    int msg;

    msg = chdir(args [1]);

    if (msg == -1){
        // printf("Error Number: %d\n", errno);
        printf("Error Description: %s\n", strerror(errno));
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

        _exit(errno); // Se exec chegou aqui, um erro aconteceu, retorna para o pai
    }
    else{

    // =========== Pai ===========

        int status;

        while (waitpid(pid, &status, 0) == -1 && errno == EINTR); // Aguarda filho, reiniciando se for interrompido por sinal

        if (WIFEXITED(status)) {  // Verifica se o processo filho saiu por exit

            int code = WEXITSTATUS(status);  //Extrai o codigo de saida  de exit
            if(code != 0 ){
                fprintf(stderr, "Erro ao executar '%s': %s\n", args[0], strerror(code));  //Exibe o erro do processo filho
            }

         }
        
        return;

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

void redirect (char **args){

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
void absolute_path(char **args){

    if (strchr(args[0], '/')) {

        execv(args[0], args);   // O comando tem uma barra -> é um caminho direto

        _exit(errno);
    }

    return;
}

void free_memory(char **args){

    for (int i = 0; args[i] != NULL; i++) {

        free(args[i]);

    }

    free(args);

}
void exiting(char **args){

    printf("\nExiting...\n\n");

    free_memory(args);

    exit(0);

}

//  Fim dos Comandos testes

#endif //SHELL_H_INCLUDED