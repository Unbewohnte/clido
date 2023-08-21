all:
	gcc -static -O2 -Wall -Werror src/*.c -o clido

clean:
	rm -f clido TODOS.bin

install: all
	mv clido /usr/local/bin
	mkdir -p /usr/local/share/clido
	touch /usr/local/share/clido/TODOS.bin
	chmod -R a+rw /usr/local/share/clido

delete:
	rm -rf ~/.clido
	sudo rm -rf /usr/local/bin/clido