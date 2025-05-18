CC = gcc
PROGRAM = hash
FLAGS = -I/opt/homebrew/opt/openssl@3/include -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto

all: hash

$(PROGRAM): main.c option.o hash.o
	$(CC) -o hash main.c option.o hash.o $(FLAGS)

hash.o: hash.c hash.h
	$(CC) -c hash.c -o hash.o $(FLAGS)

option.o: option.c option.h
	$(CC) -c option.c -o option.o