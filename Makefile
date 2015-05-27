
CC = gcc
CFLAGS = -Wall
LDFLAGS =
BIN_DIR = bin/
AOUT = example

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: $(AOUT)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

example: $(OBJ)
	$(CC) -o $(BIN_DIR)/$(AOUT) $(OBJ) $(LDFLAGS)

clean:
	rm -rf $(OBJ)
	rm -rf $(AOUT)
