# Makefile

include config.mk

all: prepare compile run

prepare:
	mkdir -p $(BUILD_DIR)

compile:
	$(CC) $(FLAGS)

run:
	./$(BUILD_DIR)/$(PROG_NAME)
