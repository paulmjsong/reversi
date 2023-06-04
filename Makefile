.PHONY: all
all: reversi

reversi: reversi.o
  gcc -o reversi reversi.o

reversi.o: keep.c
	gcc -o reversi.o -g -c keep.c

.PHONY: clean
clean:
	rm -rf reversi
	rm -rf reversi.o
