CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lmta_crypt -lmta_rand -lpthread

SRCS = $(wildcard *.c) #Automatically collects all .c source files in the current directory
OBJS = $(SRCS:.c=.o)

TARGET = encrypt.out

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

