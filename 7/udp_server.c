// udp_server.c – Continuous UDP Scientific Calculator Server
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>
#include <ctype.h>

void trim(char *s) {
    int i, j = 0;
    char temp[256];
    for (i = 0; s[i]; i++)
        if (!isspace(s[i])) temp[j++] = s[i];
    temp[j] = '\0';
    strcpy(s, temp);
}

double evaluate(char *expr) {
    double a, b;

    if (strncmp(expr, "sin(", 4) == 0) { sscanf(expr, "sin(%lf)", &a); return sin(a*M_PI/180); }
    if (strncmp(expr, "cos(", 4) == 0) { sscanf(expr, "cos(%lf)", &a); return cos(a*M_PI/180); }
    if (strncmp(expr, "tan(", 4) == 0) { sscanf(expr, "tan(%lf)", &a); return tan(a*M_PI/180); }
    if (strncmp(expr, "sqrt(", 5) == 0){ sscanf(expr, "sqrt(%lf)", &a); return sqrt(a); }
    if (strncmp(expr, "inv(", 4) == 0) { sscanf(expr, "inv(%lf)", &a); return 1.0/a; }

    if (sscanf(expr, "%lf+%lf", &a, &b) == 2) return a + b;
    if (sscanf(expr, "%lf-%lf", &a, &b) == 2) return a - b;
    if (sscanf(expr, "%lf*%lf", &a, &b) == 2) return a * b;
    if (sscanf(expr, "%lf/%lf", &a, &b) == 2) return a / b;

    return 0;
}

int main() {
    int sockfd;
    char buffer[256];
    struct sockaddr_in serv, cli;
    socklen_t len = sizeof(cli);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(6000);

    bind(sockfd, (struct sockaddr*)&serv, sizeof(serv));

    printf("UDP Scientific Calculator Server running on port 6000...\n");

    while (1) {
        memset(buffer, 0, sizeof(buffer));

        recvfrom(sockfd, buffer, sizeof(buffer), 0,
                (struct sockaddr*)&cli, &len);

        trim(buffer);

        printf("\n Received expression: %s\n", buffer);

        double ans = evaluate(buffer);

        char reply[100];
        sprintf(reply, "%lf", ans);

        printf("Sending result to client: %s\n", reply);

        sendto(sockfd, reply, strlen(reply), 0,
               (struct sockaddr*)&cli, len);
    }

    close(sockfd);
    return 0;
}

