// chat_server.c – Threaded TCP Chat Server with Logging
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

int clients[MAX_CLIENTS];
pthread_mutex_t lock;

FILE *logfile;

// Broadcast message to all clients
void broadcast_message(char *msg, int sender_fd) {
    pthread_mutex_lock(&lock);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != 0 && clients[i] != sender_fd) {
            send(clients[i], msg, strlen(msg), 0);
        }
    }
    pthread_mutex_unlock(&lock);
}

// Add timestamp and save message to log.txt
void save_to_log(char *msg) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(logfile, "[%02d:%02d:%02d] %s",
            t->tm_hour, t->tm_min, t->tm_sec, msg);
    fflush(logfile);
}

// Thread function to handle a client
void *client_handler(void *arg) {
    int client_fd = *(int*)arg;
    char buffer[BUFFER_SIZE];
    int index = -1;

    // find slot for client
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == 0) {
            clients[i] = client_fd;
            index = i;
            break;
        }
    }

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int n = recv(client_fd, buffer, sizeof(buffer), 0);

        if (n <= 0) {
            printf("Client disconnected.\n");
            pthread_mutex_lock(&lock);
            clients[index] = 0;
            pthread_mutex_unlock(&lock);
            close(client_fd);
            break;
        }

        printf("Client %d: %s", client_fd, buffer);

        save_to_log(buffer);      // write to log
        broadcast_message(buffer, client_fd);  // send to all clients
    }

    return NULL;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server, client;
    int c = sizeof(struct sockaddr_in);

    pthread_mutex_init(&lock, NULL);

    logfile = fopen("log.txt", "a");
    if (!logfile) {
        perror("Log open failed");
        return 1;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(7000);

    bind(server_fd, (struct sockaddr*)&server, sizeof(server));
    listen(server_fd, 10);

    printf("Chat Server running on port 7000...\n");

    while ((client_fd = accept(server_fd, (struct sockaddr *)&client, (socklen_t*)&c))) {
        printf("Client connected. FD = %d\n", client_fd);

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, client_handler, (void*)&client_fd) < 0) {
            perror("Thread error");
            return 1;
        }
    }

    fclose(logfile);
    close(server_fd);
    return 0;
}

