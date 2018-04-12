QKOBJS = rdrand.o quickrdrand.o #aes128k128d.o
WROBJS = rdrand.o webrandmeg.o #aes128k128d.o
EXECS =  quickrdrand webrandmeg
CC = gcc
CFLAGS = -static

all: $(EXECS)

clean:
	rm -f *.o
	rm -f *.s
	rm -f quickrdrand
	rm -f webrandmeg

install: quickrdrand
	cp  $(EXECS) /usr/local/bin

webrandmeg: $(WROBJS) 
	gcc -O2 -s $(WROBJS) -lm -o webrandmeg

quickrdrand: $(QKOBJS) 
	gcc -O2 -s $(QKOBJS) -lm -o quickrdrand

webrandmeg.o: webrandmeg.c 
	$(CC) $(CFLAGS) -c webrandmeg.c -o webrandmeg.o

quickrdrand.o: quickrdrand.c 
	$(CC) $(CFLAGS) -c quickrdrand.c -o quickrdrand.o

rdrand.o: rdrand.c
	$(CC) $(CFLAGS) -c rdrand.c -o rdrand.o

#aes128k128d.o: aes128k128d.c aes128k128d.h 
#	$(CC) $(CFLAGS) -c aes128k128d.c -o aes128k128d.o

