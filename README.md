# Projeto Shell

## Requisitos:

### . VM com Sistema Operacional Linux

## Instruções para compilação:

### Inicie a VM

### Dentro da VM, compartilhe a pasta do seu SO, onde o projeto foi clonado, com a VM:
```sh
sudo mount -t vboxsf "NomeDaPastaCompartilhada" "PastaVM"
```
### Mude para o diretório que o mount foi executado "PastaVM":
```sh
cd PastaVM
```
### Para compilar o projeto:
```sh
gcc -c Shell.c
gcc -o Shell Shell.o -lreadline
```

### Para Executar o Shell:
```sh
./Shell
```
