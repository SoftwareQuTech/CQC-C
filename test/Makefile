CC=gcc
CFLAGS=-I../ -Wall -Werror -std=gnu11

DEPS = ../cqc.h
CORE = ../cqc.c

BIN_DIR = ./bin

.PHONY: all clean
.DEFAULT: all

all: qubit send recv gates

clean:
	rm -f $(BIN_DIR)/*

qubit: test_qubit.c $(CORE) $(DEPS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $< $(CORE)

send: test_send.c $(CORE) $(DEPS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $< $(CORE)

recv: test_recv.c $(CORE) $(DEPS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $< $(CORE)

gates: test_gates.c $(CORE) $(DEPS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $< $(CORE) -lm
