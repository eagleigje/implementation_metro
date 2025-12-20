# =========================================================================
# Makefile pour le Projet Metro 2025
# =========================================================================
# Commandes disponibles :
#   make             - Compile les deux exécutables (metro et tests).
#   make all         - Compile les deux exécutables (metro et tests).
#   make rebuild     - Nettoie et recompile tout depuis zéro (rebuild complet).
#   make metro       - Compile uniquement l'application principale.
#   make tests       - Compile uniquement le programme de tests.
#   make run-tests   - Compile et exécute les tests.
#   make test-memory - Compile et exécute les tests avec valgrind.
#   make clean       - Supprime tous les fichiers objets et les exécutables.
#   make docs        - Génère la documentation Doxygen.
#   make clean-docs  - Supprime la documentation générée.
# =========================================================================
CC = gcc
# On peut forcer la version de C ci-dessous en ajoutant -std=cXX où XX peut valoir 99, 17, 23, etc.
# La version C la plus récente à ce jour est la version c23.
CFLAGS = -Wall -Wextra -g -O3
LDFLAGS_METRO = -lm -lSDL2 -lSDL2_ttf
LDFLAGS_TESTS = -lm

# Fichiers objets communs
COMMON_OBJ = truc.o liste.o config.o abr.o ligne.o connexion.o csv_utils.o aqrtopo.o outils.o dijkstra.o
GUI_OBJ = gui/gui.o gui/gui_commun.o gui/gui_menu.o gui/gui_plan.o

# Fichiers objets spécifiques
MAIN_OBJ = main.o
TESTS_OBJ = tests.o tests_projet/test_csv_utils.o tests_projet/test_exercice1.o \
            tests_projet/test_exercice2.o tests_projet/test_exercice3.o \
            tests_projet/test_exercice4.o tests_projet/test_exercice5.o \
            tests_projet/test_exercice6.o \
            tests_projet/test_autres.o

# Exécutables
TARGET_METRO = metro
TARGET_TESTS = tests

# Compilation normale incrémentable. Compilation par défaut (les deux exécutables).
all: $(TARGET_METRO) $(TARGET_TESTS)

# Rebuild complet : nettoie et recompile tout
rebuild: clean $(TARGET_METRO) $(TARGET_TESTS)
	@echo "Rebuild complet terminé avec succès!"

# Nettoyage
clean:
	rm -f *.o tests_projet/*.o gui/*.o $(TARGET_METRO) $(TARGET_TESTS)

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
$(TARGET_METRO): $(COMMON_OBJ) $(GUI_OBJ) $(MAIN_OBJ)
	$(CC) $(CFLAGS) $(COMMON_OBJ) $(GUI_OBJ) $(MAIN_OBJ) -o $(TARGET_METRO) $(LDFLAGS_METRO)

# Règle pour créer l'exécutable tests
$(TARGET_TESTS): $(COMMON_OBJ) $(TESTS_OBJ)
	$(CC) $(CFLAGS) $(COMMON_OBJ) $(TESTS_OBJ) -o $(TARGET_TESTS) $(LDFLAGS_TESTS)

# Dépendances
truc.o: truc.c truc.h coord.h station.h connexion.h
abr.o: abr.c abr.h abr_type.h liste.h truc.h
liste.o: liste.c liste.h truc.h station.h connexion.h coord.h csv_utils.h constantes.h
config.o: config.c config.h outils.h
ligne.o: ligne.c ligne.h constantes.h csv_utils.h
connexion.o: connexion.c connexion.h ligne.h abr.h liste.h truc.h constantes.h csv_utils.h
csv_utils.o: csv_utils.c csv_utils.h outils.h
aqrtopo.o: aqrtopo.c aqrtopo.h truc.h liste.h
outils.o: outils.c outils.h
dijkstra.o: dijkstra.c dijkstra.h liste.h truc.h
main.o: main.c config.h gui/gui.h
gui/gui.o: gui/gui.c gui/gui.h gui/gui_commun.h gui/gui_menu.h gui/gui_plan.h config.h liste.h abr.h aqrtopo.h ligne.h connexion.h dijkstra.h
gui/gui_commun.o: gui/gui_commun.c gui/gui_commun.h gui/gui.h
gui/gui_menu.o: gui/gui_menu.c gui/gui_menu.h gui/gui.h gui/gui_commun.h
gui/gui_plan.o: gui/gui_plan.c gui/gui_plan.h gui/gui.h gui/gui_commun.h
tests.o: tests.c tests.h config.h liste.h abr.h
tests_projet/test_csv_utils.o: tests_projet/test_csv_utils.c tests_projet/test_csv_utils.h csv_utils.h
tests_projet/test_exercice1.o: tests_projet/test_exercice1.c tests_projet/test_exercice1.h liste.h truc.h
tests_projet/test_exercice2.o: tests_projet/test_exercice2.c tests_projet/test_exercice2.h abr.h liste.h truc.h
tests_projet/test_exercice3.o: tests_projet/test_exercice3.c tests_projet/test_exercice3.h config.h ligne.h
tests_projet/test_exercice4.o: tests_projet/test_exercice4.c tests_projet/test_exercice4.h config.h ligne.h liste.h abr.h connexion.h
tests_projet/test_exercice5.o: tests_projet/test_exercice5.c tests_projet/test_exercice5.h aqrtopo.h liste.h truc.h
tests_projet/test_exercice6.o: tests_projet/test_exercice6.c tests_projet/test_exercice6.h dijkstra.h liste.h truc.h ligne.h
tests_projet/test_autres.o: tests_projet/test_autres.c tests_projet/test_autres.h config.h liste.h abr.h

# Cible pour exécuter les tests
run-tests: $(TARGET_TESTS)
	@echo "========================================="
	@echo "Exécution des tests..."
	@echo "========================================="
	./$(TARGET_TESTS)

# Cible pour exécuter les tests avec valgrind
test-memory: $(TARGET_TESTS)
	@echo "========================================="
	@echo "Vérification mémoire avec valgrind..."
	@echo "========================================="
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET_TESTS)

.PHONY: all rebuild clean metro tests run-tests test-memory docs clean-docs
