# Makefile para geração de programas teste

CC=gcc
LIB_DIR=../lib

all: main 

main: main.c $(LIB_DIR)/libt2fs.a
	$(CC) -o main main.c -L$(LIB_DIR) -lt2fs -Wall

clean:
	rm -rf main 
	rm -rf teste?.o *~ *log
