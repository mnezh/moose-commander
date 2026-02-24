CC = gcc
CFLAGS = -Wall -Wextra
BUILD_DIR = build
MC_BINARY = $(BUILD_DIR)/mc
MC_ELKS = $(BUILD_DIR)/mc.elks
TD_BINARY = $(BUILD_DIR)/termdiag
TD_ELKS = $(BUILD_DIR)/termdiag.elks

DOCKER_IMAGE = elks-builder
ELKS_CC = ia16-elf-gcc
ELKS_CFLAGS = -Wall -Wextra -march=i8086 -mtune=i8086 -melks -D__ELKS__=1

IMG_URL = https://github.com/ghaerr/elks/releases/download/v0.8.1/fd1440-fat.img
BASE_ELKS = $(BUILD_DIR)/base-elks-fat.img
BOOT_IMG = $(BUILD_DIR)/work-elks-fat.img

86BOX = /Applications/86Box.app/Contents/MacOS/86Box
VM_PATH = $(shell realpath ./86box)

SRC_MC = src/main.c src/mcurses.c src/data.c src/panel_info.c
SRC_TD = src/termdiag.c
SRC_H = $(wildcard src/*.h)

.PHONY: all clean elks run docker-image mc termdiag

all: $(MC_BINARY) $(TD_BINARY)

$(BUILD_DIR):
	mkdir -p $@

$(MC_BINARY): $(SRC_MC) $(SRC_H) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I src -o $@ $(SRC_MC)

$(TD_BINARY): $(SRC_TD) $(SRC_H) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I src -o $@ $(SRC_TD)

docker-image:
	docker build --platform linux/amd64 -t $(DOCKER_IMAGE) -f Dockerfile.elks .

$(MC_ELKS): docker-image $(SRC_MC) $(SRC_H) | $(BUILD_DIR)
	docker run --rm --platform linux/amd64 -v $(shell pwd):/project $(DOCKER_IMAGE) \
		$(ELKS_CC) $(ELKS_CFLAGS) -I src -o $@ $(SRC_MC)
	@echo "Successfully built: $@"

$(TD_ELKS): docker-image $(SRC_TD) $(SRC_H) | $(BUILD_DIR)
	docker run --rm --platform linux/amd64 -v $(shell pwd):/project $(DOCKER_IMAGE) \
		$(ELKS_CC) $(ELKS_CFLAGS) -I src -o $@ $(SRC_TD)
	@echo "Successfully built: $@"

mc: $(MC_BINARY)
termdiag: $(TD_BINARY)
elks: $(MC_ELKS) $(TD_ELKS)

$(BASE_ELKS): | $(BUILD_DIR)
	@[ -f $(BASE_ELKS) ] || curl -L $(IMG_URL) -o $(BASE_ELKS)

$(BOOT_IMG): $(MC_ELKS) $(TD_ELKS) $(BASE_ELKS)
	cp $(BASE_ELKS) $(BOOT_IMG)
	mcopy -o -i $(BOOT_IMG) $(MC_ELKS) ::/bin/mc
	mcopy -o -i $(BOOT_IMG) $(TD_ELKS) ::/bin/termdiag

run: $(BOOT_IMG) 86box/86box.cfg
	$(86BOX) --vmpath $(VM_PATH) -I A:$(shell realpath $(BOOT_IMG))

clean:
	rm -rf $(BUILD_DIR)
