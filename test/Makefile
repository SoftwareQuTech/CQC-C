CC=gcc
CFLAGS=-I../ -Wall -Werror -std=gnu11

DEPS = ../cqc.h
CORE = ../cqc.c

SRC_DIR = ./src
BIN_DIR = ./bin

.PHONY: all clean
.DEFAULT: all

all: qubit send recv gates

clean:
	rm -f $(BIN_DIR)/*

qubit: $(SRC_DIR)/test_qubit.c $(CORE) $(DEPS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $< $(CORE)

send: $(SRC_DIR)/test_send.c $(CORE) $(DEPS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $< $(CORE)

recv: $(SRC_DIR)/test_recv.c $(CORE) $(DEPS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $< $(CORE)

gates: $(SRC_DIR)/test_gates.c $(CORE) $(DEPS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $< $(CORE) -lm
