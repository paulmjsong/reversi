.PHONY: all
all: reversi

reversi: main.o
	gcc -o reversi main.o -lncurses

main.o: main.c
	gcc -g -c main.c -o main.o

.PHONY: clean
clean:
	rm -rf reversi
	rm -rf main.o
