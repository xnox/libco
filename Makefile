.PHONY: all
all:
	$(CC) -c -Wall -fpic libco.c -DLIBCO_MP
	$(CC) -shared -o libco.so libco.o
	$(AR) rcs libco.a libco.o
