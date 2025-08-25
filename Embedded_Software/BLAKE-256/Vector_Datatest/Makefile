CC=gcc
CFLAGS=-I.
OBJ = datatest_creator.o

%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

main: datatest_creator.o
	$(CC) -o $@ $^ $(CFLAGS)
	
clean: 
	rm -f *.o *~ core *~ 