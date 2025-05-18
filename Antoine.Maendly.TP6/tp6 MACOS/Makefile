CC = gcc
MODULE1 = builtin
MODULE2 = argument
MODULE3 = jobs

all: shell $(MODULE1) $(MODULE2)

clean:
	rm shell $(MODULE1) $(MODULE2) $(MODULE3)

shell: shell.c $(MODULE1).o $(MODULE2).o $(MODULE3).o
	$(CC) -o shell shell.c $(MODULE1).o $(MODULE2).o $(MODULE3).o

$(MODULE1): $(MODULE1).c $(MODULE1).h
	$(CC) -c $(MODULE1).c -o $(MODULE1).o

$(MODULE2): $(MODULE2).c $(MODULE2).h
	$(CC) -c $(MODULE2).c -o $(MODULE2).o

$(MODULE3): $(MODULE3).c $(MODULE3).h
	$(CC) -c $(MODULE3).c -o $(MODULE3).o