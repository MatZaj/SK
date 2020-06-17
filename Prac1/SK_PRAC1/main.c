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
#include "send.h"

int main(int argc, char *argv[])
{
    if(argc != 2) {
        printf("Nieprawidłowa ilość argumentów. \n");
        exit(EXIT_FAILURE);
    }

    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1){ // Utworzenie nowego gniazda
        fprintf(stderr, "Socket Error.: %s\n",
                    strerror(errno));
        exit(EXIT_FAILURE);
    }

    pid_t myPid = getpid(); // Pobranie Pid do pakietu. W celu identyfikacji pakietów.
    for(int ttl = 1; ttl<31; ttl++) { // Pętla dla TTL [1;30]
        setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
        printf("%d.  ", ttl);
        /// wysyłanie
        sendPackets(sockfd, argv[1], myPid);
        ///odbieranie
        int back = receivePackets(sockfd, myPid);
        if(back == 0) break;
        if(back == 3) break;
    }

}

