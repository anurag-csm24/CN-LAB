// file_client.c – TCP File Upload/Download Client
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFFER 1024

int main() {
    int sockfd;
    struct sockaddr_in serv;
    char buffer[BUFFER];
    char filename[100];
    FILE *fp;

    serv.sin_family = AF_INET;
    serv.sin_port = htons(9000);
    inet_pton(AF_INET, "10.0.0.1", &serv.sin_addr);  // server = h1 IP

    // ==================================
    //       DOWNLOAD FROM SERVER
    // ==================================

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    connect(sockfd, (struct sockaddr*)&serv, sizeof(serv));

    send(sockfd, "DOWNLOAD", 10, 0);

    printf("Enter filename to download from server: ");
    scanf("%s", filename);

    send(sockfd, filename, strlen(filename)+1, 0);

    fp = fopen(filename, "wb");

    time_t start = time(NULL);

    int n;
    while ((n = recv(sockfd, buffer, BUFFER, 0)) > 0) {
        fwrite(buffer, 1, n, fp);
        if (n < BUFFER) break;
    }
    fclose(fp);

    time_t end = time(NULL);
    printf("Download completed. Time taken (client): %ld seconds\n", end - start);

    close(sockfd);

    // ==================================
    //       UPLOAD TO SERVER
    // ==================================

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    connect(sockfd, (struct sockaddr*)&serv, sizeof(serv));

    send(sockfd, "UPLOAD", 10, 0);

    printf("Enter filename to upload to server: ");
    scanf("%s", filename);

    send(sockfd, filename, strlen(filename)+1, 0);

    fp = fopen(filename, "rb");

    start = time(NULL);

    while ((n = fread(buffer, 1, BUFFER, fp)) > 0) {
        send(sockfd, buffer, n, 0);
    }
    fclose(fp);

    end = time(NULL);
    printf("Upload completed. Time taken (client): %ld seconds\n", end - start);

    close(sockfd);

    return 0;
}

