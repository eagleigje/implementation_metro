# =========================================================================
# Makefile pour le Projet Metro 2025
# =========================================================================
# Commandes disponibles :
#   make        - Compile uniquement les fichiers modifiés (incrémental)
#   make build  - Compile uniquement les fichiers modifiés (incrémental)
#   make all    - Nettoie et recompile tout depuis zéro (rebuild complet)
#   make clean  - Supprime tous les fichiers objets et l'exécutable
# =========================================================================
CC = gcc
# On peut forcer la version de C ci-dessous en ajoutant -std=cXX où XX peut valoir 99, 17, 23, etc.
# La version C la plus récente à ce jour est la version c23.
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm

# Fichiers objets, liste de tous les fichiers à linker.
OBJ = truc.o liste.o config.o main.o

# Executable principal
TARGET = metro

# Compilation normale incrémentable. Compilation par défaut.
build: $(TARGET)

# Rebuild complet : nettoie et recompile tout
all: clean $(TARGET)
	@echo "Rebuild complet terminé avec succès!"

# Nettoyage
clean:
	rm -f *.o $(TARGET)

# Règle pour compiler les fichiers .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Règle pour créer l'exécutable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Dépendances
truc.o: truc.c truc.h coord.h station.h connexion.h
liste.o: liste.c liste.h truc.h
config.o: config.c config.h
main.o: main.c config.h liste.h

.PHONY: all build clean
