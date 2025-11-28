// send_tcp_raw.c – RAW TCP packet with custom payload (roll number)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

#define PCKT_LEN 8192

unsigned short csum(unsigned short *ptr, int nbytes) {
    long sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1)
        sum += *(unsigned char*)ptr;

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

int main() {
    char packet[PCKT_LEN];

    // Create raw socket
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) {
        perror("Socket error");
        return -1;
    }

    // Zero out packet buffer
    memset(packet, 0, PCKT_LEN);

    // Pointers to IP and TCP header
    struct iphdr *ip = (struct iphdr *) packet;
    struct tcphdr *tcp = (struct tcphdr *) (packet + sizeof(struct iphdr));

    // Payload = roll number
    char *payload = packet + sizeof(struct iphdr) + sizeof(struct tcphdr);
    strcpy(payload, "ROLLNO_23XYZ1234");    // <--- CHANGE THIS TO YOUR ROLL NUMBER
    int payload_size = strlen(payload);

    // Server address
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(80);           // arbitrary port
    inet_pton(AF_INET, "10.0.0.2", &dest.sin_addr); // target = h2

    // IP HEADER
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr) + payload_size);
    ip->id = htons(54321);
    ip->ttl = 64;
    ip->protocol = IPPROTO_TCP;
    ip->saddr = inet_addr("10.0.0.1");    // spoofed source (h1)
    ip->daddr = dest.sin_addr.s_addr;
    ip->check = csum((unsigned short*)packet, sizeof(struct iphdr));

    // TCP HEADER
    tcp->source = htons(12345);
    tcp->dest = htons(80);
    tcp->seq = htonl(1);
    tcp->ack_seq = 0;
    tcp->doff = 5;
    tcp->syn = 1;
    tcp->window = htons(5840);

    tcp->check = 0; // Kernel will compute

    // Send the packet
    if (sendto(sock, packet, ntohs(ip->tot_len), 0,
               (struct sockaddr*)&dest, sizeof(dest)) < 0) {
        perror("sendto");
    } else {
        printf("RAW TCP packet sent with roll number in payload.\n");
    }

    close(sock);
    return 0;
}

