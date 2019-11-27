QKOBJS = rdrand_stdint.o quickrdrand.o
WROBJS = rdrand_stdint.o webrandmeg.o
EXECS =  quickrdrand webrandmeg
CC = gcc
CFLAGS = -mrdrnd -mrdseed

all: $(EXECS)

clean:
	rm -f *.o
	rm -f *.s
	rm -f quickrdrand
	rm -f webrandmeg

install: quickrdrand
	cp  $(EXECS) /usr/local/bin

webrandmeg: $(WROBJS) 
	gcc -O2 $(CFLAGS) $(WROBJS) -lm -o webrandmeg

quickrdrand: $(QKOBJS) 
	gcc -O2  $(CFLAGS) $(QKOBJS) -lm -o quickrdrand

webrandmeg.o: webrandmeg.c 
	$(CC) $(CFLAGS) -c webrandmeg.c -o webrandmeg.o

quickrdrand.o: quickrdrand.c 
	$(CC) $(CFLAGS) -c quickrdrand.c -o quickrdrand.o

rdrand_stdint.o: rdrand_stdint.c rdrand_stdint.h
	$(CC) $(CFLAGS) -c rdrand_stdint.c -o rdrand_stdint.o



