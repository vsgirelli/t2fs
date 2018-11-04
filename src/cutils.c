/*
 *  Source code with the implemented auxiliar functions for the libt2fs.
 */

#include "../include/cutils.h"

/*
 *  Function that reads the Superblock from the disk.
 */
int readSuperblock() {
  unsigned char buffer[SECTOR_SIZE];

  if (read_sector(0, buffer) != 0) {
    return READ_ERROR;
  }

  // since the ID uses 4 bytes, copy 4 bytes from the buffer
  strncpy(superblock.id, (char *)buffer, 4);
  // and the other informationi's offset is described on the t2fs description
  superblock.version = *( (WORD*) (buffer + 4) );
  superblock.superblockSize = *( (WORD*) (buffer + 6) );
  superblock.DiskSize = *( (DWORD*) (buffer + 8) );
  superblock.NofSectors = *( (DWORD*) (buffer + 12) );
  superblock.SectorsPerCluster = *( (DWORD*) (buffer + 16) );
  superblock.pFATSectorStart = *( (DWORD*) (buffer + 20) );
  superblock.RootDirCluster = *( (DWORD*) (buffer + 24) );
  superblock.DataSectorStart = *( (DWORD*) (buffer + 28) );
  printf("superblock.id: %s\n", superblock.id);
  printf("superblock.version: %d\n", superblock.version);
  printf("superblock.superblockSize: %d\n", superblock.superblockSize);
  printf("superblock.DiskSize: %d\n", superblock.DiskSize);
  printf("superblock.NofSectors: %d\n", superblock.NofSectors);
  printf("superblock.SectorsPerCluster: %d\n", superblock.SectorsPerCluster);
  printf("superblock.pFATSectorStart: %d\n", superblock.pFATSectorStart);
  printf("superblock.RootDirCluster: %d\n", superblock.RootDirCluster);
  printf("superblock.DataSectorStart: %d\n", superblock.DataSectorStart);

  return FUNC_WORKING;
}

/*
 *  Function that checks if the informed path exists.
 *  Uses strtok to generate the tokens, and opens the dir's cluster to
 *  check the file's records inside the dir.
 */
char* checkPath(char *path) {
  int isAbsolute = (*path == '/');

  Record dir;

  char *token = malloc(strlen(path) * sizeof(char));
  int tokenSize = sizeof(token);
  memset(token, '\0', tokenSize);
  strcpy(token, path);
  token = strtok(token, "/");

  if (isAbsolute) { // If the given path is absolute, starts from the root dir
    dir = root;
  }
  else { // If it is a relative path, starts from the cwd
    dir = cwd;
  }


  return token;
}
