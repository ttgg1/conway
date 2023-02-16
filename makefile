# Directories
S_DIR=src
B_DIR=build

# Files
S_FILES=$(S_DIR)/main.c

# Output
EXEC=$(B_DIR)/conway

# Build settings
CC=gcc
# SDL options
CC_SDL=-lSDL2 `sdl2-config --cflags --libs`


all:Build

Build:
	$(CC) $(S_FILES) -o $(EXEC) $(CC_SDL)

build_run:Build
	$(EXEC)

clean:
	rm -rf $(B_DIR)/*
