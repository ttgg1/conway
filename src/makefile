# Directories
src=$(wildcard src/*.c)
obj=main.o game.o utils.o cgl.o hashlife.o
DEPS=game.h utils.h cgl.h hashlife.h


B_DIR=../build

# Output
EXEC=$(B_DIR)/conway

# Build settings
CC=gcc
FLAGS=-O2 -I.
CC_GLIB=-I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/sysprof-4 -pthread -lglib-2.0 -lm
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
