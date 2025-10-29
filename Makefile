# ========================================
# Makefile pour le Projet Metro 2025
# ========================================
# Commandes disponibles :
#   make all    - Nettoie et recompile tout depuis zero (rebuild complet)
#   make build  - Compile uniquement les fichiers modifies (incremental)
#   make clean  - Supprime tous les fichiers objets et l'executable
# ========================================
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm

# Fichiers objets, liste de tous les fichiers à linker.
OBJ = truc.o liste.o config.o main.o

# Executable principal
TARGET = metro

# Rebuild complet : nettoie et recompile tout
all: clean $(TARGET)
	@echo "Rebuild complet termine avec succes!"

# Compilation normale sans nettoyage
build: $(TARGET)

# Règle pour compiler les fichiers .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Règle pour créer l'exécutable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Nettoyage
clean:
	rm -f *.o $(TARGET)

# Dépendances
truc.o: truc.c truc.h coord.h station.h connexion.h
liste.o: liste.c liste.h truc.h
config.o: config.c config.h
main.o: main.c config.h liste.h

.PHONY: all build clean
