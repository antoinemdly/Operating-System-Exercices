CC = gcc
PROGRAM = ultrals
FLAGS = 

all: $(PROGRAM)

$(PROGRAM): main.c option.o files.o utilities.o
	$(CC) -o $(PROGRAM) main.c option.o files.o utilities.o $(FLAGS)

files.o: files.c files.h utilities.o
	$(CC) -c files.c -o files.o

option.o: option.c option.h
	$(CC) -c option.c -o option.o

utilities.o: utilities.c utilities.h
	$(CC) -c utilities.c -o utilities.o