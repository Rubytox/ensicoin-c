BIN_DIR=bin
SRC_DIR=src
INC_DIR=inc
OBJ_DIR=obj

CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -pedantic
LDFLAGS=

OBJ_FILES=$(OBJ_DIR)/sockets.o

TARGET=$(BIN_DIR)/sockets

all: $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $(BIN_DIR)/sockets $(LDFLAGS)

$(OBJ_DIR)/sockets.o: $(SRC_DIR)/sockets.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	rm -rf $(BIN_DIR)/*
	rm -rf $(OBJ_DIR)/*
