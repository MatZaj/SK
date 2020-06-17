/*
	Mateusz Zając, 298654
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include "utils.h"

#include <stdbool.h>


// Rozdzielam taki ciag znakowy: ADRESIP/MASKA na elementy: one = ADRESIP, two = MASKA
// Korzystam tutaj z funkcji bibliotecznej "strtok", ktora rozdziela ciag znakowy na tokeny według podanych znakow
void splitIntoArgs(char buffer[STRL], char one[STRL], uint32_t *two) {
    char *pch;                              // Wskaznik, ktory bedzie przechowywal aktualny wynik funkcji strtok
    char temp[STRL];                        // Zmienna, ktora bedzie miala w sobie kopie buffer
    strcpy(temp, buffer);                   // Strtok modyfikuje oryginalna zmienna, jesli nie skopiuje jej do temp, dostane segmentation fault)

    pch = strtok(temp, "/");                // Wywoluje funkcje raz na temp, pch bedzie miec teraz sam adres IP (rozdzielam wedlug znaku "/")
    strcpy(one, pch);                       // Kopiuje wynik do "one"
    pch = strtok(NULL, "/");                // Wywoluje funkcje drugi raz, tym razem na NULL. Takie wywolanie sprawia, ze funkcja pracuje na poprzednio przekazanym parametrze (temp)
    *two = atoi(pch);                       // Zapisuje wynik to two, zmieniajac go na int
}

// Czyszcze tablice table, wpisujac do niej "0"
void clearTab3() {
    for(int i = 0; i<IPS; i++) {
        for(int j = 0; j<tELEMS; j++) {
            strcpy(table[i][j], "0");
        }
    }
}

// Czyszcze tablice intTable, wpisujac do niej 0
void cleariTab3() {
    for(int i = 0; i<IPS; i++) {
        for(int j = 0; j<iELEMS; j++) {
            intTable[i][j] = 0;
        }
    }
}

// Czyszcze tablice turns, wpisujac do niej 0
void clearTurns() {
    for(int i = 0; i<IPS; i++) {
        turns[i] = 0;
    }
}

// Wypisuje tablice routingu, odpowiednio sformatowana
void printRouteTable() {

    for(int i = 0; i<n; i++) {

        printf("%s/%u \t", table[i][0], intTable[i][0]);

        if(intTable[i][1] < UNREACHLIMIT)
            printf("distance %u \t", intTable[i][1]);
        else
            printf("unreachable \t");

        if(isDir(i))
            printf("connected directly");
        else
            printf("via %s", table[i][1]);

        printf("\n");
    }
    printf("\n");
}

// Wrzuca do tablicy routingu wpis, jesli ten wpis jest lepszy niz dotychczasowy
// IPtoSend - IP to wpisania, mask - maska do wpisania, dist - dystans do tej sieci, router - ip routera z ktorego to przyszlo
void insertIntoTable(uint32_t IPtoSend, uint8_t mask, uint32_t dist, char router[STRL]) {
    char MyData[STRL];      // String dla nowych danych
    char newIP[STRL];       // String dla nowych danych

    char routerNet[STRL];   // Zmienna trzymajaca adres podsieci routera
    makeSubnetAddressRouter(router, routerNet);     // Robie adres podsieci tego routera

    inet_ntop(AF_INET, &IPtoSend, MyData, STRL);         // W MyData jest IPtoSend wersji string
    makeSubnetAddress(MyData, mask, newIP);              // Tworze adres podsieci wedlug adresu IP i podanej maski, wpisuje to do newIP

    uint32_t routerDistance = searchDistance(router);    // Szukam jaka jest odleglosc do tego routera, ktory mi wyslal pakiet wedlug mojej tablicy routingu

    for(int i = 0; i<n; i++) {                          // Przechodze przez cala swoja tablice routingu
        if(strcmp(newIP, table[i][0]) == 0) {           // Znalazlem adres, ktory pasuje. Sprawdze co trzeba z nim zrobic
            if(routerDistance + dist < intTable[i][1]) {   // Czyli odleglosc z pakietu + odleglosc do tego routera jest mniejsza niz odleglosc, ktora mam zapisana
                if(routerDistance >= UNREACHLIMIT) {     // Jesli odleglosc do routera, z ktorego to przyszlo wedlug mojej tablicy jest wieksza niz UNREACHLIMIT
                    if(strcmp(routerNet, table[i][0]) == 0) {   // Sprawdzam czy to pakiet z tej samej sieci (czy na przyklad od innego sasiada)
                        intTable[i][1] = dist;                  // Jesli to pakiet od mojego sasiada z tej sieci, zmieniam bezwzglednie wszystko na to co mi wyslal i resetuje tury, bo wreszcie dostalem odpowiedz od niego.
                        strcpy(table[i][1], "0");
                        turns[i] = 0;
                    }
                    return;
                }
                else {          // Odleglosc do routera jest mniejsza niz UNREACHLIMIT, czyli jest poprawna
                    if(dist < UNREACHLIMIT) {       // Dystans z pakietu takze jest dobry, wiec pozostaje sprawdzic czy suma nie wyszla poza zakres
                        if(dist + routerDistance >= UNREACHLIMIT) { // Jesli suma wyszla poza zakres
                            if(i<k) {                               // Jesli to jest bezposrednie polaczenie, to wpis na pewno jest jednym z tych pierwszych, wiec jest mniejsze od k
                                    strcpy(table[i][1], "0");       // Wtedy naprawiam po prostu kolumne "via". Zliczanie do do nieskonczonosci najpewniej naruszylo te kolumne, wiec musze miec w niej poprawna wartosc
                            }
                            return;
                        }
                        else {
                            intTable[i][1] = dist + routerDistance;     // W takim razie tutaj dist i routerDistance oraz ich suma jest w porzadku, wiec po prostu zmieniam normalnie na te mniejsze wartosci
                            strcpy(table[i][1], router);
                            turns[i] = 0;
                            return;
                        }

                    }
                    else {      // Przyszedl dystans typu nieskonczonosc
                        if(isVia(router, i)) {                  // Czyli router "via" przyslal mi informacje o tym, ze jest nieskonczona odleglosc
                           intTable[i][1] = UNREACH;            // Wiec zmieniam bezwzglednie. Router przyslal mi informacje, ze jakas siec jest niedostepna
                           if(turns[i] < TURNUNREACH) turns[i] = TURNUNREACH;
                           if(i<k) strcpy(table[i][1], "0");    // Normalizuje wpis "via", zeby wyswietlal sie poprawnie
                        }
                        return;
                    }
                }
            }
            else if(routerDistance + dist == intTable[i][1]) {      // Jesli wyjdzie mi ten sam dystans co mam w tablicy
                if(isDir(i)) {                                      // Jesli to polaczenie bezposrednie
                    if(strcmp(routerNet, table[i][0]) == 0) {       // I informacja przyszla z tej sieci, ktorej to wpis rozpatruje
                        turns[i] = 0;                               // Zeruje tury. Dostalem informacje od mojego sasiada z tej sieci, wiec siec jest dostepna.
                    }
                }
                else {                                              // Jesli to nie jest polaczenie bezposrednie
                    if(isVia(router, i)) {                          // Patrze czy router od ktorego to przyszlo to moj router "via" dla tej sieci. Jesli tak, zeruje tury. Jesli nie, ignoruje to. Bo to moze byc pakiet od innej sieci, ktora jest tylko tak samo dobra, wiec zmiana nie ma sensu.
                        turns[i] = 0;
                    }
                }
                return;
            }
            else {                                                  // Suma, ktora mi wyszla jest gorsza niz to co mam
                if(isVia(router, i)) {                              // Jesli to jest router "via"
                    if(dist + routerDistance < UNREACHLIMIT) {  // I do tego suma nie wyszla poza zakres, to zmieniam na te nowe wartosci. Router "via" widocznie ma jakieś nowsze informacje o jakiejs sieci.
                        intTable[i][1] = dist + routerDistance;
                        turns[i] = 0;
                    }
                    else {                                      // Jesli ten dystans wyszedl poza zakres, uznaje siec na nieosiagalna.
                        intTable[i][1] = UNREACH;
                        if(turns[i] < TURNUNREACH) turns[i] = TURNUNREACH;  // Ustawiam odpowiednio tury tylko jesli wczesniej siec byla osiagalna
                        if(i<k) strcpy(table[i][1], "0");       // Jesli to polaczenie bezposrednie, normalizuje
                    }
                    return;
                }

                if(strcmp(routerNet, table[i][0]) == 0) {           // Patrze czy to przyszlo z tej samej sieci, ktorej jest to wpis. Tutaj wejde jesli siec router nie jest "via". Sprawdze czy router, ktory to przyslal to router z tej sieci. Jak tak, przywracam odleglosc pierwotna.
                    intTable[i][1] = atoi(myAddresses[i][1]);
                    turns[i] = 0;
                }
            }
            return;
        }
    }

    // Przypadek jesli nie mam takiego wpisu w tablicy routingu. Wtedy go dodaje do tablicy.

    if((dist >= UNREACHLIMIT) || (routerDistance + dist >= UNREACHLIMIT)) return;   // Jesli ten potencjalny wpis ma niepoprawna odleglosc, pomijam. Nie chce miec w tablicy niz, co wykracza powyzej UNREACHLIMIT

    routerDistance += dist;             // Dodaje dystans z pakietu do znalezionego dystansu do routera

    intTable[n][0] = (uint32_t)mask;      // Dodaje maske do tablicy routingu

    intTable[n][1] = routerDistance;      // Dodaje dystans  do tablicy routingu

    strcpy(table[n][0], newIP);    // Dodaje adres sieci do tablicy routingu

    strcpy(table[n][1], router);    // Dodaje "via" do tablicy routingu

    turns[n] = 0;                   // Zeruje tury dla tego wpisu

    n = n + 1;    // Zwiekszam n, no bo mam jeden nowy wpis

}

// Szuka dystansu do podanego adresu w tablicy routingu
// W poleceniu jest napisane, ze mozemy przyjac, ze podane sieci sa rozlaczne
// Funkcja szuka pasujacego adresu podsieci porownujac podany adres z kazdym z wpisow w tablicy.
// Przy kazdej iteracji bierze adres z argumentu funkcji oraz maske z danego wpisu w tablicu routingu i przekazuje je jako argumenty do makeSubnetAddress/
// Wynik funkcji jest zapisywany w newIP i ten wynik jest porownywany dopiero z wlasciwym wpisem w tablicy routingu
uint32_t searchDistance(char IP[STRL]) {
    char newIP[STRL];                                   // Zmienna przechowujaca
    for(int i = 0; i<n; i++) {
        makeSubnetAddress(IP, intTable[i][0], newIP);
        if(strcmp(newIP, table[i][0]) == 0) {
            return intTable[i][1];
        }
    }
    return INT_MAX; // Do tej linijki program nie powinien nigdy dojsc
}

// Funkcja tworzaca adres podsieci uzywajac tylko podanego w argumencie IP adresu. W kazdej iteracji najpierw tworzy adres podsieci uzywajac i-tej maski i ip a argumentu i zapisujac wynik do netIP.
// Jesli ten adres bedzie taki sam jak adres podsieci w i-tym kroku, funkcja wraca (bo w netIP siedzi juz wlasciwy adres).
void makeSubnetAddressRouter(char IP[STRL], char netIP[STRL]) {
    for(int i = 0; i<n; i++) {
        makeSubnetAddress(IP, intTable[i][0], netIP);
        if(strcmp(netIP, table[i][0]) == 0) {
            return;
        }
    }
    return;
}

// Tworzy adres podsieci uzywajac podanego adresu ip i maski. Umieszcza wynik w netIP
void makeSubnetAddress(char IP[STRL], uint32_t mask, char netIP[STRL]) {
    uint32_t netIPint;
    inet_pton(AF_INET, IP, &netIPint); // Wpisuje do inta adres
    netIPint = ntohl(netIPint);                 // Przerabiam na hostowa kolejnosc
    netIPint = ((netIPint >> (32-mask)) << (32-mask));  // usuwam bity hosta
    netIPint = htonl(netIPint);                 // Przerabian znowu na sieciowa
    inet_ntop(AF_INET, &netIPint, netIP, STRL);
}

// Sprawdza czy podany w argumencie adres jest wlasnym adresem programu, ktory zostal podany na poczatku.
_Bool isMyAddress(char IP[STRL]) {
    for(int i = 0; i<k; i++) {
        if( strcmp(IP, myAddresses[i][0]) == 0 )  {
            return true;
        }
    }
    return false;
}

// Dodaje tury do kazdego wpisu. Jesli licznik doszedl do TURNDELETE to albo usuwa wpis (jesli to nie jest bezposrednie polaczenie) albo po prostu nie liczy dalej.
void addTurns() {
    for(int i = 0; i<n; i++) {
        if(turns[i] >= TURNDELETE) {
            if(!isDir(i)) {
                deleteRow(i);
            }
            else {
                continue;
            }
        }
        else {
            turns[i] += 1;
        }

        if(turns[i] == TURNUNREACH) {
            intTable[i][1] = (uint32_t)UNREACH;
            if(i<k) {
                strcpy(table[i][1], "0");
            }
        }
    }
}

// Funkcja sprawdza czy dany wiersz to bezposrednie polaczenie.
_Bool isDir(int index) {
    if(strcmp(table[index][1], "0") == 0) return true;
    return false;
}

// Czy dany router jest na trasie (via) w danym wierszu.
_Bool isVia(char router[STRL], int index) {
    if(strcmp(table[index][1], router) == 0) return true;
    return false;
}

// Usuwa i-ty wiersz z tablicy routingu
void deleteRow(int i) {
    n = n-1;

    // Jesli usuwam ostatni wiersz, nie musze nic zamieniac. Po prostu zmiejszam indeks.
    if(i == n) {
        return;
    }
    char buffer[STRL];
    //Kopiuje elementy table
    strcpy(buffer, table[n][0]);
    strcpy(table[i][0], buffer);

    strcpy(buffer, table[n][1]);
    strcpy(table[i][1], buffer);

    // Kopiuje elementy intTable
    intTable[i][0] = intTable[n][0];
    intTable[i][1] = intTable[n][1];

    // Kopiuje elementy turns
    turns[i] = turns[n];
}


