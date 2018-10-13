#
# Makefile ESQUELETO
#
# DEVE ter uma regra "all" para geração da biblioteca
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
#
# 

CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src

OBJS=$(BIN_DIR)/t2fs.o

all: $(BIN_DIR)/t2fs.o
	ar crs $(LIB_DIR)/libt2fs $(OBJS)

$(BIN_DIR)/t2fs.o: $(SRC_DIR)/t2fs.c
	gcc -c $(SRC_DIR)/t2fs.c -o $(BIN_DIR)/t2fs.o -Wall

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~

tests:
	$(MAKE) -C testes

tests_clean:
	$(MAKE) -C testes clean
