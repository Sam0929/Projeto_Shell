#ifndef SHELL_H_INCLUDED
#define SHELL_H_INCLUDED


// Comandos a implementar

void cd(char **args) { //Muda o diretório de trabalho.

    int msg;

    msg = chdir(args [1]);

    if (msg == -1){
        // printf("Error Number: %d\n", errno);
        printf("Error Description: %s\n", strerror(errno));
    }
    
    return;
}
void path() { //Define caminho(s) para busca de executáveis.
    return;
}
void pwd () { // Exibir o caminho do diretório atual.
    return;
}
void cat () { // Lê o conteúdo do arquivo no argumento e o escreve na saída padrão.
    return;
}
void ls (char **args) { // lista o conteúdo do diretório atual. Seu ls deve suportar os parâmetros -l e -a conforme o funcionamento do ls original.

    pid_t PID;

    PID = fork();

    if (PID == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (PID == 0){

        int status_code = execvp(args[0], args);

        if (status_code == -1) {
            printf("Process did not terminate correctly\n");
            exit(1);
        }
    }

    wait(NULL);
    
    return;
}
//  Fim dos comandos a implementar


//  Comandos testes
void teste() {
    printf("\nSuccess!!\n\n");
}

void help() {
    printf("\nHelp: Type 'teste' to test success or 'exit' to quit.\n\n");
}

//  Fim dos Comandos testes

#endif //SHELL_H_INCLUDED