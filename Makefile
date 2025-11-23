# =========================================================================
# Makefile pour le Projet Metro 2025
# =========================================================================
# Commandes disponibles :
#   make        - Compile les deux exécutables (metro et tests).
#   make build  - Compile les deux exécutables (metro et tests).
#   make all    - Nettoie et recompile tout depuis zéro (rebuild complet).
#   make metro  - Compile uniquement l'application principale.
#   make tests  - Compile uniquement le programme de tests.
#   make clean  - Supprime tous les fichiers objets et les exécutables.
#   make docs   - Génère la documentation Doxygen.
#   make clean-docs - Supprime la documentation générée.
# =========================================================================
CC = gcc
# On peut forcer la version de C ci-dessous en ajoutant -std=cXX où XX peut valoir 99, 17, 23, etc.
# La version C la plus récente à ce jour est la version c23.
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm

# Fichiers objets communs
COMMON_OBJ = truc.o liste.o config.o abr.o

# Fichiers objets spécifiques
MAIN_OBJ = main.o
TESTS_OBJ = tests.o

# Exécutables
TARGET_METRO = metro
TARGET_TESTS = tests

# Compilation normale incrémentable. Compilation par défaut (les deux exécutables).
build: $(TARGET_METRO) $(TARGET_TESTS)

# Rebuild complet : nettoie et recompile tout
all: clean $(TARGET_METRO) $(TARGET_TESTS)
	@echo "Rebuild complet terminé avec succès!"

# Nettoyage
clean:
	rm -f *.o $(TARGET_METRO) $(TARGET_TESTS)

# Dossier de documentation
DOCS_DIR = doc/projet
# Nettoyage de la documentation
clean-docs:
	rm -rf $(DOCS_DIR)
	@echo "Documentation supprimée!"

# Génération de la documentation Doxygen
docs:
	@echo "Génération de la documentation Doxygen..."
	@if [ ! -f Doxyfile ]; then \
		echo "Création du fichier Doxyfile..."; \
		doxygen -g; \
	fi
	doxygen Doxyfile
	@echo "Documentation générée dans $(DOCS_DIR)/html/"
	@echo "Ouvrez $(DOCS_DIR)/html/index.html dans un navigateur"

# Règle pour compiler les fichiers .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Règle pour créer l'exécutable metro
$(TARGET_METRO): $(COMMON_OBJ) $(MAIN_OBJ)
	$(CC) $(CFLAGS) $(COMMON_OBJ) $(MAIN_OBJ) -o $(TARGET_METRO) $(LDFLAGS)

# Règle pour créer l'exécutable tests
$(TARGET_TESTS): $(COMMON_OBJ) $(TESTS_OBJ)
	$(CC) $(CFLAGS) $(COMMON_OBJ) $(TESTS_OBJ) -o $(TARGET_TESTS) $(LDFLAGS)

# Dépendances
truc.o: truc.c truc.h coord.h station.h connexion.h
abr.o: abr.c abr.h abr_type.h liste.h
liste.o: liste.c liste.h truc.h
config.o: config.c config.h
main.o: main.c config.h liste.h abr.h
tests.o: tests.c tests.h config.h liste.h abr.h

.PHONY: all build clean metro tests
