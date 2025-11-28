#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char*)buf;
    
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main() {
    int sockfd;
    struct sockaddr_in addr;
    char packet[64];

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    memset(packet, 0, sizeof(packet));

    struct icmphdr *icmp = (struct icmphdr *) packet;
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = getpid() & 0xFFFF;
    icmp->un.echo.sequence = 1;
    icmp->checksum = checksum(packet, sizeof(packet));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("10.0.0.2");   // h2 IP

    if (sendto(sockfd, packet, sizeof(packet), 0,
               (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("sendto");
        return 1;
    }

    printf("ICMP Echo Request sent.\n");

    close(sockfd);
    return 0;
}
