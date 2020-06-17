/*
	Mateusz Zając, 298654
*/

#ifndef send_h
#define send_h

#include "utils.h"

// Wysylam wszystkie swoje wpisy z tablicy routingu do sieci, ktorej index w mojej tablicy routingu to "index" gniazdem sockfd
int sendPacket(int sockfd, int index);

// Wysylam do wszystkich swoich sasiadow moja tablice routingu gniazdem "sockfd"
int sendTable(int sockfd);

#endif
