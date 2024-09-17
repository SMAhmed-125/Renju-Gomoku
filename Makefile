# Compilation flags
CC = gcc
CFLAGS = -Wall -std=c99 -g

# Targets
all: gomoku renju replay
.PHONY: all

# Linking rules
gomoku: gomoku.o game.o io.o board.o
	$(CC) $(CFLAGS) -o $@ $^
renju: renju.o game.o io.o board.o
	$(CC) $(CFLAGS) -o $@ $^
replay: replay.o io.o game.o board.o
	$(CC) $(CFLAGS) -o $@ $^

# Compilation rules
board.o: board.c board.h
	$(CC) $(CFLAGS) -c board.c

game.o: game.c game.h board.c board.h
	$(CC) $(CFLAGS) -c game.c

io.o: io.c io.h game.c game.h board.c board.h
	$(CC) $(CFLAGS) -c io.c

gomoku.o: gomoku.c game.c game.h io.c io.h
	$(CC) $(CFLAGS) -c gomoku.c

renju.o: renju.c game.c game.h io.c io.h
	$(CC) $(CFLAGS) -c renju.c

replay.o: replay.c game.c game.h io.c io.h
	$(CC) $(CFLAGS) -c replay.c

# Clean
clean:
	rm -f board.o game.o io.o gomoku.o renju.o replay.o
	rm -f gomoku renju replay
	rm -f output.txt stderr.txt

