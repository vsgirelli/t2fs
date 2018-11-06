/*
 *  Header file with auxiliar functions's prototypes and datatypes for the libt2fs.
 */

#ifndef __cutils__
#define __cutils__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "t2fs.h"
#include "config.h"
#include "apidisk.h"

// **** DATATYPES

typedef struct t2fs_record Record;
typedef struct t2fs_superbloco Superblock;

/* 
 * Datatype defined for the open files.
 * Contains the file record on the directory
 * and the current pointer position, in bytes.
 */
typedef struct open_file {
  Record *frecord;     // file record
  long int curr_pointer;    // current position pointer, in bytes
} oFile;

// **** GLOBAL VARIABLES
// TODO verificar qual o tipo exatamente do nosso working dir
// Current working directory
Record *cwd;
// Record for the root dir
Record *root;
// Superblock
Superblock superblock;
// Array with the open files.
oFile opened_files[MAX_OPEN_FILES];
// Indicates if the t2fs is initialized
int initializedT2fs;
// Cluster size, in bytes
int clusterSize;
// Maximum records per directory
int recordsPerDir;
// Maximum records per sector
int recordsPerSector;

// 32 bits pointers addressing clusters on the disk
unsigned int *FAT;

// **** Functions's prototypes
// TODO funão de inicializaão que lê o Superbloco e incializa as variáveis
// necessárias

char* checkPath(char *path);

#endif
