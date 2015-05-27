
CC = gcc
CFLAGS = -Wall
LDFLAGS =
BIN_DIR = examples/bin/
AOUT = example

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

example: examples/example.c $(OBJ)
	$(CC) -o $(BIN_DIR)/$(AOUT) examples/example.c $(OBJ) $(LDFLAGS)

clean:
	rm -rf $(OBJ)
	rm -rf $(BIN_DIR)/$(AOUT)
