# config.mk

CC=gcc

PROG_NAME=mseq

SRC_DIR=src

SRC_FILES=$(SRC_DIR)/*.c

BUILD_DIR=build

INCLUDE_DIR=include

LIBS=-lportaudio -lm

FLAGS=$(SRC_FILES) $(LIBS) -o $(BUILD_DIR)/$(PROG_NAME) -O2 -I$(INCLUDE_DIR) -Wall -W

# shared library configuration

LIB_PATH=/usr/lib

LIB_NAME=lib$(PROG_NAME)

LIB_INCLUDE_PATH=/usr/include/$(PROG_NAME)

LIB_BUILD_DIR=$(BUILD_DIR)/shared

FLAGS_LIB=$(SRC_FILES) -I$(INCLUDE_DIR) -shared -fPIC $(LIBS) -W -Wall -o $(LIB_BUILD_DIR)/$(LIB_NAME).so -D NDEBUG -D COMP_SHARED_LIB
