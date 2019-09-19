# Makefile


CC=arm-unknown-linux-gnu-gcc

LDFLAGS=-static


all: main
clean:
	rm main

main:   Makefile main.c
	$(CC) main.c \
	-I./ -I./include/ -O \
	-lm -ldl -lrt -lpthread -static -o main

