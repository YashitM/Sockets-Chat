#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

#define true 1
#define false 0

#define user_file_name "users"
#define port 9092

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

    if (strstr(username, ";;;;;"))
    {
        char *pch;
        pch = strtok(username, ";;;;;");
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
        strcpy(username, final_message);
    }

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

int check_choice(int choice) {
    FILE *file = fopen(user_file_name, "r");
    char *line = NULL;
    size_t len = 0;
    if(file) {
        char check_string[10];
        sprintf(check_string, "%d) ", choice);
        while (getline(&line, &len, file) != -1)
        {
            if (strstr(line, check_string))
            {
                fclose(file);
                return true;
            }
        }
        fclose(file);
    }
    return false;
}

void select_chatroom(int socket_fd) {
    printf("Available ChatRooms\n");
    printf("111) Global;\n");
    printf("222) Group Chat;\n");

    if (get_user_count() > 1)
    {
        int c;
        size_t len = 0;
        FILE *file;
        char *line = NULL;
        file = fopen(user_file_name, "r");
        if (file)
        {
            while (getline(&line, &len, file) != -1)
            {
                if (!strstr(line, username))
                {
                    printf("%s", line);
                }
            }
            fclose(file);
        }
    }
    printf("000) Exit;\n");
    int choice;
    while (true)
    {
        printf("Select a Chatroom: ");
        scanf("%d", &choice);
        if (choice == 111 || choice == 000 || choice == 222)
        {
            break;
        }
        if (check_choice(choice) == true)
        {
            printf("You are now chatting with User Number: %d\n", choice);
            break;
        }
    }

    if (choice == 000)
    {
        remove_online_user();
        close(socket_fd);
        exit(0);
    }
    else if(choice == 111) {
        if (strstr(username, ";;;;;"))
        {
            char *pch;
            pch = strtok(username, ";;;;;");
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
            strcpy(username, final_message);
        }
    }
    else if (choice == 222) {
        printf("Enter the Users you want to chat with (eg: 2,3,4): ");
        char group_users[100] = "";
        scanf("%100s", group_users);
        char *tokenizer;
        tokenizer = strtok(group_users, ",");
        char *string_users_array[100];
        memset(string_users_array, 0, sizeof(string_users_array));
        int user_counter = 0;
        while (tokenizer != NULL)
        {
            string_users_array[user_counter++] = tokenizer;
            tokenizer = strtok(NULL, ",");
        }
        int k=0;
        int users_array[100];
        int valid_user_counter = 0;
        for (; k < user_counter; k++) {
            int x;
            sscanf(string_users_array[k], "%d", &x);
            if(check_choice(x) == true) {
                users_array[valid_user_counter++] = x;
            }
        }
        
        if(valid_user_counter > 0) {
            if (strstr(username, ";;;;;"))
            {
                char *pch;
                pch = strtok(username, ";;;;;");
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
                strcpy(username, final_message);
            }
            int x=0;
            for(; x<valid_user_counter; x++) {
                char choice_string[10];
                sprintf(choice_string, "%d", users_array[x]);
                strcat(username, ";;;;;");
                strcat(username, choice_string);
            }
            strcat(username, ";;;;;");
            printf("%s\n", username);
        }
        else {
            printf("No valid user entered\n");
        }
    }
    else
    {
        char choice_string[10];
        if (strstr(username, ";;;;;"))
        {
            char *pch;
            pch = strtok(username, ";;;;;");
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
            strcpy(username, final_message);
        }
        sprintf(choice_string, "%d", choice);
        strcat(username, ";;;;;");
        strcat(username, choice_string);
        strcat(username, ";;;;;");
    }
}

void init(int socket_fd) {
    printf("\n");
    printf("__          __   _                             _            _____ _           _   \n");
    printf("\\ \\        / /  | |                           | |          / ____| |         | |  \n");
    printf(" \\ \\  /\\  / /___| | ___  ___  _ __ ___   ___  | |_  ___   | |    | |__   __ _| |_ \n");
    printf("  \\ \\/  \\/ // _ \\ |/ __|/ _ \\| '_ ` _ \\ / _ \\ | __|/ _ \\  | |    | '_ \\ / _` | __|\n");
    printf("   \\  /\\  /|  __/ | (__| (_) | | | | | |  __/ | |_| (_) | | |____| | | | (_| | |_ \n");
    printf("    \\/  \\/  \\___|_|\\___|\\___/|_| |_| |_|\\___|  \\__|\\___/   \\_____|_| |_|\\__,_|\\__|\n");
    printf("\n");

    FILE *file = fopen(user_file_name, "r");

    while (true)
    {
        printf("Enter your Username: ");
        memset(username, 0, sizeof(username));
        fgets(username, 100, stdin);
        username[strcspn(username, "\n")] = 0;
        if (check_already_exists(file) == false)
        {
            break;
        }
    }
    if (send(socket_fd, username, strlen(username), 0) < 0)
    {
        printf("Couldn't send message\n");
        return;
    }
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
    while(true) {
        select_chatroom(*socket_fd);
        while (true)
        {
            fgets(local_message, 900, stdin);
            if (strstr(local_message, "exithard"))
            {
                remove_online_user();
                if (send(*socket_fd, local_message, strlen(local_message), 0) < 0)
                {
                    printf("Couldn't send message\n");
                    pthread_exit(&thread_write);
                    pthread_exit(&thread_read);
                    return 0;
                }
                close(*socket_fd);
                exit(0);
            }
            else if (strstr(local_message, "exit"))
            {
                break;
            }

            memset(message_buffer, 0, sizeof(message_buffer));
            strcat(message_buffer, username);
            strcat(message_buffer, ": ");
            strcat(message_buffer, local_message);

            if (send(*socket_fd, message_buffer, strlen(message_buffer), 0) < 0)
            {
                printf("Couldn't send message\n");
                pthread_exit(&thread_write);
                pthread_exit(&thread_read);
                return 0;
            }
        }
    }
    pthread_exit(&thread_write);
    pthread_exit(&thread_read);
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
    
    int loop_counter = 0;

    init(socket_file_descriptor);
    while(true) {
        // user_setup();

        // Todo: Add signal handler for ctrl + c

        pthread_create(&thread_write, NULL, write_function, &socket_file_descriptor);
        pthread_create(&thread_read, NULL, read_function, &socket_file_descriptor);
        pthread_join(thread_write, NULL);
        pthread_join(thread_read, NULL);
        loop_counter += 1;
    }

    end();
    return 0;
}