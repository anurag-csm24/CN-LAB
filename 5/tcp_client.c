// Simple TCP client: sends fixed-size data repeatedly for few seconds.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sockfd;
    char buffer[1024];
    memset(buffer, 'A', sizeof(buffer));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv = {0};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(5001);
    inet_pton(AF_INET, "10.0.0.1", &serv.sin_addr);   // server = h1

    connect(sockfd, (struct sockaddr*)&serv, sizeof(serv));

    printf("Sending data to server...\n");

    for (int i = 0; i < 50000; i++) {   // send 50k packets
        send(sockfd, buffer, sizeof(buffer), 0);
        usleep(1000);                  // slow down (1 ms)
    }

    printf("Client finished.\n");

    close(sockfd);
    return 0;
}

