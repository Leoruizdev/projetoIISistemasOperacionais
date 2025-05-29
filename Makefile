# Makefile - Simulador de Paginação

CC = gcc
CFLAGS = -Wall -Wextra -std=c99

TARGET = simulador
OBJS = main.o simulador.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c estruturas.h
	$(CC) $(CFLAGS) -c main.c

simulador.o: simulador.c estruturas.h
	$(CC) $(CFLAGS) -c simulador.c

clean:
	rm -f *.o $(TARGET)
