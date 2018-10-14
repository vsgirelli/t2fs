/*
 *  File with auxiliar functions and datatypes for the libt2fs.
 */

#ifndef __cutils__
#define __cutils__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "t2fs.h"
#include "config.h"

/* 
 * Datatype defined for the open files.
 * Contains the file record on the directory
 * and the current pointer position, in bytes.
 */
typedef struct open_file {
  struct t2fs_record *frecord;     // file record
  long int curr_pointer;    // current position pointer, in bytes
} oFile;

char *splitPath(char *path);


#endif
