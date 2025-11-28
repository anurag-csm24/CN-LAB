// Simple TCP server: receives data and prints total bytes received.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sockfd, connfd;
    char buffer[1024];
    long long total = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv = {0};
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(5001);

    bind(sockfd, (struct sockaddr*)&serv, sizeof(serv));
    listen(sockfd, 1);

    printf("TCP Server running on port 5001...\n");

    connfd = accept(sockfd, NULL, NULL);

    while (1) {
        int n = recv(connfd, buffer, sizeof(buffer), 0);
        if (n <= 0) break;
        total += n;
    }

    printf("Total bytes received: %lld\n", total);

    close(connfd);
    close(sockfd);
    return 0;
}

