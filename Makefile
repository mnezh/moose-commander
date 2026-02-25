CC = gcc
CFLAGS = -Wall -Wextra
BUILD_DIR = build
MC_BINARY = $(BUILD_DIR)/mc
MC_ELKS = $(BUILD_DIR)/mc.elks
SD_BINARY = $(BUILD_DIR)/sysdiag
SD_ELKS = $(BUILD_DIR)/sysdiag.elks

DOCKER_IMAGE = elks-builder
ELKS_CC = ia16-elf-gcc
ELKS_CFLAGS = -Wall -Wextra -march=i8086 -mtune=i8086 -melks -D__ELKS__=1

IMG_URL = https://github.com/ghaerr/elks/releases/download/v0.8.1/fd1440-fat.img
BASE_ELKS = $(BUILD_DIR)/base-elks-fat.img
BOOT_IMG = $(BUILD_DIR)/work-elks-fat.img

86BOX = /Applications/86Box.app/Contents/MacOS/86Box
VM_PATH = $(shell realpath ./86box)

SRC_MC = src/main.c src/mcurses.c src/data.c src/panel_info.c src/util.c
SRC_SD = src/sysdiag.c
SRC_ELKS = src/elks.c
SRC_UTIL = src/util.c
SRC_H = $(wildcard src/*.h)

.PHONY: all clean elks run docker-image mc sysdiag

all: $(MC_BINARY) $(SD_BINARY)

$(BUILD_DIR):
	mkdir -p $@

$(MC_BINARY): $(SRC_MC) $(SRC_H) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I src -o $@ $(SRC_MC)

$(SD_BINARY): $(SRC_SD) $(SRC_H) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I src -o $@ $(SRC_SD)

docker-image:
	docker build --platform linux/amd64 -t $(DOCKER_IMAGE) -f Dockerfile.elks .

$(MC_ELKS): docker-image $(SRC_MC) $(SRC_ELKS) $(SRC_H) | $(BUILD_DIR)
	docker run --rm --platform linux/amd64 -v $(shell pwd):/project $(DOCKER_IMAGE) \
		$(ELKS_CC) $(ELKS_CFLAGS) -I src -o $@ $(SRC_MC) $(SRC_ELKS)
	@echo "Successfully built: $@"

$(SD_ELKS): docker-image $(SRC_SD) $(SRC_ELKS) $(SRC_UTIL) $(SRC_H) | $(BUILD_DIR)
	docker run --rm --platform linux/amd64 -v $(shell pwd):/project $(DOCKER_IMAGE) \
		$(ELKS_CC) $(ELKS_CFLAGS) -I src -o $@ $(SRC_SD) $(SRC_ELKS) $(SRC_UTIL)
	@echo "Successfully built: $@"

mc: $(MC_BINARY)
sysdiag: $(SD_BINARY)
elks: $(MC_ELKS) $(SD_ELKS)

$(BASE_ELKS): | $(BUILD_DIR)
	@[ -f $(BASE_ELKS) ] || curl -L $(IMG_URL) -o $(BASE_ELKS)

$(BOOT_IMG): $(MC_ELKS) $(SD_ELKS) $(BASE_ELKS)
	cp $(BASE_ELKS) $(BOOT_IMG)
	mcopy -o -i $(BOOT_IMG) $(MC_ELKS) ::/bin/mc
	mcopy -o -i $(BOOT_IMG) $(SD_ELKS) ::/bin/sysdiag

run: $(BOOT_IMG) 86box/86box.cfg
	$(86BOX) --vmpath $(VM_PATH) -I A:$(shell realpath $(BOOT_IMG))

clean:
	rm -rf $(BUILD_DIR)
