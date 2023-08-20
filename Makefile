all:
	gcc -static -O2 -Wall -Werror src/*.c -o clido

clean:
	rm -f clido TODOS.bin

install: all
	mv clido /usr/local/bin

delete:
	rm -f /usr/local/bin/clido