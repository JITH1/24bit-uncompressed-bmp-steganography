CC = gcc
CFLAGS = -Wall -Wextra

stegout : main.o encode.o decode.o
	$(CC) $(CFLAGS) main.o encode.o decode.o -o stegout
main.o : main.c steggo.h encode.h decode.h
	$(CC) $(CFLAGS) -c main.c
encode.o : encode.c steggo.h encode.h
	$(CC) $(CFLAGS) -c encode.c
decode.o : decode.c decode.h steggo.h
	$(CC) $(CFLAGS) -c decode.c

clean :
	rm -f *.o stegout

