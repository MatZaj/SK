// Mateusz Zając, 298654

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <strings.h>
#include <unistd.h>
#include "receive.h"


void clearArray(char array[3][20]) {
    memset(array, 0, 60);
}

void remDuplicates(char array[3][20]) {
    if(strcmp(array[0], array[1]) == 0) strcpy(array[1], "");
    if(strcmp(array[0], array[2]) == 0) strcpy(array[2], "");
    if(strcmp(array[1], array[2]) == 0) strcpy(array[2], "");
}

void printArray(char array[3][20]) {
    remDuplicates(array);
    for(int i = 0; i<3; i++) {
        printf("%s ", array[i]);
    }
}

int receivePackets(int sockfd, pid_t myPid) {

    char IPs[3][20];
    strcpy(IPs[0], "");
    strcpy(IPs[1], "");
    strcpy(IPs[2], "");


    int exit_code = 11; // do zakończenia programu
    int localCount = 0; //liczenie pakietow
    float sum = 0.0; //suma czasow
    clearArray(IPs);

    while(1) {
        // ---- Teraz muszę napisać sockaddr_in, żeby odebrać pakiet jakoś
        struct sockaddr_in  sender;
        socklen_t           sender_len = sizeof(sender);
        u_int8_t            buffer [IP_MAXPACKET];
        ssize_t             packet_len;
        fd_set descriptors;
        FD_ZERO(&descriptors);
        FD_SET(sockfd, &descriptors);
        struct timeval tv; tv.tv_sec = 1; tv.tv_usec = 0;
        int ready = select(sockfd+1, &descriptors, NULL, NULL, &tv);
        switch(ready) {
            case -1:
                fprintf(stderr, "Select error. %s\n",
                    strerror(errno));
                exit(EXIT_FAILURE);
                break;
            case 0:
                if(localCount != 0) // jesli przyszedl jakis pakiet wczesniej
                    sum = -1.0; // to zaznaczam to ujemną sumą
                localCount=3;
                break;
            default:
                packet_len = recvfrom (sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender, &sender_len);
                if(packet_len<0) {
                    fprintf(stderr, "Recvfrom error: %s\n",
                    strerror(errno));
                    exit(EXIT_FAILURE);
                }

                if(tv.tv_usec>0)
                    sum += 1000000-tv.tv_usec;

                char sender_ip_str[20];
                inet_ntop(AF_INET, &(sender.sin_addr),
                sender_ip_str, sizeof(sender_ip_str));

                struct ip* ip_header = (struct ip*) buffer;
                u_int8_t* icmp_packet = buffer + 4*ip_header->ip_hl;
                struct icmp* icmp_header = (struct icmp*) icmp_packet;

                exit_code = icmp_header->icmp_type;
                if(exit_code == 11) {   // jeśli to pakiet od ktoregos z routerow po drodze
                    struct ip* second_ip_h = (struct ip*) ((void*)icmp_header + (ssize_t)sizeof(struct icmphdr));
                    ssize_t second_ip_h_len = 4* second_ip_h->ip_hl;
                    icmp_header = (struct icmp*) ((void*) second_ip_h + second_ip_h_len);
                }
                strcpy(IPs[localCount], sender_ip_str); // dodaję ip do listy, aby potem wypisać
                if(exit_code == 3) {
                    localCount = 3;
                    continue;
                }
                if((icmp_header->icmp_hun.ih_idseq.icd_id) != myPid) { //jak złapałem nie taki pakiet jak chcę, to lecę dalej po pakietach
                    continue;
                }
                localCount++;
        }
        if(localCount == 3) {
            if(sum == -1.0) {// czyli jesli nie nadeszla czesc pakietow
                printArray(IPs);
                printf(" ??? ms \n");
            }
            else if(sum == 0) { //nie dotarły żadne pakiety
                printf("*\n");
            }
            else {
                sum /= 3000.0; // zamiana na milisekundy i srednia z trzech
                printArray(IPs);
                printf(" %i ms \n",(int)sum);
            }
            break;
        }
    }
    if(exit_code == 0) return 0; // traceroute dotarl do celu
    if(exit_code == 3) return 3; // traceroute nie dojdzie do celu (unreachable)
    return 11;
}
