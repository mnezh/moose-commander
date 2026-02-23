CC=gcc
CFLAGS=-Wall -Wextra
BUILD_DIR=build
TARGET=$(BUILD_DIR)/mc
SRC_C=$(wildcard src/*.c)
SRC_H=$(wildcard src/*.h)

all: $(TARGET)

$(TARGET): $(SRC_C) $(SRC_H) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I src -o $@ $(SRC_C)

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)
