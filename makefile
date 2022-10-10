main: main.o
	gcc -g -Wall -pedantic $^ -o $@

main.o: main.c
	gcc -c -g -Wall -pedantic $< -o $@

.PHONY: clean

clean:
	-rm main.o main
