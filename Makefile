IDIR = -I.
CC = g++
CFLAGS = -Wall -std=c++17 -fPIC

PROGRAMS = bitcoin_shell printdb db_to_csv reload_db block_finder
TARGETS = $(addsuffix .out,$(PROGRAMS))

LIB_SRCS = utils.cpp functions.cpp
LIB = utils
LIBPATH = lib$(LIB).so

$(LIBPATH): $(LIB_SRCS)
	$(CC) $(CFLAGS) -shared -o $@ $^

%.out: %.cpp $(LIBPATH)
	$(CC) $(CFLAGS) -o $@ $< $(IDIR) -l$(LIB) -L. -Wl,-rpath=.

all: $(TARGETS)

.PHONY: clean
clean:
	rm -f *.out lib$(LIB).so

