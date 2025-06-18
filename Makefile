CC=gcc
CFLAGS=-Wall -g -Wextra -pedantic -pthread -std=c99 -fsanitize=address,undefined

all: ctencrypt ctdecrypt scrypt bencrypt bdecrypt

ctencrypt: ctencrypt.c
	$(CC) $(CFLAGS) ctencrypt.c -o ctencrypt

ctencrypt.o: ctencrypt.c
	$(CC) $(CFLAGS) -o $@ $^

ctdecrypt: ctdecrypt.c
	$(CC) $(CFLAGS) ctdecrypt.c -o ctdecrypt

ctdecrypt.o: ctdecrypt.c
	$(CC) $(CFLAGS) -o $@ $^

scrypt: scrypt.c
	$(CC) $(CFLAGS) scrypt.c -o scrypt

scrypt.o: scrypt.c
	$(CC) $(CFLAGS) -o $@ $^

bencrypt: bencrypt.c
	$(CC) $(CFLAGS) bencrypt.c -o bencrypt

bencrypt.o: bencrypt.c
	$(CC) $(CFLAGS) -o $@ $^

bdecrypt: bdecrypt.c
	$(CC) $(CFLAGS) bdecrypt.c -o bdecrypt

bdecrypt.o: bdecrypt.c
	$(CC) $(CFLAGS) -o $@ $^
