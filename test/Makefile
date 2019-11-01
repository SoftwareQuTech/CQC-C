CC=gcc
CFLAGS=-I../ -Wall -Werror -std=c11
LIBS=-lm

SRC_DIR = ./src
BIN_DIR = ./bin

DEPS = ../cqc.h $(SRC_DIR)/test.h
CORE = ../cqc.c

PROG=qubit send_recv epr gates
ALL=$(addprefix $(BIN_DIR)/, $(PROG))

.PHONY: all clean
.DEFAULT: all

all: $(ALL)

clean:
	rm -f $(ALL)

$(BIN_DIR)/%: $(SRC_DIR)/test_%.c $(CORE) $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(CORE) $(LIBS)
