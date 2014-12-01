
CC = gcc
CFLAGS = -g -Wall

default:	main

main:	main.o viewshed.o solarPosition.o
	$(CC) $(CFLAGS) $(LDFLAGS) main.o viewshed.o solarPosition.o -o $@

main.o:	main.c viewshed.c viewshed.h solarPosition.c solarPosition.h
	$(CC) $(CFLAGS) -c $< -o $@

viewshed.o:	viewshed.c viewshed.h
	$(CC) $(CFLAGS) -c $< -o $@

solarPosition.o:	solarPosition.c solarPosition.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) main.o viewshed.o solarPosition.o main