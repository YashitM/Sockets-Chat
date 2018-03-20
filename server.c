#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

#define port 9092
#define MAX_CONNECTIONS 5
#define user_file_name "users"

struct thread_arguments {
    int socket_fd;
    int connection_fd;
};

int current_connections[10];
int current_connection_length = 0;

void print_all_connections() {
    int i;
    for(i=0 ; i<current_connection_length; i++) {
        printf("%d\n", current_connections[i]);
    }
}

void remove_connection(int connection_id) {
    int i=0, j=0;
    for(i=0; i<current_connection_length; i++) {
        if(current_connections[i] == connection_id) {
            for (j = i; j < current_connection_length; j++)
                current_connections[j] = current_connections[j + 1];
        }
    }
    current_connection_length -= 1;
}

int add_online_user(char *username, int connection)
{
    FILE *file;
    file = fopen(user_file_name, "a");

    if (file)
    {
        char string_for_file[1000];
        sprintf(string_for_file, "%d) %s;\n", connection, username);
        fputs(string_for_file, file);
        fclose(file);
        return true;
    }
    else
    {
        printf("Couldn't add a new user.\n");
        return false;
    }
}

void *handle_connection(void *thread_arg) {
    struct thread_arguments *thread_args = thread_arg;
    int connection_fd = thread_args->connection_fd;
    int socket_fd = thread_args->socket_fd;
    char message_buffer[1024];

    printf("User Connected: %d\n", connection_fd);

    memset(message_buffer, 0, sizeof(message_buffer));

    while (recv(connection_fd, message_buffer, 256, 0) > 0)
    {
        if (strstr(message_buffer, "exithard"))
        {
            printf("User Disconnected: %d\n", connection_fd);
            remove_connection(connection_fd);
            break;
        }
        printf("%s", message_buffer);
        if (strstr(message_buffer, ";;;;;")) {
            char *pch;
            pch = strtok(message_buffer, ";;;;;");
            char *message_array[100];
            memset(message_array, 0, sizeof(message_array));
            int line_counter = 0;
            while (pch != NULL)
            {
                message_array[line_counter++] = pch;
                pch = strtok(NULL, ";;;;;");
            }
            char final_message[1000];
            memset(final_message, 0, sizeof(final_message));
            strcat(final_message, message_array[0]);
            strcat(final_message, message_array[line_counter-1]);
            int iterator = 0;
            // printf("Line counter: %d\n", line_counter);
            // for(; iterator<line_counter; iterator++) {
            //     printf("%s\n", message_array[iterator]);
            // }
            // printf("Final Message: %s\n", final_message);
            iterator = 1;
            for (; iterator < line_counter - 1; iterator ++) {
                int conn_id = (int)strtol(message_array[iterator], (char **)NULL, 10);
                if (conn_id != connection_fd)
                    send(conn_id, final_message, sizeof(final_message), 0);
            }
        }
        else {
            int i = 0;
            printf("Messgae Recieved: %s", message_buffer);
            for (i = 0; i < current_connection_length; i++)
            {
                if (current_connections[i] != connection_fd)
                {
                    send(current_connections[i], message_buffer, sizeof(message_buffer), 0);
                }
            }
        }
        // printf("Number of Users: %d\n", current_connection_length);
        memset(message_buffer, 0, sizeof(message_buffer));
    }
    close(connection_fd);
}


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
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(socket_file_descriptor, (struct sockaddr *)&server, sizeof(server)) != 0) {
        printf("Port Busy\n");
        return 0;
    }

    listen(socket_file_descriptor, MAX_CONNECTIONS);

    pthread_t threads[MAX_CONNECTIONS];

    while (connection = accept(socket_file_descriptor, (struct sockaddr *)NULL, NULL))
    {
        current_connections[current_connection_length++] = connection;

        char username_buffer[100] = "";

        recv(connection, username_buffer, 100, 0);
        add_online_user(username_buffer, connection);
        struct thread_arguments thread_args;

        thread_args.socket_fd = socket_file_descriptor;
        thread_args.connection_fd = connection;

        pthread_create(&threads[current_connection_length], NULL, &handle_connection, (void *)&thread_args);
        pthread_join(&threads[current_connection_length], NULL);
    }
    return 0;
}