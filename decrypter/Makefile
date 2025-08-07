# decrypter/Makefile

CC      := gcc
CFLAGS  := -Wall -I..
LDFLAGS := -lrt
LIBS    := -lmta_rand -lmta_crypt -lcrypto -pthread

decrypter: decrypters.c utils.c
	$(CC) $(CFLAGS) -o decrypter decrypters.c utils.c $(LDFLAGS) $(LIBS)
