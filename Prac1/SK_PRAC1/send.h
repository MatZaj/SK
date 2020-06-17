// Mateusz Zając, 298654

#ifndef send_h
#define send_h

u_int16_t compute_icmp_checksum (const void *buff, int length);
void sendPackets(int sockfd, char argum[20], pid_t myPid);

#endif
