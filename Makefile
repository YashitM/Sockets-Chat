all: server client users

users:
	touch users
	
server: server.c
	gcc -pthread server.c -o server

client: client.c
	gcc -pthread client.c -o client

clean:
	rm client server users
