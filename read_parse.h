#ifndef READ_PARSE_H_INCLUDED           // BIBLIOTECA PARA I/O
#define READ_PARSE_H_INCLUDED

// Prototype

char **parsing (char *input, char *delimiter, int def);
char **reading ();
char **pipe_parsing   (char *linha);
// Functions to read and parse

char **reading(){

    char **tokens;

    char *linha = readline(">>> ");

    if (linha == NULL) {
        // Ctrl+D
        printf("\n");
        exit(0);
    }
    if (strlen(linha) > 0) {
        add_history(linha);  // adiciona ao historico se nao for vazia
    }

    if (strchr(linha, '|')){  // Se nao tiver o caracter NULL e retornado e o if e invalido
        pipe_parsing(linha);
        
    }

    if(strchr(linha, '&')){
        printf("SIM PARALELO");
        fflush(stdout);
    }

    tokens = parsing(linha, " \t\n", 1);

    free(linha);

    return tokens;

}

char **parsing (char *input, char *delimiter, int def){


    char **args = malloc(50 * sizeof(char *));  // Lista de argumentos
    char *token = strtok(input, delimiter);       // " \t\n" // strtok encontra o delimitador e substitui ele por \0

    int i = 0;

    while (token != NULL) {

        args[i++] = strdup(token);           //Copia a string até o primeiro \0 para args
        token = strtok(NULL, delimiter);
    }


    if (i != 0){
        for (int i = 0; args[0][i]; i++){
            args [0][i] = tolower(args[0][i]);  // Padronizando o comando a ser reconhecido
        }
    }

    if (def){
        args[i] = NULL;
    }
   

    return args;

}

char **pipe_parsing(char *linha){

    int n_commands = 0;
    char **pipe_tokens;
    pipe_tokens = parsing(linha, "|", 0);
    
    while (pipe_tokens[n_commands]) n_commands++;

    char ***commands = malloc((n_commands + 1) * sizeof(char**)); // Lista de lista de strings

        for (int i = 0; i < n_commands; i++) {
        commands[i] = parsing(pipe_tokens[i], " \t\n", 1);
    }
        
    for (int i = 0; i < n_commands; i++) {
        printf("Comando %d:\n", i);
        for (int j = 0; commands[i][j]; j++) {
            printf("  [%d][%d] = '%s'\n", i, j, commands[i][j]);
        }
    }

    return commands[0];  // Isso é para testes, depois teria que ter outra função para receber return commands;

}


#endif   //READ_PARSE_H_INCLUDED