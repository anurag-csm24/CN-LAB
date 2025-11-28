// file_server.c – TCP File Upload/Download Server
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFFER 1024

int main() {
    int sockfd, newfd;
    struct sockaddr_in serv, cli;
    socklen_t cli_len = sizeof(cli);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(9000);

    bind(sockfd, (struct sockaddr*)&serv, sizeof(serv));
    listen(sockfd, 5);

    printf("Server running on port 9000...\n");

    while ((newfd = accept(sockfd, (struct sockaddr*)&cli, &cli_len))) {

        char option[10], filename[100];
        char buffer[BUFFER];
        FILE *fp;

        // Receive option: UPLOAD or DOWNLOAD
        recv(newfd, option, sizeof(option), 0);

        // ===========================
        //        FILE DOWNLOAD
        // ===========================
        if (strcmp(option, "DOWNLOAD") == 0) {

            recv(newfd, filename, sizeof(filename), 0);
            printf("Client requested download: %s\n", filename);

            fp = fopen(filename, "rb");
            if (!fp) { printf("File not found.\n"); close(newfd); continue; }

            time_t start = time(NULL);

            int n;
            while ((n = fread(buffer, 1, BUFFER, fp)) > 0) {
                send(newfd, buffer, n, 0);
            }
            fclose(fp);

            time_t end = time(NULL);
            printf("Download finished. Time taken (server): %ld seconds\n",
                   end - start);
        }

        // ===========================
        //         FILE UPLOAD
        // ===========================
        else if (strcmp(option, "UPLOAD") == 0) {

            recv(newfd, filename, sizeof(filename), 0);
            printf("Client uploading file: %s\n", filename);

            fp = fopen(filename, "wb");
            if (!fp) { perror("File error"); close(newfd); continue; }

            time_t start = time(NULL);

            int n;
            while ((n = recv(newfd, buffer, BUFFER, 0)) > 0) {
                fwrite(buffer, 1, n, fp);
                if (n < BUFFER) break;
            }
            fclose(fp);

            time_t end = time(NULL);
            printf("Upload finished. Time taken (server): %ld seconds\n",
                   end - start);
        }

        close(newfd);
    }

    close(sockfd);
    return 0;
}

