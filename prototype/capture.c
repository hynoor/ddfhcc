#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip_icmp.h>  // ICMP header declaraiton
#include <netinet/udp.h>      // UDP header declaration
#include <netinet/tcp.h>      // TCP header declaration
#include <netinet/ip.h>       // IP header declaration
#include <netinet/if_ether.h> // ETH_P_ALL
#include <net/ethernet.h>     // Ethernet header 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_PACK_SIZE 65536

void ProcessPacket(unsigned char* , int);
void print_ip_header(unsigned char* , int);
void print_tcp_packet(unsigned char* , int);
void print_udp_packet(unsigned char* , int);
void print_icmp_packet(unsigned char* , int);
void PrintData(unsigned char* , int);

FILE *logfile;
struct sockaddr_in source, dest;
int tcp=0, udp=0, icmp=0, others=0, igmp=0, total=0, i, j;

int main()
{
    int saddr_size, data_size;
    struct sockaddr saddr;

    unsigned char *buffer = (unsigned char *) malloc(MAX_PACK_SIZE);

    logfile=fopen("log.txt", "w");
    if(logfile == NULL)
    {
        printf("Unable to create log.txt file. \n");
    }
    printf("Starting... \n");

    int sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if(sock_raw < 0) 
    {
        // print the error with proper message
        perror("Socket Error");
        return 1;
    }
    while (1)
    {
        saddr_size = sizeof saddr;
        // receive a packet
        data_size = recvfrom(sock_raw, buffer, MAX_PACK_SIZE, 0, &saddr, (socklen_t*)&saddr_size);
        if(data_size < 0)
        {
            printf("Recvfrom error, failed to get packets\n");
            return 1;
        }
        // now process the packet
        ProcessPacket(buffer, data_size);
    }
    close(sock_raw);
    printf("Finished");
    return 0;
}

void ProcessPacket(unsigned char* buffer, int size)
{
    // get the ip header part of thi packet, excluding the ethernet hearder
    struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    ++total;
    switch (iph->protocol) // check the protocol 
    {
        case 1: // ICMP 
            ++icmp;
            //print_icmp_packet(buffer, size);
            break;

        case 2: // IGMP 
            ++igmp;
            break;

        case 6: //TCP Protocol
            ++tcp;
            print_tcp_packet(buffer, size);
            break;

        case 17: //UDP
            ++udp;
            //print_udp_packet(buffer, size);
            break;

        default: // the other protocols
            ++others;
            break;
    }
    printf(" TCP : %d - UDP : %d - ICMP : %d - IGMP : %d - others : %d | Totoal : %d\r", tcp, udp, icmp, igmp, others, total);
}

void print_ethernet_header(unsigned char* buffer, int size)
{
    struct ethhdr *eth = (struct ethhdr *)buffer;

    fprintf(logfile, "\n");
    fprintf(logfile, "Ethernet Header\n");
    fprintf(logfile, "   |-Destination MAC Address   : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0], eth->h_dest[1], eth->h_dest[2], eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
    fprintf(logfile, "   |-Source MAC Address        : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0], eth->h_source[1], eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5]);
    fprintf(logfile, "   |-Protocol                  : %u \n", (unsigned short)eth->h_proto);
}

void print_ip_header(unsigned char* buffer, int size)
{
    print_ethernet_header(buffer, size);
    unsigned short iphdrlen; struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr)); iphdrlen = iph->ihl * 4; memset(&source, 0, sizeof(struct sockaddr_in));
    source.sin_addr.s_addr = iph->saddr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    fprintf(logfile, "\n");
    fprintf(logfile, "IP Hearder\n");
    fprintf(logfile, "   |-IP Version      : %d\n", (unsigned int)iph->version);
    fprintf(logfile, "   |-Header Length   : %d DWORDS or %d Bytes\n", (unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);
    fprintf(logfile, "   |-Type Of Service : %d\n", (unsigned int)iph->tos);
    fprintf(logfile, "   |-IP Total Length : %d Bytes(Size of Packet)\n", ntohs(iph->tot_len));
    fprintf(logfile, "   |-Identification  : %d\n", ntohs(iph->id));
    fprintf(logfile, "   |-TTL       : %d\n", (unsigned int)iph->ttl);
    fprintf(logfile, "   |-Protocol  : %d\n", (unsigned int)iph->protocol);
    fprintf(logfile, "   |-Checksum  : %d\n", ntohs(iph->check));
    fprintf(logfile, "   |-Source IP       : %s\n", inet_ntoa(source.sin_addr));
    fprintf(logfile, "   |-Destination IP  : %s\n", inet_ntoa(dest.sin_addr));
}

void print_tcp_packet(unsigned char* buffer, int size)
{
    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    iphdrlen = iph->ihl * 4;

    struct tcphdr *tcph=(struct tcphdr*)(buffer + iphdrlen +sizeof(struct ethhdr));

    int header_size = sizeof(struct ethhdr) + iphdrlen + tcph->doff * 4;

    fprintf(logfile, "\n\n********************** TCP Packet *********************\n");

    print_ip_header(buffer, size);
    fprintf(logfile, "\n");
    fprintf(logfile, "TCP Header\n");
    fprintf(logfile, "   |-Source Port          : %u\n", ntohs(tcph->source));
    fprintf(logfile, "   |-Destination Port     : %u\n", ntohs(tcph->dest));
    fprintf(logfile, "   |-Sequence Number      : %u\n", ntohs(tcph->seq));
    fprintf(logfile, "   |-Acknowledge Number   : %u\n", ntohs(tcph->ack_seq));
    fprintf(logfile, "   |-Header Length        : %d DWORDS or %d BYTES\n", (unsigned int)tcph->doff, (unsigned int)tcph->doff * 4);
    fprintf(logfile, "   |-Urgent Flag          : %d\n", (unsigned int)tcph->urg);
    fprintf(logfile, "   |-Acknowledgement Flag : %d\n", (unsigned int)tcph->ack);
    fprintf(logfile, "   |-Push Flag            : %d\n", (unsigned int)tcph->psh);
    fprintf(logfile, "   |-Reset Flag           : %d\n", (unsigned int)tcph->rst);
    fprintf(logfile, "   |-Synchronise Flag     : %d\n", (unsigned int)tcph->syn);
    fprintf(logfile, "   |-Finish Flag          : %d\n", (unsigned int)tcph->fin);
    fprintf(logfile, "   |-Window               : %d\n", ntohs(tcph->window));
    fprintf(logfile, "   |-Checksum             : %d\n", ntohs(tcph->check));
    fprintf(logfile, "   |-Urgent Pointer       : %d\n", tcph->urg_ptr);
    fprintf(logfile, "\n");
    fprintf(logfile, "            DATA Dump                ");
    fprintf(logfile, "\n");

    fprintf(logfile, "IP Header\n"); 
    PrintData(buffer, iphdrlen);

    fprintf(logfile, "TCP Header\n"); 
    PrintData(buffer+iphdrlen, tcph->doff*4);

    fprintf(logfile, "Data Payload\n"); 
    PrintData(buffer+header_size, size-header_size);

    fprintf(logfile, "\n#####################################################\n");

}

void PrintData(unsigned char* data , int size)
{
    int i, j;
    for(i=0; i < size; i++)
    {
        if(i != 0 && i%16 == 0) // if one line of hex printing is complete...
        {
            fprintf(logfile, "    |    ");
            for(j=i-16; j<i; j++)
            {
                if(data[j]>=32 && data[j]<=128)
                    fprintf(logfile, "%c", (unsigned char)data[j]);
                else
                    fprintf(logfile, "."); // unprintable char
            } // end for
            fprintf(logfile, "\n"); 
        } // end if
        if(i%16 == 0)
            fprintf(logfile, "    ");
        else
            fprintf(logfile, " %02X", (unsigned int)data[i]);
        if(i==size-1)
        {
            for(j=0; j<15-i%16;j++)
            {
                fprintf(logfile, "    ");
            }
            fprintf(logfile, "        ");

            for(j=i-i%16; j<=1; j++)
            {
                if(data[j]>=32 && data[j]<=128)
                    fprintf(logfile, "%c", (unsigned char)data[j]);
                else
                    fprintf(logfile, ".");
            }
            fprintf(logfile, "\n");
        } // end if
    } // end for
}



