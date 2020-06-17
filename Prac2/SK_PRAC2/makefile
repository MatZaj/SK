CC=gcc
#Makro CC zawiera nazwę kompilatora
CFLAGS=-Wextra -Wall -O2 -std=gnu99
#Makro CFLAGS zawiera parametry kompilatora
CLINK=-lm
#Makro CLINK zawiera opcje linkera.
all: main.o send.o receive.o utils.o
	$(CC) -Wall -Wextra -std=c99 -o router main.o send.o receive.o utils.o $(CLINK)
#Wykorzystanie kompilatora gcc (makro CC) do zlinkowania
#programu z opcją -lm (CLINK)
main.o: main.c receive.h send.h utils.h
	$(CC) -c  $(CFLAGS) main.c
send.o: send.c send.h utils.h
	$(CC) -c  $(CFLAGS) send.c
receive.o: receive.c receive.h utils.h
	$(CC) -c  $(CFLAGS) receive.c
utils.o: utils.c utils.h
	$(CC) -c  $(CFLAGS) utils.c
clean:
	rm -f *.o
distclean:
	rm -f *.o router
