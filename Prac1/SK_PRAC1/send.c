// Mateusz Zając, 298654

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <strings.h>
#include "send.h"
#include <unistd.h>
#include <assert.h>

u_int16_t compute_icmp_checksum (const void *buff, int length)
{
	u_int32_t sum;
	const u_int16_t* ptr = buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}
void sendPackets(int sockfd, char argum[20], pid_t myPid) {
    int errorCatcher;

    struct icmp header;
    header.icmp_type = ICMP_ECHO;
    header.icmp_code = 0;
    header.icmp_hun.ih_idseq.icd_id = myPid;
    header.icmp_hun.ih_idseq.icd_seq = 0;
    header.icmp_cksum = 0;
    header.icmp_cksum = compute_icmp_checksum(
    (u_int16_t*)&header, sizeof(header));

    struct sockaddr_in recipient;
    bzero(&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;
    if((errorCatcher = inet_pton(AF_INET, argum, &recipient.sin_addr)) == 0) {
        printf("IP Conversion error. \n");
                exit(EXIT_FAILURE);
    }

    for(int i = 0; i<3; i++) { // Wysyłam trzy pakiety dla TTL z pętli
            if((errorCatcher = sendto(
                sockfd,
                &header,
                sizeof(header),
                0,
                (struct sockaddr*)&recipient,
                sizeof(recipient)
            )) == SO_ERROR) {
                fprintf(stderr, "Send Error. %s\n",
                    strerror(errno));
                exit(EXIT_FAILURE);
            }
    }
}
