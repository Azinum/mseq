# Makefile

include config.mk

all: prepare compile run

clean:
	rm -drf $(BUILD_DIR)/*

prepare:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(LIB_BUILD_DIR)

compile:
	$(CC) $(FLAGS)

shared:
	$(CC) $(FLAGS_LIB)
	chmod o+x $(LIB_BUILD_DIR)/$(LIB_NAME).so
	cp $(LIB_BUILD_DIR)/$(LIB_NAME).so $(LIB_PATH)/
	mkdir -p $(LIB_INCLUDE_PATH)
	cp -r $(INCLUDE_DIR)/* $(LIB_INCLUDE_PATH)/
run:
	./$(BUILD_DIR)/$(PROG_NAME)
