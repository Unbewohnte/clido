all:
	gcc -static -O2 -Wall -Werror src/*.c -o clido

clean:
	rm -f clido TODOS.bin

install: all


delete:
