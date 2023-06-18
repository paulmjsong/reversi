.PHONY: all
all: reversi

reversi: reversi.c
	gcc reversi.c -o reversi

reversi.o: reversi.c
	gcc -g -c reversi.c -lncurses -o reversi.o

.PHONY: clean
clean:
	rm -rf reversi
	rm -rf reversi.o
