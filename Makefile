CC=cc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb
BIN=ac
SRCS=main.c
EXAMPLE='2 + 3*4*6 + 6      - 20 * 2'

$(BIN):$(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

.PHOTY:run
run:$(BIN)
	echo $(EXAMPLE) | ./$(BIN)
