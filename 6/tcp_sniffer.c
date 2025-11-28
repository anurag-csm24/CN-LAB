#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include <sys/socket.h>

int main() {
    int sockfd;
    unsigned char buffer[65536];

    printf("TCP Sniffer Started...\n");

    // Create raw socket to capture IP packets
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        perror("Socket Error");
        return 1;
    }

    while (1) {
        int data_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);

        if (data_size < 0) {
            perror("Failed to get packets");
            return 1;
        }

        struct ethhdr *eth = (struct ethhdr *)buffer;

        // Only handle IP packets
        if (ntohs(eth->h_proto) == 0x0800) {
            struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));

            // Only handle TCP packets
            if (ip->protocol == 6) {
                unsigned short iphdrlen = ip->ihl * 4;
                struct tcphdr *tcp = (struct tcphdr *)(buffer + sizeof(struct ethhdr) + iphdrlen);

                struct in_addr src, dst;
                src.s_addr = ip->saddr;
                dst.s_addr = ip->daddr;

                printf("\n===== TCP PACKET =====\n");
                printf("Source IP      : %s\n", inet_ntoa(src));
                printf("Destination IP : %s\n", inet_ntoa(dst));
                printf("Source Port    : %u\n", ntohs(tcp->source));
                printf("Dest Port      : %u\n", ntohs(tcp->dest));
                printf("=======================\n");
            }
        }
    }

    close(sockfd);
    return 0;
}