# encrypter/Makefile  (אם בכל זאת תרצי make מקומי)
CC      := gcc
CFLAGS  := -Wall
LDFLAGS := -lrt
LIBS    := -lmta_rand -lmta_crypt -lcrypto -pthread

encrypter: encrypter.c utils.c
	$(CC) $(CFLAGS) -o encrypter encrypter.c utils.c $(LDFLAGS) $(LIBS)

