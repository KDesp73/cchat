CC = gcc
CFLAGS = -Wall -Wextra
SRC_DIR = src
BUILD_DIR = build

all: cchat server client

cchat: $(SRC_DIR)/cchat.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC_DIR)/cchat.c -o $(BUILD_DIR)/cchat

server: $(SRC_DIR)/cchat-server.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC_DIR)/cchat-server.c -o $(BUILD_DIR)/cchat-server

client: $(SRC_DIR)/cchat-client.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC_DIR)/cchat-client.c -o $(BUILD_DIR)/cchat-client

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
