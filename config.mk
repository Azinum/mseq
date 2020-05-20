# config.mk

CC=clang

PROG_NAME=mseq

SRC_DIR=src

SRC_FILES=$(SRC_DIR)/*.c

BUILD_DIR=build

LIBS=-lportaudio -lm

FLAGS=$(SRC_FILES) $(LIBS) -o $(BUILD_DIR)/$(PROG_NAME) -O2 -Iinclude -Wall -W
