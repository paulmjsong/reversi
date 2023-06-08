.PHONY: all
all: reversi

reversi: reversi.o
	gcc reversi.o -o reversi

reversi.o: reversi.c
	gcc -g -c reversi.c -o reversi.o

.PHONY: clean
clean:
	rm -rf reversi
	rm -rf reversi.o
