all: server client
	
server: server.c
	gcc server.c -o server

client: client.c
	gcc -pthread client.c -o client

clean:
	rm client server users
