// send_icmp_timestamp.c – RAW ICMP Timestamp packet

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b, result;
    unsigned int sum = 0;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main() {
    int sock;
    char buffer[1024];

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("socket error");
        return -1;
    }

    struct icmphdr *icmp = (struct icmphdr*) buffer;
    memset(buffer, 0, sizeof(buffer));

    // Build ICMP Timestamp Request
    icmp->type = 13;    // Timestamp request
    icmp->code = 0;
    icmp->un.echo.id = htons(1);
    icmp->un.echo.sequence = htons(1);

    // ICMP needs checksum
    icmp->checksum = 0;
    icmp->checksum = checksum(buffer, sizeof(struct icmphdr));

    // Destination IP
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    inet_pton(AF_INET, "10.0.0.2", &target.sin_addr);  // target host

    if (sendto(sock, buffer, sizeof(struct icmphdr), 0,
               (struct sockaddr*)&target, sizeof(target)) < 0) {
        perror("sendto");
    } else {
        printf("ICMP Timestamp Request sent.\n");
    }

    close(sock);
    return 0;
}

