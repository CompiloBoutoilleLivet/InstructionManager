
CC = gcc
CFLAGS = -Wall
LDFLAGS =
AOUT = example

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: $(AOUT)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

example: $(OBJ)
	$(CC) -o $(AOUT) $(OBJ) $(LDFLAGS)

clean:
	rm -rf $(OBJ)
	rm -rf $(AOUT)
