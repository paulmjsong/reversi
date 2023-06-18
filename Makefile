.PHONY: all
all: reversi

reversi: main.o
	gcc -o reversi play.o main.o -lncurses

main.o: main.c header.h
	gcc -g -c main.c -o main.o

play.o: play.c header.h
	gcc -g -c play.c -o play.o

.PHONY: clean
clean:
	rm -rf reversi
	rm -rf main.o
	rm -rf play.o
