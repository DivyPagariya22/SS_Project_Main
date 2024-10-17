# # Makefile for banking system


# CC = gcc

# CFLAGS = -Wall -I./include  # -Wall enables all warnings, -I specifies the include directory


# SRC = src/main.c src/banking.c src/login.c src/admin.c

# # Object files (replace .c with .o and place in build directory)
# OBJ = $(patsubst src/%.c, build/%.o, $(SRC))

# # Target executable name
# TARGET = banking_system

# # Default target to build the project
# all: $(TARGET)

# # Link object files to create the executable
# $(TARGET): $(OBJ)
# 	$(CC) $(OBJ) -o $(TARGET)

# # Compile source files to object files in the build directory
# build/%.o: src/%.c
# 	mkdir -p build         # Create build directory if it doesn't exist
# 	$(CC) $(CFLAGS) -c $< -o $@

# # Clean target to remove object files and the executable
# clean:
# 	rm -f build/*.o $(TARGET)

# # Phony targets
# .PHONY: all clean

# Compiler and flags
CC = gcc
CFLAGS = -Wall -I./include

# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

# Server and client source files
SERVER_SRC = $(SRC_DIR)/main.c $(SRC_DIR)/utils.c $(SRC_DIR)/admin.c $(SRC_DIR)/banking.c $(SRC_DIR)/customer.c $(SRC_DIR)/employee.c $(SRC_DIR)/manager.c
CLIENT_SRC = $(SRC_DIR)/client.c $(SRC_DIR)/utils.c $(SRC_DIR)/admin.c $(SRC_DIR)/banking.c $(SRC_DIR)/customer.c $(SRC_DIR)/employee.c $(SRC_DIR)/manager.c

# Object files for server and client
SERVER_OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/utils.o $(BUILD_DIR)/admin.o $(BUILD_DIR)/banking.o $(BUILD_DIR)/customer.o $(BUILD_DIR)/employee.o $(BUILD_DIR)/manager.o
CLIENT_OBJS = $(BUILD_DIR)/client.o $(BUILD_DIR)/utils.o $(BUILD_DIR)/admin.o $(BUILD_DIR)/banking.o $(BUILD_DIR)/customer.o $(BUILD_DIR)/employee.o $(BUILD_DIR)/manager.o

# Executables
SERVER_EXEC = server
CLIENT_EXEC = client

# Default target
all: $(SERVER_EXEC) $(CLIENT_EXEC)

# Build the server
$(SERVER_EXEC): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $(SERVER_EXEC) $(SERVER_OBJS)

# Build the client
$(CLIENT_EXEC): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $(CLIENT_EXEC) $(CLIENT_OBJS)

# Compile server object files
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/utils.o: $(SRC_DIR)/utils.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/utils.c -o $(BUILD_DIR)/utils.o

$(BUILD_DIR)/admin.o: $(SRC_DIR)/admin.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/admin.c -o $(BUILD_DIR)/admin.o

$(BUILD_DIR)/banking.o: $(SRC_DIR)/banking.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/banking.c -o $(BUILD_DIR)/banking.o

# Compile client object files
$(BUILD_DIR)/client.o: $(SRC_DIR)/client.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/client.c -o $(BUILD_DIR)/client.o

$(BUILD_DIR)/customer.o: $(SRC_DIR)/customer.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/customer.c -o $(BUILD_DIR)/customer.o

$(BUILD_DIR)/employee.o: $(SRC_DIR)/employee.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/employee.c -o $(BUILD_DIR)/employee.o

$(BUILD_DIR)/manager.o: $(SRC_DIR)/manager.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/manager.c -o $(BUILD_DIR)/manager.o

# Clean up build artifacts
clean:
	rm -f $(BUILD_DIR)/*.o $(SERVER_EXEC) $(CLIENT_EXEC)

