.PHONY: all
all:
	$(CC) -c -Wall -fpic libco.c
	$(CC) -shared -o libco.so libco.o
	$(AR) rcs libco.a libco.o
