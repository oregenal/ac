CC=cc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb
BIN=ac
SRCS=main.c
# EXAMPLE=
EXAMPLE='3 + 2*2/  2*3+1+        2'

$(BIN):$(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

.PHOTY:run
run:$(BIN)
	echo $(EXAMPLE) | ./$(BIN)
