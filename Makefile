CC=cc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb
BIN=ast
SRCS=main.c

$(BIN):$(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

.PHOTY:run
run:$(BIN)
	./$(BIN)
