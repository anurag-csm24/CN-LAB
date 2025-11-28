// chat_client.c – Basic TCP Chat Client
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int sockfd;

// Thread to continuously receive messages from server
void *receive_handler(void *arg) {
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int n = recv(sockfd, buffer, sizeof(buffer), 0);
        if (n <= 0) break;
        printf("%s", buffer);
        fflush(stdout);
    }

    return NULL;
}

int main() {
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(7000);
    inet_pton(AF_INET, "10.0.0.1", &server.sin_addr);

    connect(sockfd, (struct sockaddr*)&server, sizeof(server));

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_handler, NULL);

    printf("Connected to chat server.\n");

    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sockfd, buffer, strlen(buffer), 0);
    }

    close(sockfd);
    return 0;
}

