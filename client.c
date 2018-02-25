#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080
#define true 1
#define false 0

#define user_file_name "users"

char username[100] = "";

int get_user_count() {
    int lines = 1, c = 0;
    FILE *f = fopen(user_file_name, "r");
    if (f)
    {
        for (c = getc(f); c != EOF; c = getc(f)) {
            if (c == '\n') 
            {
                lines = lines + 1;
            }
        }
        return lines;
    }
    else {
        return 0;
    }
}

void add_online_user() {

    int counter = get_user_count();

    FILE *file = fopen(user_file_name, "a");

    if (file) 
    {
        char string_for_file[1000];
        sprintf(string_for_file, "%d) %s\n", counter, username);
        fputs(string_for_file, file);
        fclose(file);
    }
    else {
        printf("Some Error Ocurred. -> Add Online User.\n");
    }
}

int get_online_users() {
    int c;
    FILE *file;
    file = fopen("users_online", "r");
    if (file)
    {
        while ((c = getc(file)) != EOF)
            putchar(c);
        fclose(file);
    }
    else {
        printf("No online users\n");
        return -1;
    }
}

void remove_online_user() {
    FILE *f;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    f = fopen(user_file_name, "r");
    if (f) {
        while ((read = getline(&line, &len, f)) != -1)
        {
            printf("Retrieved line of length %zu :\n", read);
            printf("%s", line);
        }
    }

    fclose(f);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
}

void user_setup() {
    printf("Enter your Username: ");
    fgets(username, 100, stdin);
    username[strcspn(username, "\n")] = 0;
    add_online_user();
}

int main(int argc, char const *argv[])
{
    struct sockaddr_in server;
    int socket_file_descriptor = 0;
    char message_buffer[1024] = "";

    if ((socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Couldn't create Socket\n");
        return 0;
    }

    memset(&server, '0', sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(8090);

    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) < 0)
    {
        printf("Could not Connect to Localhost\n");
        return 0;
    }

    if ((connect(socket_file_descriptor, (struct sockaddr *)&server, sizeof(server))) < 0)
    {
        printf("Couldn't connect to Server\n");
        return 0;
    }

    user_setup();

    // Todo: Add check for whether users exist or not
    // Todo: Add signal handler for ctrl + c

    while(true) {

        char local_message[1024];

        memset(message_buffer, 0, sizeof(message_buffer));
        strcat(message_buffer, username);
        strcat(message_buffer, ": ");
        printf("Enter your message: ");
        fgets(local_message, 900, stdin);
        if (strstr(message_buffer, "exit"))
        {
            remove_online_user();
            break;
        }
        strcat(message_buffer, local_message);
        if ((send(socket_file_descriptor, message_buffer, strlen(message_buffer), 0)) < 0)
        {
            printf("Couldn't send message\n");
            return 0;
        }
    }
    close(socket_file_descriptor);
    return 0;
}