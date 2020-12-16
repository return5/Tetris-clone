CFLAGS = -std=gnu99 -Wall -Wextra -O2 
LIBFLAGS = -lncurses 
CC = gcc
SRC = ./src/
OUTFILE = tetris_clone

all:
	$(CC) $(CFLAGS) -o$(OUTFILE) $(SRC)*.c $(LIBFLAGS)

clean:
	rm -f *.o *.gch
