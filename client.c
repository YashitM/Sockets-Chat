#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

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
        sprintf(string_for_file, "%d) %s;\n", counter, username);
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

void user_setup() {
    printf("Enter your Username: ");
    fgets(username, 100, stdin);
    username[strcspn(username, "\n")] = 0;
    add_online_user();
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

int main()
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
    server.sin_port = htons(8080);

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

    init();
    user_setup();

    // Todo: Add check for whether users exist or not
    // Todo: Add signal handler for ctrl + c

    while(true) {

        char local_message[1024];

        printf("Enter your message: ");
        fgets(local_message, 900, stdin);
        if (strstr(local_message, "exit"))
        {
            remove_online_user();
            if ((send(socket_file_descriptor, local_message, strlen(local_message), 0)) < 0)
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
        if ((send(socket_file_descriptor, message_buffer, strlen(message_buffer), 0)) < 0)
        {
            printf("Couldn't send message\n");
            return 0;
        }
    }
    close(socket_file_descriptor);
    end();
    return 0;
}