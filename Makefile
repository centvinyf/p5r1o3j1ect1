all: server client



server: server.c
	gcc server.c -pthread -o server

client: client.c
	gcc client.c -o client



clean:
	rm *o CS_513_Project1

