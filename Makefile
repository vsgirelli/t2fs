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

OBJS=$(BIN_DIR)/t2fs.o $(BIN_DIR)/cutils.o

all: t2fs cutils
	ar crs $(LIB_DIR)/libt2fs.a $(OBJS) $(LIB_DIR)/apidisk.o

t2fs: $(SRC_DIR)/t2fs.c
	gcc -c $(SRC_DIR)/t2fs.c -o $(BIN_DIR)/t2fs.o -Wall -ggdb

cutils: $(SRC_DIR)/cutils.c
	gcc -c $(SRC_DIR)/cutils.c -o $(BIN_DIR)/cutils.o -Wall -ggdb

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~

tests:
	$(MAKE) -C testes

tests_clean:
	$(MAKE) -C testes clean
