# Compiler and flags
CC = gcc
CFLAGS = -Wall
LDFLAGS = -lserialport -lsocketcan

# Directory paths
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = build

# Source files and corresponding object files
SRC = $(SRC_DIR)/main.c
OBJ = $(OBJ_DIR)/main.o

# Executable name
EXEC = $(BIN_DIR)/my_program

# Create the necessary directories if they don't exist
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))

# Default target: Build the executable
all: $(EXEC)

# Rule to link object files into the executable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

# Rule to compile source files into object files
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
