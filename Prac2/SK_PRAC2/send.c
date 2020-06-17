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

#include "send.h"
#include "utils.h"

int sendPacket(int sockfd, int index) {
    // Dane odbiorcy. Ustawiam odpowiedni port(54321) i adres sieci, do ktorej to ma pojsc
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(MY_PORT);
    inet_pton(AF_INET, table[index][0], &server_address.sin_addr);

    // Tutaj robie adres broadcast wedlug podanej maski. Poniewaz program musi wysylac pakiety na ten adres, aby dotarly do wszystkim komputerow.
    uint32_t m_temp = INADDR_BROADCAST >> intTable[index][0];
    m_temp = htonl(m_temp);
    server_address.sin_addr.s_addr |= m_temp;

    // Wysylam wszystkie wpisy tablicy routingu jeden za drugim
    for(int i = 0; i<n; i++) {
        uint8_t UDPdata[9];
        ssize_t UDPdata_len = sizeof(UDPdata);

        uint32_t IPtoSend;
        inet_pton(AF_INET, table[i][0], &IPtoSend);
        IPtoSend = htonl(IPtoSend);

        uint8_t mask = (uint8_t)intTable[i][0];

        uint32_t dist = 0;

        // index to numer aktualnie rozpatrywanego interfejsu (index jest mniejszy niz k)
        // i to numer aktualnie rozpatrywanego wpisu z tablicu routingu
        if(turns[index] < TURNUNREACH) {        // Czyli jesli siec jest osiagalna.
            if(turns[i] < TURNUNREACH) {        // Przegladany wpis jest osiagalny. Wysylam normalnie
                dist = htonl(intTable[i][1]);
            }
            else if(turns[i] < TURNDELETE) {    // Przegladany wpis jest nieosiagalny. Jak to wpis direct, wysylam pakiet z nieskonczonoscia
                dist = htonl(intTable[i][1]);   // Wysylam zawartosc tablicy pod indeksem i. Jesli wpis ma w turns < TURNDELETE to na pewno ma tam nieskonczonosc
            }
            else {      // Wpis jest bezuzyteczny. Jesli to wpis direct, zachowuje. Jak nie, moge wywalic
                continue; // Program dojdzie tutaj tylko jesli ma wpis o turns 10, a to jest tylko jak to wpis bezposredni. Nie rozglaszam go po paru turach
            }
        }
        else if (turns[index] < TURNDELETE) {   //Jesli turns skazuja juz na siec nieosiagalna. Sprawdze czy jest to siec bezposrednio podlaczona. Jak tak, wysylam odleglosci z myAddresses. Jak nie - pomijam.
            if(isDir(i)){
                dist = htonl(atoi(myAddresses[i][1]));  // Czyli wysylam do nieosiagalnego routera moje poczatkowe ustawienia odleglosci.
            }
            else {
                continue;   // Nie mam po co wysylac pakietow innych niz bezposrednie do nieosiagalnego routera
            }
        }
        else {  // Jesli doszedlem tutaj to host od dawna nie jest dostepny (nie direct nie maja tego wpisu), wiec musze tylko wysylac odpowiednie odleglosci
            if(isDir(i)) {
                dist = htonl(atoi(myAddresses[i][1]));
            }
            else {
                continue;
            }
        }

        memcpy(UDPdata + 0, &IPtoSend, sizeof(IPtoSend));   // Kopiuje bajty adresu IP to UDPData (to co chce wyslac)
        memcpy(UDPdata + 4, &mask, sizeof(mask));           // Kopiuje tak samo maske podsieci
        memcpy(UDPdata + 5, &dist, sizeof(dist));           // Kopiuje odleglosc

        if(sendto(sockfd, UDPdata, UDPdata_len, 0, (struct sockaddr*) &server_address, sizeof(server_address)) != UDPdata_len) {
            // Sendto zwrocilo blad, czyli nie mam jak sie dostac do tej sieci. Wiem od razu, ze jest nieosiagalna. Wiec moge ustawic odpowiednio tablice.
            intTable[index][1] = UNREACH;
            if(turns[index] < TURNUNREACH) turns[index] = TURNUNREACH;
            strcpy(table[index][1], "0");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;

}

// Wysylam swoja tablice do kazdego z sasiadow
int sendTable(int sockfd) {
    int a = 0;
    for(int i = 0; i<k; i++) {
        a = sendPacket(sockfd, i);
    }
    return a;
}
