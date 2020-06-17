/*
	Mateusz Zając, 298654
*/

#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "send.h"
#include "receive.h"
#include "utils.h"

int main()
{
    clearTab3();            // Ustawiam table na wartosci "0"
    clearTurns();           // Ustawiam turns na wartosci 0
    cleariTab3();           // Ustawiam intTable na wartosci 0

    char process[STRL];     // Zmienna, ktora sluzy do przetwarzania wejscia danych
    scanf("%d", &n);        // Pobieram ilosc interfejsow (sieci bezposrednio podlaczonych)

    k = n;                  // Kopiuje ta wartosc do zmiennej k

    for(int i = 0; i<n; i++) {                                              // Petla, ktora pobiera n adresow sieci z wejscia
        scanf("%s distance %d", process, &intTable[i][1]);                  // Pobieram pobieram linijke z wejscia. Adres oraz maske skladuje w zmiennej process, aby potem moc ja przetworzyc dalej. Dystans skladuje zwyczajnie w intTable
        splitIntoArgs(process, myAddresses[i][0], &intTable[i][0]);         // Tutaj oddzielam adres sieci i maske od siebie, zapisuje oba elementy oddzielnie.
        makeSubnetAddress(myAddresses[i][0], intTable[i][0], table[i][0]);  // Pobrany adres przerabiam na adres sieci (chce miec w tablicy adresy sieci, nie hostow).
        sprintf(myAddresses[i][1], "%d", intTable[i][1]);                   // Kopiuje pierwotny dystans do danej sieci (aby w razie czego moc do tego dystansu wrocic).
    }


    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);                // Tworze gniazdo surowe do UDP
    if(sockfd < 0) {                                            // Jesli wystapi blad, funkcja zwraca wartosc mniejsza od 0
        fprintf(stderr, "socket error: %s\n", strerror(errno)); // Wypisuje blad na ekran
        return EXIT_FAILURE;                                    // Wychodze z programu, poniewaz wystapil blad
    }

    int broadcastPermission = 1;                                // Musze wlaczyc w gniezdzie opcje wysylania na adres rozgloseniowy
    if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastPermission, sizeof(broadcastPermission)) < 0) {   // Jesli nie uda mi sie ustawic tej opcji
        printf("ERROR SETTING BROADCAST OPTION \n");            // Wypisuje blad na ekran
        close(sockfd);                                          // Zamykam te gniazdo (wystapil blad)
        return EXIT_FAILURE;                                    // Wychodze z programu
    }


    struct sockaddr_in server_address;                          // Deklaruje structa, ktory bedzie zawieral informacje o moim serwerze odbierajacym dane
	bzero (&server_address, sizeof(server_address));            // Zeruje wszystkie bity tej struktury
	server_address.sin_family      = AF_INET;                   // Ustawiam odpowiednia rodzine
	server_address.sin_port        = htons(MY_PORT);            // Ustawiam port (w sieciowym porzadku bajtow)
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);         // Ustawiam adres, z ktorego maja przychodzic pakiety (dla odbioru danych bedzie to dowolny adres, wiec ANY)

	if (bind (sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) { // Jesli nie usa mi sie przypisac gniazda do tego portu
		fprintf(stderr, "bind error: %s\n", strerror(errno));   // Wypisuje na ekran blad
		close(sockfd);                                          // Zamykam gniazdo
		return EXIT_FAILURE;                                    // Wychodze z programu
	}

    for(;;) {                                                   // Chcemy, aby program dzialal caly czas, wiec ustawiam nieskonczona petle
        sendTable(sockfd);                                      // Wywoluje modul, ktory wysyla pakiety. W argumencie przekazuje utworzone gniazdo
        addTurns();                                             // Dodaje 1 do licznika dla kazdego z interfejsow
        receivePackets(sockfd);                                 // Wywoluje modul, ktory odbiera pakiety. W argumencie takze przekazuje utworzone gniazdo.
        printRouteTable();                                      // Wypisuje na ekran tablice routingu
        sleep(10);                                               // Czekam podana w nawiasie ilosc sekund zanim przejde dalej
    }
    close(sockfd);                                              // Zamykam gniazdo
    return EXIT_SUCCESS;                                        // Wychodze z programu
}
