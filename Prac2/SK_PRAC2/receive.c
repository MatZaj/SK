/*
	Mateusz Zając, 298654
*/

#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "receive.h"

// Funkcja odbierajaca pakiety z gniazda
int receivePackets(int sockfd) {
    while(1) {
        // Pola, ktore zapelnie odebranymi danymi
		struct sockaddr_in  sender;
		socklen_t           sender_len = sizeof(sender);
		uint8_t            buffer[9];

		ssize_t             datagram_len;
        fd_set descriptors;
        FD_ZERO(&descriptors);
        FD_SET(sockfd, &descriptors);

        struct timeval tv; tv.tv_sec = 5; tv.tv_usec = 0;          // Czas czekania na pakiet: 5 sekund

        int ready = select(sockfd+1, &descriptors, NULL, NULL, &tv);

        // Jesli wystapil jakis blad przy odbieraniu, wychodze z funkcji
        if(ready < 0) {
            fprintf(stderr, "SELECT ERROR. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Jesli nie mam juz pakietow w gniezdzie po uplywie czasu, wychodze. Widocznie nie ma juz nic do obejrzenia.
        if(ready == 0) {
            break;
        }

        // Znalazly sie jakies pakiety w gniezdzie. Zapisuje je i odpowiednio przetwarzam dalej funkcja "insertIntoTable"
        if(ready > 0) {
            datagram_len = recvfrom (sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender, &sender_len);

            // Jesli pobralem pakiet, ktory jest niepoprawny, wychodze z bledem. Odebralem jakis zly pakiet.
            if (datagram_len > 9) {
                printf("ERROR_SIZE: %ld \n", datagram_len);
                return EXIT_FAILURE;
            }
            // Jesli funkcja odbierajaca zwrocila jakis blad, takze wychodze.
            if (datagram_len < 0) {
                fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
                return EXIT_FAILURE;
            }

            // Zmienna do zapisania adresu odbiorcy
            char router[STRL];
            inet_ntop(AF_INET, &(sender.sin_addr), router, sizeof(router)); // Tutaj robie z tego zwykly string

            if(isMyAddress(router)) continue;   // Odrzuca pakiety, ktore sa ode mnie samego. Takie pakiety sa bezuzyteczne, a wysylanie na adres broadcast sprawia, ze dostaje pakiety od siebie samego.

            // Deklaracja pomocniczych zmiennych
            uint32_t IPtoSend;
            uint8_t mask;
            uint32_t dist;

            // Kopiowanie z buffer do tych zmiennych
            memcpy(&IPtoSend, buffer + 0, sizeof(IPtoSend));
            memcpy(&mask, buffer + 4, sizeof(mask));
            memcpy(&dist, buffer + 5, sizeof(dist));

            // Zmieniam z sieciowego porzadku na hostowy
            IPtoSend = ntohl(IPtoSend);
            dist = ntohl(dist);

            // Zapisuję do tablicy (funkcja sprawdzajaca wpisy)
            insertIntoTable(IPtoSend, mask, dist, router);
        }
    }
    return EXIT_SUCCESS;
}
