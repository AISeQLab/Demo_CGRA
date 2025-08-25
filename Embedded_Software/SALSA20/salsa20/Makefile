CC = gcc
CFLAGS = -Wall -Wextra
DEBUGFLAGS = -DDEBUG

all: salsa20

salsa20: salsa20.c
	$(CC) $(CFLAGS) -o salsa20 salsa20.c

debug: salsa20.c
	$(CC) $(DEBUGFLAGS) -o salsa20 salsa20.c

clean:
	rm -f salsa20