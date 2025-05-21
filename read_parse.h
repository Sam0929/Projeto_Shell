#ifndef READ_PARSE_H_INCLUDED           // BIBLIOTECA PARA I/O
#define READ_PARSE_H_INCLUDED

char **parsing (char input[100]);
char **reading ();

char **reading(){

    char *linha = readline(">>> ");

    if (linha == NULL) {
        // Ctrl+D
        printf("\n");
        exit(0);
    }
    if (strlen(linha) > 0) {
        add_history(linha);  // adiciona ao histórico se não for vazia
    }

    char **tokens = parsing(linha);

    free(linha);

    return tokens;

}

char **parsing (char *input){


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
#endif   //READ_PARSE_H_INCLUDED