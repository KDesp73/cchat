CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g -DDEBUG 
# CFLAGS = -Wall -Wextra -Iinclude

SRC_DIR = src
TEST_DIR = tests
INCLUDE_DIR = include
BUILD_DIR = build

# List all the source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

# Generate the corresponding object file names
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))
OBJ_TEST_FILES = $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/$(TEST_DIR)/%.o,$(TEST_FILES))

# Define the main object file
MAIN_OBJ = $(BUILD_DIR)/main.o

# Define object files excluding the main object file
OBJ_EXCEPT_MAIN = $(filter-out $(MAIN_OBJ),$(OBJ_FILES))

# Target: the final executable
TARGET = cchat
TEST = test

# Default target, build the executable
all: $(BUILD_DIR) $(TARGET)

# Rule to create the build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/$(TEST_DIR)

# Rule to build the executable
$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^

$(TEST): $(OBJ_TEST_FILES) $(OBJ_EXCEPT_MAIN)
	$(CC) -o $@ $^

# Rule to build object files from source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean rule to remove generated files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

compile_commands.json: $(SRC_FILES)
	bear -- make

# FOR TESTING
serve: $(BUILD_DIR) $(TARGET)
	./cchat serve --address 127.0.0.1 --port 9876

connect: $(TARGET)
	./cchat connect --address 127.0.0.1 --port 9876

# Phony target to avoid conflicts with file names
.PHONY: all clean test

