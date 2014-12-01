
CC = gcc
CFLAGS = -g -Wall

default:	main

main:	main.o viewshed.o solarPosition.o irradiance.o
	$(CC) $(CFLAGS) $(LDFLAGS) main.o viewshed.o solarPosition.o irradiance.o -o $@

main.o:	main.c viewshed.c viewshed.h solarPosition.c solarPosition.h irradiance.c irradiance.h
	$(CC) $(CFLAGS) -c $< -o $@

viewshed.o:	viewshed.c viewshed.h
	$(CC) $(CFLAGS) -c $< -o $@

solarPosition.o:	solarPosition.c solarPosition.h
	$(CC) $(CFLAGS) -c $< -o $@

irradiance.o:	irradiance.c irradiance.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) main.o viewshed.o solarPosition.o irradiance.o main