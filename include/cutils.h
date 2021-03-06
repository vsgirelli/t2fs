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
  Record *frecord;          // file record
  long int curr_pointer;    // current position pointer, in bytes or number of dirEntry
  DWORD parentCluster;      // parent cluster
} oFile;


// **** GLOBAL VARIABLES
// Current working directory
Record *cwd;
// Current path name:
char currentPath[MAX_FILE_NAME_SIZE + 1];
// Record for the root dir
Record *root;
// Superblock
Superblock superblock;
// Array with the open files.
oFile opened_files[MAX_OPEN_FILES];
// Bitmap for the open files positions
short int opened_files_map[MAX_OPEN_FILES];
// Indicates if the t2fs is initialized
int initializedT2fs;
// Cluster size, in bytes
int clusterSize;
// Maximum records per directory
int recordsPerDir;
// Maximum records per sector
int recordsPerSector;
// Number of opened files
short int numberOfOpenedFiles;

// 32 bits pointers addressing clusters on the disk
DWORD *FAT;
// number of fat pointers per sector
DWORD pointersPerSector;
// fat size in sectors
DWORD fatSizeInSectors;
// index for creating files
DWORD currentFreeFATIndex;
// last index for the FAT
DWORD lastFATIndex;


// **** Functions's prototypes
Record* getLastDir(char *path);
int initT2fs(void);
void ls(Record *dir);
char *getFileName(char *path);
Record* openFile(char *pathname);
int getNextHandleNum();
char * readCluster(int cluster);
int isValidDirEntry(BYTE typeVal);
//void formatString(char *string, char *new_string);
DWORD getNextFreeFATIndex();
int writeCluster(BYTE *buffer, int clusterNumber);
unsigned int createFile(char *filename, int typeval);
int writeFAT(void);
void fixPath(char *path);
void updateDir(oFile ofile);

#endif

