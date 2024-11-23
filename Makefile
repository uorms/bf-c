CC = gcc -std=c99

bf:
	$(CC) -o bf bf.c

clean:
	rm bf
