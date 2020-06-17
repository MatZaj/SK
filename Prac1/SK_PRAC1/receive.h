// Mateusz Zając, 298654

#ifndef receive_h
#define receive_h

void clearArray(char array[3][20]);
void remDuplicates(char array[3][20]);
void printArray(char array[3][20]);
int receivePackets(int sockfd, pid_t myPid);

#endif
