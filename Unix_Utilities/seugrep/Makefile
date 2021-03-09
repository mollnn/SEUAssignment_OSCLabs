seugrep: seugrep.c
	gcc -o seugrep seugrep.c -Wall 

clean: 
	rm seugrep

test: seugrep test-seugrep.sh
	chmod 777 test-seugrep.sh
	./test-seugrep.sh