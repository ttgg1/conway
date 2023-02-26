# Directories
src=$(wildcard src/*.c)
obj=$(src:.c=.o)
B_DIR=build

# Files
S_FILES=$(S_DIR)/*

# Output
EXEC=$(B_DIR)/conway

# Build settings
CC=gcc
FLAGS=-O2
# SDL options
CC_SDL=-lSDL2 `sdl2-config --cflags --libs`

all:conway

conway: $(obj)
	$(CC) -o $(ECEC) $@ $^ $(CC_SDL) $(FLAGS)

build_run:conway
	$(EXEC)

.PHONY: clean
clean:
	rm -f $(obj) conway
