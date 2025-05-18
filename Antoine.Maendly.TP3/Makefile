CC = gcc
PROGRAM = copybench
FLAGS = 
MODULE1 = copy

all: help

help:
	@echo "Available targets:"
	@echo "bigfile 	generates a 10MB file of random data from /dev/urandom"
	@echo "copybench 	generates the copybench executable"
	@echo "run 		executes copybench"
	@echo "clean 		deletes all generated files (including bigfile)"

$(PROGRAM): $(PROGRAM).c $(MODULE1).o
	$(CC) -o $(PROGRAM) $(PROGRAM).c $(MODULE1).o $(FLAGS)

$(MODULE1).o: $(MODULE1).c $(MODULE1).h
	$(CC) -c $(MODULE1).c -o $(MODULE1).o

bigfile:
	dd if=/dev/urandom of=bigfile bs=1000000 count=10

run:
	./$(PROGRAM) bigfile tmp

clean:
	rm -f $(PROGRAM) $(MODULE1).o bigfile tmp