CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm

# Fichiers objets
OBJ = config.o main.o

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

# Dépendances
config.o: config.c config.h
main.o: main.c config.h

.PHONY: all clean
