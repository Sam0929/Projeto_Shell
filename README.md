# Projeto Shell

## Requisitos:

### . VM com Sistema Operacional Linux

## Instruções para compilação:

### Inicie a VM

### Dentro da VM, compartilhe a pasta do seu SO, onde o projeto foi clonado, com a VM:
```sh
sudo mount -t vboxsf "NomeDaPastaCompartilhada" "PastaVM"
```
### Instale o readline
```sh
sudo apt install libreadline-dev
```
### Mude para o diretório que o mount foi executado "PastaVM":
```sh
cd PastaVM
```
### Para compilar o projeto:
```sh
make
```
### Para Executar o Shell:
```sh
./bin/Shell /usr/bin
```
### Especifique o diretorio que contem os comandos externos, caso use "./bin/Shell":
```sh
path /usr/bin
```
