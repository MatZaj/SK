/*
	Mateusz Zając, 298654
*/

#ifndef utils_h
#define utils_h

#define IPS 5
#define tELEMS 2
#define iELEMS 2
#define STRL 50
#define MY_PORT 54321
#define UNREACH 0xFFFFFFFF

#define TURNUNREACH 5
#define TURNDELETE 10

#define UNREACHLIMIT 16

#include <stdint.h>

int n;                              // Zmienna przechowujaca ilosc wpisow
char table[IPS][tELEMS][STRL];      // Pierwsza czesc tablicy routingu
int turns[IPS];                     // Licznik tur dla kazdego z wpisow
uint32_t intTable[IPS][iELEMS];     // Druga czesc tablicy routingu

char myAddresses[IPS][2][STRL];     // Tablica moich wlasnych adresow, ktore sa podlaczone bezposrednio
int k;                              // Ilosc moich wlasnych adresow


// Rozdzielam adres ip z maska podsieci wedlug znaku "/". Adres ip jest zapisywany do "one", maska do "two"
void splitIntoArgs(char buffer[STRL], char one[STRL], uint32_t *two);

// Czyszcze tablice table
void clearTab3();

// Czyszcze tablice intTab
void cleariTab3();

// Czyszcze tablice turns
void clearTurns();

// Wypisuje na ekran tablice rutingu
void printRouteTable();

// Funkcja, ktora odpowiada za wpisanie do tablicy routingu nowych wpisow lub aktualizacje tych juz posiadanych
void insertIntoTable(uint32_t IPtoSend, uint8_t mask, uint32_t dist, char router[STRL]);

// Wyszukaj odleglosc w tablicy routingu do adresu, ktory jest przeslany w argumencie
uint32_t searchDistance(char IP[STRL]);

// Stworz adres sieci dla podanego adresu IP i wynik zapisz w netIP
void makeSubnetAddressRouter(char IP[STRL], char netIP[STRL]);

// Stworz adres sieci z podanego adresu IP oraz maski podsieci i wynik zapisz w netIP
void makeSubnetAddress(char IP[STRL], uint32_t mask, char netIP[STRL]);

// Funkcja sprawdzajaca czy dany adres jest adresem wlasnym programu czy jest to adres jakiegos innego komputera
_Bool isMyAddress(char IP[STRL]);

// Dodaje do licznika +1 dla kazdego wpisu tablicy routingu
void addTurns();

// Sprawdza czy wpis o numerze "index" w tablicy routingu jest polaczeniem bezposrednim czy tez nie
_Bool isDir(int index);

// Sprawdza czy podany adres(router) jest wpisem "via" w danym indeksie tablicy routingu
_Bool isVia(char router[STRL], int index);

// Usuwa i-ty wpis z tablicy routingu
void deleteRow(int i);

#endif
