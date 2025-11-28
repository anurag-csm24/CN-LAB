// udp_client.c – Continuous UDP Client Until User Types "exit"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sockfd;
    char expr[256], result[256];

    struct sockaddr_in serv;
    socklen_t slen = sizeof(serv);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    serv.sin_family = AF_INET;
    serv.sin_port = htons(6000);
    inet_pton(AF_INET, "10.0.0.1", &serv.sin_addr);

    while (1) {
        printf("\nEnter expression (or type 'exit' to quit): ");
        fgets(expr, sizeof(expr), stdin);
        expr[strcspn(expr, "\n")] = 0;

        if (strcmp(expr, "exit") == 0) {
            printf("Client stopped.\n");
            break;
        }

        sendto(sockfd, expr, strlen(expr), 0,
               (struct sockaddr*)&serv, slen);

        memset(result, 0, sizeof(result));

        recvfrom(sockfd, result, sizeof(result), 0,
                 (struct sockaddr*)&serv, &slen);

        printf("➡ Result from server: %s\n", result);
    }

    close(sockfd);
    return 0;
}

