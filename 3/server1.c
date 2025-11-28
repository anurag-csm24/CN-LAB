#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>  // For mkdir() and stat()

#define PORT 12345
#define BUF_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUF_SIZE];
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);
    int file_count = 1;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Setup server address
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(1);
    }

    printf("UDP Server started on port %d\n", PORT);
    printf("Waiting for client messages...\n");

    while (1) {
        memset(buffer, 0, BUF_SIZE);
        recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr*)&client, &client_len);

        // Handle exit command
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Client disconnected.\n");
            break;
        }

        // Handle file transfer
        else if (strncmp(buffer, "sendfile", 8) == 0) {
            char folder[] = "received_files";
            char filename[200];
            struct stat st = {0};

            // Create the folder if it doesn't exist
            if (stat(folder, &st) == -1) {
                if (mkdir(folder, 0700) == 0) {
                    printf("Created folder: %s\n", folder);
                } else {
                    perror("Failed to create folder");
                    continue;
                }
            }

            // Full path
            snprintf(filename, sizeof(filename), "%s/received_%d.txt", folder, file_count++);

            FILE *fp = fopen(filename, "w");
            if (!fp) {
                perror("File open failed");
                continue;
            }

            printf("Receiving file... (waiting for data)\n");

            int file_failed = 0; // flag to detect file open failure on client side

            while (1) {
                memset(buffer, 0, BUF_SIZE);
                recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr*)&client, &client_len);

                // Check if client reported a file open error
                if (strncmp(buffer, "FILE_ERROR", 10) == 0) {
                    printf("Failed to receive file (client could not open it).\n");
                    fclose(fp);
                    remove(filename);
                    file_failed = 1;
                    break; // stop file receiving loop
                }

                // End of file marker
                if (strncmp(buffer, "EOF", 3) == 0)
                    break;

                fputs(buffer, fp);
            }

            if (!file_failed) {
                fclose(fp);
                printf("File saved successfully as '%s'\n", filename);
            }

            printf("You can continue chatting with the client.\n");
        }

        // Normal chat message
        else {
            printf("Client: %s\n", buffer);
            printf("Server: ");
            fgets(buffer, BUF_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline
            sendto(sockfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr*)&client, client_len);
        }
    }

    close(sockfd);
    return 0;
}