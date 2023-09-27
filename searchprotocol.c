#include <stdio.h>
#include <pcap.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

void print_packet_info(const struct iphdr *ip_header, const void *transport_header, const u_char *packet) {
    if (ip_header->protocol == IPPROTO_TCP) {
        struct tcphdr *tcp_header = (struct tcphdr *)transport_header;

        printf("Source IP: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->saddr));
        printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->daddr));
        printf("Source Port: %u\n", ntohs(tcp_header->source));
        printf("Destination Port: %u\n", ntohs(tcp_header->dest));
        
        int payload_offset = sizeof(struct tcphdr);
        int payload_length = ntohs(ip_header->tot_len) - ip_header->ihl * 4 - tcp_header->doff * 4;
        
        printf("Payload Length: %d bytes\n", payload_length);
        printf("Payload Data: ");
        for (int i = 0; i < payload_length; i++) {
            printf("%02X ", packet[payload_offset + i]);
        }
        printf("\n");
    } else if (ip_header->protocol == IPPROTO_UDP) {
        struct udphdr *udp_header = (struct udphdr *)transport_header;

        printf("Source IP: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->saddr));
        printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->daddr));
        printf("Source Port: %u\n", ntohs(udp_header->source));
        printf("Destination Port: %u\n", ntohs(udp_header->dest));
        
        int payload_offset = sizeof(struct udphdr);
        int payload_length = ntohs(udp_header->len) - sizeof(struct udphdr);
        
        printf("Payload Length: %d bytes\n", payload_length);
        printf("Payload Data: ");
        for (int i = 0; i < payload_length; i++) {
            printf("%02X ", packet[payload_offset + i]);
        }
        printf("\n");
    } else {
        printf("Protocol: Unknown\n");
    }
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    struct pcap_pkthdr header;
    const u_char *packet;

    // Open the .pcap file for reading
    handle = pcap_open_offline("output.pcap", errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Error opening input file: %s\n", errbuf);
        return 1;
    }

    int protocol_choice;
    printf("Enter the protocol (6 for TCP, 17 for UDP): ");
    scanf("%d", &protocol_choice);

    // Loop through the packets in the .pcap file
    while ((packet = pcap_next(handle, &header)) != NULL) {
        struct iphdr *ip_header = (struct iphdr *)(packet + sizeof(struct ethhdr));
        void *transport_header = (void *)(packet + sizeof(struct ethhdr) + ip_header->ihl * 4);

        if (ip_header->protocol == protocol_choice) {
            print_packet_info(ip_header, transport_header, packet);
            printf("\n");
        }
    }

    // Close the input file handle
    pcap_close(handle);

    return 0;
}

