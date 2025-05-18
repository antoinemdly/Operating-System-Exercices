CC = gcc

all: server client

clean:
	rm server client

server: server.c
	$(CC) server.c -o server

client: client.c
	$(CC) client.c -o client

run_client:
	./client 127.0.0.1 8082

run_server:
	./server 8082