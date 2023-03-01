# Directories
src=$(wildcard src/*.c)
obj=main.o game.o
DEPS=game.h


B_DIR=../build

# Output
EXEC=$(B_DIR)/conway

# Build settings
CC=gcc
FLAGS=-O2 -I.
CC_GLIB=-I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/sysprof-4 -pthread -lglib-2.0
# SDL options
CC_SDL=-lSDL2 `sdl2-config --cflags --libs`

all: conway

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(FLAGS)

conway: $(obj)
	$(CC) $(FLAGS) -o $(EXEC) $(obj) $(CC_SDL) $(CC_GLIB)

.PHONY: clean
clean:
	rm -f $(obj) $(EXEC)
