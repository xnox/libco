.PHONY: all
all:
	gcc -c -Wall -fpic libco.c
	gcc -shared -o libco.so libco.o
	ar rcs libco.a libco.o
