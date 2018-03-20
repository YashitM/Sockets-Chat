# Sockets Chat

A Multi User Chat System built using Sockets in C. 

## Description

There are two files in this project, one being for the server and the other for the client. The server script will be running and multiple client scripts can be instantiated at the same time and can chat simultaneously.

### Server
In the server script, a socket is first instantiated and then the server port, the family etc are defined. then the connection is `bind`ed to a port for the clients to connect to it. And then the server starts listening for connections. There's a while loop which handles all teh connections and creates a new working pthread for each new connection. So for each connection, a recv() command is used to recieve the incomming data (from the client). This is done in a while loop as the client can send multiple messages. In this function, the server checks whether the message is to be sent to all the online users or only a specific user. This is done using some basic String Manipulation (which is reversed and checked in the server script). If the message is to be sent to all the users, a loop iterates over all the socket IDs available (the online users) and a message is sent to each and everyone. When a message is to be sent to a particular user, the Socket Connection ID of the user is manipulated from the string and the message is just sent to that user. So basically, the server acts as a middleman. All the messages are first sent to the server and then the server decides where to push the messages.

### Client
In the client script, again, a socket is first created and then the client searches for a connection on the mentioned port. Once a connection is found, the client is prompted to enter a username for his/her identification. Then, two independant threads are created (one being for taking input from the client and the other for receiving data from the server). This has been done so as to allow the client to both recieve and send messages simultaneously. Once the threads are started, the user is prompted to select whom he/she want's to chat with. A set of options are provided. Based on the selection of the user, the message string(that the user enters inorder to send the message to server ) is manipulated and the socket ID of the person (who is to recieve the message) is appended and then parsed by the server as mentioned above. If the user wishes to exit, there are two options, the user wants to exit the current chat session and move on to chatting with someone else. The other is when the user wants to completely exit the chat system. There is an outer while loop which handles this exiting part. The threads are terminated only when the user choses to exit the entire program (using the `exithard` command). A normal exit would be using the `exit` command. This assumes that the user wants to chat with some other user.

## Compilation
The compilation of the program can be easily done using the `make` command. `make clean` will clear all the binaries and other files that are required for the program to function.

## Inputs
The user (client) on successful connection with the server is expected to enter a useranme (for identification) and also select the user he/she wants to chat with. After this basic setup, the user can send messages to server (Even when the messages are being recieved, as reading and writing are done in two separate threads). To exit to the selection menu (to whom to chat with), the user can type `exit`. So as to exit the complete program, the user can type `exithard`.

## Output
If the user recieves a message, it will be prompted on the screen of the user (regardless of whose chat the user is focussed into). The name of the person who sent the message will also be displayed (for identification purposes).

## Error Values
In both the server and client scripts, if an error is detected in the socket creation, a corresponding message is displayed on the console.

`Couldn't create Socket`: When `socket()` function returns error
`Couldn't connect to Server`: When `connect()` function (in the client script return an error).
`Port Busy`: When the server cannot connect to the desired port.
`Couldn't add a new user`: When the user file cannot be created.

## Note
A `users` files is maintained so as to keep a log of the online users.






