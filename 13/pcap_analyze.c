#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    // Open pcap file
    handle = pcap_open_offline("root_capture.pcap", errbuf);
    if (!handle) {
        printf("Error opening pcap file: %s\n", errbuf);
        return 1;
    }

    struct pcap_pkthdr *header;
    const u_char *data;
    int count = 0;

    printf("Time Diagram (Packet vs Timestamp)\n");
    printf("----------------------------------\n");

    while (pcap_next_ex(handle, &header, &data) >= 0) {
        count++;

        // Extract L2 Ethernet header
        struct ether_header *eth = (struct ether_header *)data;

        // Extract L3
        struct iphdr *ip = (struct iphdr *)(data + sizeof(struct ether_header));

        // Convert TS to seconds
        double t = header->ts.tv_sec + header->ts.tv_usec / 1000000.0;

        printf("Packet %d at %.6f sec | ", count, t);

        // Determine protocol
        if (ntohs(eth->ether_type) == ETHERTYPE_IP) {
            if (ip->protocol == IPPROTO_ICMP)
                printf("ICMP (Ping)\n");
            else if (ip->protocol == IPPROTO_TCP)
                printf("TCP\n");
            else if (ip->protocol == IPPROTO_UDP)
                printf("UDP\n");
            else
                printf("Other IP Protocol\n");
        } else if (ntohs(eth->ether_type) == ETHERTYPE_ARP) {
            printf("ARP\n");
        } else {
            printf("Other L2\n");
        }
    }

    pcap_close(handle);
    return 0;
}