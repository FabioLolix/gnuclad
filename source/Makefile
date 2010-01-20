.PHONY: all debug clean
HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)
OBJS=$(wildcard *.o)
BINS=$(SOURCES:.cpp=)
BIN=../bin/gnuclad
CC = g++
CFLAGS = -Wall -Wextra -Werror -pedantic-errors -ansi -O2
DBGFLAGS = -Wall -Wextra -Werror -pedantic-errors -ansi -O0 -g3

$(BIN): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(BIN) $(SOURCES)

all: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(BIN) $(SOURCES)

debug: $(SOURCES) $(HEADERS)
	$(CC) $(DBGFLAGS) -o $(BIN)-dbg $(SOURCES)

clean:
	rm $(BIN)
	rm -f $(BINS)
	rm -f $(OBJS)