# Compilador
CC = gcc

# --- Diretórios ---
SRCDIR = src
OBJDIR = obj
BINDIR = bin
INCLUDEDIR = include # Mesmo que você não use agora, é bom ter

# Flags de compilação:
# -I$(SRCDIR) e -I$(INCLUDEDIR) dizem ao compilador para procurar headers nessas pastas.
CFLAGS = -Wall -Wextra -g -I$(SRCDIR) -I$(INCLUDEDIR) --pedantic

# Flags de linkagem:
LDFLAGS = -lreadline

# --- Arquivos ---
# Nome do executável final
TARGET_NAME = Shell
TARGET = $(BINDIR)/$(TARGET_NAME)

# Lista dos NOMES BASE dos seus arquivos fonte .c (sem o src/)
SOURCES_NAMES = main.c read_parse.c shell_commands.c

# Constrói o caminho completo para os arquivos fonte em src/
SOURCES = $(foreach name,$(SOURCES_NAMES),$(SRCDIR)/$(name))

# Gera automaticamente a lista de arquivos objeto .o em obj/
# Ex: src/main.c -> obj/main.o
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

# --- Regras do Makefile ---

# Regra padrão (o que acontece se você só digitar "make")
all: $(TARGET)

# Regra para linkar os arquivos objeto e criar o executável
$(TARGET): $(OBJECTS)
	@echo "Ligando para criar $(TARGET)..."
	@mkdir -p $(BINDIR) # Cria o diretório bin/ se não existir
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "$_SUCCESS $(TARGET) criado com sucesso!"

# Regra genérica para compilar um arquivo .c (de src/) em um arquivo .o (em obj/)
# $< é o primeiro pré-requisito (o arquivo .c)
# $@ é o nome do alvo (o arquivo .o)
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo "Compilando $< -> $@..."
	@mkdir -p $(OBJDIR) # Cria o diretório obj/ se não existir
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para limpar os arquivos gerados
clean:
	@echo "Limpando arquivos gerados..."
	rm -rf $(OBJDIR) $(BINDIR)
	@echo "Limpeza concluída."

# Regra para reconstruir tudo (limpa e depois compila)
re: clean all

# Define cores para mensagens (opcional, para terminais que suportam)
_SUCCESS = [\033[0;32mSUCESSO\033[0m]

# Opcional: Declarar que 'all', 'clean', 're' não são nomes de arquivos
.PHONY: all clean re