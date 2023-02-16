conway: main.o
			gcc -o conway main.o

main.o: main.c
			gcc -c main.c -lSDL2
