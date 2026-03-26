# 🖥️ Projeto Shell

Este projeto consiste na implementação de uma **shell simples**, desenvolvida com o objetivo de compreender melhor o funcionamento de sistemas operacionais, chamadas de sistema e gerenciamento de processos.

## 📌 Objetivo

O principal objetivo deste projeto é simular o comportamento básico de um terminal, permitindo a execução de comandos e interação com o sistema operacional.

## ⚙️ Funcionalidades

- Execução de comandos do sistema
- Criação de processos (fork)
- Execução de programas (exec)
- Espera de processos (wait)
- Tratamento básico de entrada do usuário
- Suporte a comandos internos (se aplicável)

## 🛠️ Tecnologias Utilizadas

- Linguagem C
- Sistema operacional Linux
- Bibliotecas padrão do C (`stdio.h`, `stdlib.h`, `unistd.h`, etc.)

## 📚 Aprendizados do Projeto

* **Modularização:** Organização do código em múltiplos arquivos, facilitando manutenção, leitura e reutilização.

* **Estrutura `src/` e `include/`:** Separação entre implementação (`.c`) e cabeçalhos (`.h`), seguindo boas práticas de projetos em C.

* **Interpretação de comandos:** Compreensão de como um shell lê, processa (parsing) e executa comandos digitados pelo usuário.

* **Execução de programas:** Uso de chamadas de sistema como `fork()`, `exec()` e `wait()` para criar e gerenciar processos.

* **IPC (Comunicação entre Processos):** Como processos trocam informações e se sincronizam, base para recursos como pipes.


## 🚀 Como executar

### 1. Clone o repositório

```bash
git clone https://github.com/Sam0929/Projeto_Shell.git
cd Projeto_Shell
```

### 2. Compile o projeto

```bash
make
```

### 3. Execute

```bash
./shell
```

## 💡 Exemplo de uso

```bash
ls
pwd
echo "Olá mundo"
```

## 📂 Estrutura do Projeto

* **src/** - Contém os arquivos de código-fonte em C.

* **include/** - Contém os arquivos de cabeçalho (.h).

* **sum.c** - Código de exemplo/utilitário presente no repositório.

* **makefile** - Script de regras para a compilação automatizada.

## 👤 Autor

* Desenvolvido por Samuel.


