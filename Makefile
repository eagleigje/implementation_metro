CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm

# Fichiers objets, liste de tous les fichiers à linker.
OBJ = truc.o liste.o config.o main.o

# Executable principal
TARGET = metro

all: $(TARGET)

# Règle pour compiler les fichiers .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Règle pour créer l'exécutable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Nettoyage
clean:
	rm -f *.o $(TARGET)

rebuild:
	$(MAKE) clean
	$(MAKE) all

# Dépendances
truc.o: truc.c truc.h coord.h station.h connexion.h
liste.o: liste.c liste.h truc.h
config.o: config.c config.h
main.o: main.c config.h liste.h

.PHONY: all clean
