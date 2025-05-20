#ifndef SHELL_H_INCLUDED
#define SHELL_H_INCLUDED


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

int path() { //Define caminho(s) para busca de executáveis.
    return 0;
}

int exec_command (char **args) {

    pid_t pid;

    pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0){

        int redirect = -1;
        for (int i = 0; args[i]; i++) {
            if (strcmp(args[i], ">") == 0) {
                redirect = i;
            }
        }

        if (redirect != -1) {
            args[redirect] = NULL; // fim dos args do comando
            FILE *out = freopen(args[redirect + 1], "w", stdout);
            if (!out) {
                perror("Erro ao redirecionar saída");
                exit(EXIT_FAILURE);
            }
        }

        if(strcmp(args[0], "ls") == 0){
            printf("\033[0;32m");               //  Mudando o terminal para a cor verde
            fflush(stdout);
            if (fflush(stdout) == EOF) {
                perror("fflush falhou");
            }
        }

        execvp(args[0], args);

        perror("execvp falhou");

        if (errno == ENOENT)
            _exit(127);  // comando não encontrado
        else
            _exit(126);  // não executável ou outro erro
    }

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

    printf("\033[0m");  // Resetar a cor antes de sair

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        // printf("Comando terminou com código %d\n", exit_code);
        return exit_code;
    }
    else if (WIFSIGNALED(status)) {
        int sig = WTERMSIG(status);
        fprintf(stderr, "Processo filho terminou por sinal %d\n", sig);
        return 128 + sig;  // convenção de shells para sinal
    }

    // Casos raros (ex.: parou por trap)
    return -1;
}

//  Fim dos comandos a implementar


//  Comandos testes
int teste() {
    printf("\nSuccess!!\n\n");
    return 0;
}

int help() {
    printf("\nHelp: Type 'teste' to test success or 'exit' to quit.\n\n");
    return 0;
}

void exiting(char **args){

    printf("\nExiting...\n\n");

    for (int i = 0; args[i]; i++) free(args[i]);

    free(args);

    exit(0);

}

//  Fim dos Comandos testes

#endif //SHELL_H_INCLUDED