SRC_DIR := src
INC_DIR := include
BIN_DIR := bin

CC := gcc
CFLAGS := -Wall -Wextra -g $(shell sdl2-config --cflags) -I $(INC_DIR)
LIBS := $(shell sdl2-config --libs) -lSDL2_ttf -lm
EXEC := app_hack

SRC := $(wildcard $(SRC_DIR)/*.c) \
	$(wildcard $(SRC_DIR)/util/*.c) \
	$(wildcard $(SRC_DIR)/scene/*c)

OBJ := $(SRC:$(SRC_DIR)/%.c=$(BIN_DIR)/%.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(EXEC) $(BIN_DIR)/*.o $(BIN_DIR)/util/*.o

.PHONY: all clean
