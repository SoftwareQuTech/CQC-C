CC=gcc
CFLAGS=-I../ -Wall -Werror -std=gnu11

SRC_DIR = ./src
BIN_DIR = ./bin

DEPS = ../cqc.h $(SRC_DIR)/test.h
CORE = ../cqc.c

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
