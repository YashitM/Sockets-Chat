#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

#define true 1
#define false 0

#define user_file_name "users"
#define port 9090

char username[100] = "";
pthread_t thread_write, thread_read;

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

int check_already_exists(FILE *file) {
    int read;
    size_t len = 0;
    char username_delimiter[100] = "", *line = NULL;

    strcpy(username_delimiter, ") ");
    strcat(username_delimiter, username);
    strcat(username_delimiter, ";");

    while (getline(&line, &len, file) != -1)
    {
        if (strstr(line, username_delimiter))
        {
            printf("Username already exists :(\n");
            fclose(file);
            return true;
        }
    }
    fclose(file);
    return false;
}

int add_online_user() {

    FILE *file, *file2;
    int counter = get_user_count();

    file = fopen(user_file_name, "a");
    file2 = fopen(user_file_name, "r");

    if(file && file2)
    {
        if(check_already_exists(file2) == true) {
            return false;
        }
        char string_for_file[1000];
        sprintf(string_for_file, "%d) %s;\n", counter, username);
        fputs(string_for_file, file);
        fclose(file);
        return true;
    }
    else {
        printf("Couldn't add a new user.\n");
        return false;
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
    FILE *f, *f2;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char username_delimiter[100] = "";

    strcpy(username_delimiter, ") ");
    strcat(username_delimiter, username);
    strcat(username_delimiter, ";");

    f = fopen(user_file_name, "r");
    f2 = fopen("temp", "w");
    if (f && f2) {
        while ((read = getline(&line, &len, f)) != -1)
        {
            if(!strstr(line, username_delimiter)) {
                fputs(line, f2);
            }
        }
        fclose(f2);
        fclose(f);
        if (remove(user_file_name) != 0)
            printf("Couldn't delete users file\n");
        if (rename("temp", user_file_name) != 0)
            printf("Couldn't remove temp file\n");
    }
}

int user_setup() {
    while(true) {
        printf("Enter your Username: ");
        fgets(username, 100, stdin);
        username[strcspn(username, "\n")] = 0;
        if (add_online_user() == true) {
            break;
        }
    }
    return true;
}

void init() {
    printf("\n");
    printf("__          __   _                             _            _____ _           _   \n");
    printf("\\ \\        / /  | |                           | |          / ____| |         | |  \n");
    printf(" \\ \\  /\\  / /___| | ___  ___  _ __ ___   ___  | |_  ___   | |    | |__   __ _| |_ \n");
    printf("  \\ \\/  \\/ // _ \\ |/ __|/ _ \\| '_ ` _ \\ / _ \\ | __|/ _ \\  | |    | '_ \\ / _` | __|\n");
    printf("   \\  /\\  /|  __/ | (__| (_) | | | | | |  __/ | |_| (_) | | |____| | | | (_| | |_ \n");
    printf("    \\/  \\/  \\___|_|\\___|\\___/|_| |_| |_|\\___|  \\__|\\___/   \\_____|_| |_|\\__,_|\\__|\n");
    printf("\n");
}

void end() {
    printf("\n");
    printf(" _______ _                 _           __              _____ _           _   _   _             _ \n");
    printf("|__   __| |               | |         / _|            / ____| |         | | | | (_)           | |\n");
    printf("   | |  | |__   __ _ _ __ | | _____  | |_ ___  _ __  | |    | |__   __ _| |_| |_ _ _ __   __ _| |\n");
    printf("   | |  | '_ \\ / _` | '_ \\| |/ / __| |  _/ _ \\| '__| | |    | '_ \\ / _` | __| __| | '_ \\ / _` | |\n");
    printf("   | |  | | | | (_| | | | |   <\\__ \\ | || (_) | |    | |____| | | | (_| | |_| |_| | | | | (_| |_|\n");
    printf("   |_|  |_| |_|\\__,_|_| |_|_|\\_\\___/ |_| \\___/|_|     \\_____|_| |_|\\__,_|\\__|\\__|_|_| |_|\\__, (_)\n");
    printf("                                                                                          __/ |  \n");
    printf("                                                                                         |___/   \n");
    printf("\n");
}

void *write_function(void *fd)
{
    char message_buffer[1024], local_message[900];
    int *socket_fd = fd;
    pthread_t id = pthread_self();

    FILE *file = fopen(user_file_name, "r");
    
    while(true) {
        printf("Enter your Username: ");
        memset(username, 0, sizeof(username));
        fgets(username, 100, stdin);
        username[strcspn(username, "\n")] = 0;
        if (check_already_exists(file) == false)
            break;
    }

    if (send(*socket_fd, username, strlen(username), 0) < 0)
    {
        printf("Couldn't send message\n");
        return 0;
    }

    while (1)
    {
        fgets(local_message, 900, stdin);
        if (strstr(local_message, "exit"))
        {
            remove_online_user();
            if (send(*socket_fd, local_message, strlen(local_message), 0) < 0)
            {
                printf("Couldn't send message\n");
                return 0;
            }
            break;
        }

        memset(message_buffer, 0, sizeof(message_buffer));
        strcat(message_buffer, username);
        strcat(message_buffer, ": ");
        strcat(message_buffer, local_message);

        if (send(*socket_fd, message_buffer, strlen(message_buffer), 0) < 0)
        {
            printf("Couldn't send message\n");
            return 0;
        }
    }
    close(*socket_fd);
    pthread_exit(&thread_write);
}

void *read_function(void *fd)
{
    char message_buffer[1024];
    int *socket_fd = fd;

    memset(message_buffer, 0, sizeof(message_buffer));

    while (recv(*socket_fd, message_buffer, 256, 0) > 0)
    {
        printf("%s", message_buffer);
        memset(message_buffer, 0, sizeof(message_buffer));
    }
    close(*socket_fd);
    pthread_exit(&thread_read);
}

int main()
{
    struct sockaddr_in server;
    int socket_file_descriptor = 0, connection;
    char message_buffer[1024] = "";

    if ((socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Couldn't create Socket\n");
        return 0;
    }

    memset(&server, '0', sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) < 0)
    {
        printf("Could not Connect to Localhost\n");
        return 0;
    }

    if ((connection = connect(socket_file_descriptor, (struct sockaddr *)&server, sizeof(server))) < 0)
    {
        printf("Couldn't connect to Server\n");
        return 0;
    }

    init();
    // user_setup();

    // Todo: Add check for whether users exist or not
    // Todo: Add signal handler for ctrl + c

    pthread_create(&thread_write, NULL, write_function, &socket_file_descriptor);
    pthread_create(&thread_read, NULL, read_function, &socket_file_descriptor);
    pthread_join(thread_write, NULL);
    pthread_join(thread_read, NULL);
    
    end();

    pthread_cancel(thread_read);

    return 0;
}