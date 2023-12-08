CC = gcc

#DEBUG = -D _DEBUG
DEBUG =  


all: curse-fibs

curse-fibs: ./src/curse_fibs.c
	$(CC) $(DEBUG) -O2 -I./src/ -o cfibs ./src/curse_fibs.c ./src/fibs.c -lncurses

clean:
	rm -f cfibs cfibs_debug.out

install: curse-fibs
	mv cfibs $(HOME)/.local/bin/.

