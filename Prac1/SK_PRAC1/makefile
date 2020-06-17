CC=gcc
#Makro CC zawiera nazwę kompilatora
CFLAGS=-Wextra -Wall -O2 -std=gnu99
#Makro CFLAGS zawiera parametry kompilatora
CLINK=-lm
#Makro CLINK zawiera opcje linkera.
all: main.o send.o receive.o
	$(CC) -o program main.o send.o receive.o $(CLINK)
#Wykorzystanie kompilatora gcc (makro CC) do zlinkowania
#programu z opcją -lm (CLINK)
main.o: main.c receive.h send.h
	$(CC) -c  $(CFLAGS) main.c
send.o: send.c send.h
	$(CC) -c  $(CFLAGS) send.c
receive.o: receive.c receive.h
	$(CC) -c  $(CFLAGS) receive.c
clean:
	rm -f *.o program
distclean:
	@find $(srctree) $(RCS_FIND_IGNORE) \
		\( -name '*.orig' -o -name '*.rej' -o -name '*~' \
		-o -name '*.bak' -o -name '#*#' -o -name '.*.orig' \
		-o -name '.*.rej' -o -size 0 \
		-o -name '*%' -o -name '.*.cmd' -o -name 'core' \) \
		-type f -print | xargs rm -f
