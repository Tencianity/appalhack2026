SRC = ./src
INC = ./include
SRCS = $(shell find $(SRC) -name *.c)

CC = gcc
CFLAGS = -std=c17 -lSDL2 -Wall -I $(INC)

render: $(SRC)/main.c
	$(CC) $(CFLAGS) $< -o $@

$(SRC)/main.c: $(SRCS)
	$(CC) $(CFLAGS) $< -o $@ 

clean:
	rm -f render