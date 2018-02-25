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
    server.sin_port = htons(8090);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(socket_file_descriptor, (struct sockaddr *)&server, sizeof(server));

    listen(socket_file_descriptor, 5);
    while (connection = accept(socket_file_descriptor, (struct sockaddr *)NULL, NULL))
    {
        if ((pid = fork()) == 0)
        {
            printf("New User Connected!\n");

            close(socket_file_descriptor);

            memset(message_buffer, 0, sizeof(message_buffer));

            while (recv(connection, message_buffer, 256, 0) > 0)
            {
                printf("%s", message_buffer);
                memset(message_buffer, 0, sizeof(message_buffer));
            }
            close(connection);
            exit(0);
        }
    }
    return 0;
}