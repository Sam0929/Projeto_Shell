#ifndef SHELL_H_INCLUDED
#define SHELL_H_INCLUDED


// Comandos a implementar

void cd() { //Muda o diretório de trabalho.
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
void ls () { // lista o conteúdo do diretório atual. Seu ls deve suportar os parâmetros -l e -a conforme o funcionamento do ls original.
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

void exit_program() {
    printf("\nExiting...\n\n");
    
    exit(0);  // Exit the program
}

//  Fim dos Comandos testes

#endif //SHELL_H_INCLUDED