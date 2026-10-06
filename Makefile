CC = gcc

main: main.c
	$(CC) -o main main.c

clean: 
	rm -rf main
	rm -rf main.dSYM
