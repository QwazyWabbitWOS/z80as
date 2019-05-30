CC = gcc
CFLAGS = -std=c99 -O2 -Wall -Wpedantic

z80as:	as0.o as1.o as2.o as3.o as4.o as5.o as6.o
	$(CC) -o z80as as0.o as1.o as2.o as3.o as4.o as5.o as6.o

install:
	strip z80as
	cp z80as ${HOME}/bin/z80as

clean:
	rm -f *.o

all:
	make clean
	make
