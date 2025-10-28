CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm

# Fichiers objets
OBJ = config.o main.o

# Executable principal
TARGET = metro

all: $(TARGET)

# Regle pour compiler les fichiers .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regle pour creer l'executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Nettoyage
clean:
	rm -f *.o $(TARGET)

# Dependances
config.o: config.c config.h
main.o: main.c config.h

.PHONY: all clean
