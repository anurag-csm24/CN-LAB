#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUF_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUF_SIZE];
    char filename[256];
    struct sockaddr_in server;
    socklen_t server_len = sizeof(server);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Server address setup
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("10.0.0.1");

    printf("Connected to UDP Chat/File Server\n");
    printf("Type messages to chat.\n");
    printf("Type 'sendfile' to send a file.\n");
    printf("Type 'exit' to quit.\n");

    while (1) {
        printf("You: ");
        fgets(buffer, BUF_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        sendto(sockfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr*)&server, server_len);

        if (strncmp(buffer, "exit", 4) == 0)
            break;

        // File transfer
        if (strncmp(buffer, "sendfile", 8) == 0) {
            printf("Enter filename to send: ");
            fgets(filename, sizeof(filename), stdin);
            filename[strcspn(filename, "\n")] = '\0';

            FILE *fp = fopen(filename, "r");
            if (!fp) {
                perror("File not found");
                // Notify server that file could not be opened
                strcpy(buffer, "FILE_ERROR");
                sendto(sockfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr*)&server, server_len);
                continue;
            }

            printf("Sending file '%s'...\n", filename);
            while (fgets(buffer, BUF_SIZE, fp) != NULL) {
                sendto(sockfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr*)&server, server_len);
                usleep(100000); // delay to avoid flooding
            }
            sendto(sockfd, "EOF", 3, 0, (struct sockaddr*)&server, server_len);
            fclose(fp);
            printf("File '%s' sent successfully.\n", filename);
        } else {
            memset(buffer, 0, BUF_SIZE);
            recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr*)&server, &server_len);
            printf("Server: %s\n", buffer);
        }
    }

    close(sockfd);
    return 0;
}