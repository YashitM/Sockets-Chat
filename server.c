#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

int main()
{
    int socket_file_descriptor = 0, pid, connection;
    struct sockaddr_in server;
    char message_buffer[1024] = "";

    if ((socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket could not be created\n");
        return 0;
    }

    memset(&server, '0', sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(9090);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(socket_file_descriptor, (struct sockaddr *)&server, sizeof(server)) != 0) {
        printf("Port Busy\n");
        return 0;
    }

    listen(socket_file_descriptor, 5);

    while (connection = accept(socket_file_descriptor, (struct sockaddr *)NULL, NULL))
    {
        if ((pid = fork()) == 0)
        {
            printf("User Connected\n");

            close(socket_file_descriptor);

            memset(message_buffer, 0, sizeof(message_buffer));

            while (recv(connection, message_buffer, 256, 0) > 0)
            {
                if(strstr(message_buffer,"exit")) {
                    printf("User Disconnected\n");
                    break;
                }
                printf("%s", message_buffer);
                // send(connection, message_buffer, sizeof(message_buffer), 0);
                memset(message_buffer, 0, sizeof(message_buffer));
            }
            close(connection);
            exit(0);
        }
    }
    return 0;
}