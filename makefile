CC=gcc
ASMBIN=nasm

all: encode paint cc link
encode:
	$(ASMBIN) -o encode.o -f elf64 encode.asm
paint:
	$(ASMBIN) -o paint.o -f elf64 paint.asm
cc:
	$(CC) -c -g -O0 main.c
link:
	$(CC) -o release main.o encode.o paint.o
debug:
	gdb release
clean:
	rm *.o release
