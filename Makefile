SRC = src
INC = include
UTIL = $(SRC)/util
SCN = $(SRC)/scene
SRCS = $(UTIL)/* $(SCN)/*

CC = g++
CFLAGS = -Wall -Werror -I $(INC)

render: $(SRC)/main.c
	$(CC) $(CFLAGS) $@ -o $<

$(SRC)/main.c: $(SRCS)
	$(CC) $(CFLAGS) $@ -o $< 

clean:
	rm -f render