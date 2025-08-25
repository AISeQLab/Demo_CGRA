CC=gcc
CFLAGS=-I.
DEPS = sph_blake.h sph_types.h
OBJ = blake-256_print.o blake-256_time.o blake-256_write.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

blake-256_print: blake-256_print.o
	$(CC) -o $@ $^ $(CFLAGS)

blake-256_time: blake-256_time.o
	$(CC) -o $@ $^ $(CFLAGS)
	
blake-256_write: blake-256_write.o
	$(CC) -o $@ $^ $(CFLAGS)
	
clean: 
	rm -f *.o *~ core *~ 