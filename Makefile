CC = gcc -std=c99

bf: bf.c
	$(CC) -o $@ $^

clean:
	rm bf
