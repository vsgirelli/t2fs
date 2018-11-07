/*
 *  Source code with the implemented auxiliar functions for the libt2fs.
 */

#include "../include/cutils.h"

int initT2fs(void);
int initRoot(void);
int initFat(void);
int readSuperblock(void);
char* checkPath(char *path);

/*
 *  Function that initializes the root dir
 */
int initRoot() {
  // malloc allocates based on the byte count, not on the type, so there is no
  // need for cast.
  // All directories occupy ONE cluster
  root = malloc(clusterSize);
  unsigned char rootBuffer[SECTOR_SIZE];
  // reading root's cluster (all of it's sectors, given by SectorsPerCluster)
  int i;
  for (i = 0; i < superblock.SectorsPerCluster; i++) {
    // The initial address is given by the offset in sectors from the DataSectorStart
    // The offset depends on the amount of SectorsPerCluster and the cluster we
    // are trying to access.
    // Thus, starting from the DataSectorStart, and skipping n clusters of size
    // SectorsPerCluster, we have the sector that initiates the cluster.
    // Currently, the cluster we are trying to read is the RootDirCluster.
    int adds = superblock.DataSectorStart + (superblock.RootDirCluster * superblock.SectorsPerCluster);
    if (read_sector((adds + i), rootBuffer) != 0) {
      return READ_ERROR;
    }
    // memcpy does a binary copy of the data.
    // Once the root pointer is a Record pointer, we shall read a total of recordsPerSector
    // in each access to the disk.
    memcpy((root + (recordsPerSector * i)), rootBuffer, SECTOR_SIZE);
  }
  return FUNC_WORKING;
}

/*
 *  Function that initializes the t2fs
 */
int initT2fs() {
  if(initializedT2fs) {
    return 0;
  }

  if (readSuperblock() != 0) {
    return READ_ERROR;
  }

  // INITIALIZING VARIABLES FROM THE SUPERBLOCK INFO
  // The cluster size (in bytes) is defined by the SECTOR_SIZE * the number of
  // sectors in a cluster
  clusterSize = SECTOR_SIZE * superblock.SectorsPerCluster;
  // And the maximum number of records in a dir depends on the cluster size
  // (because a dir occupies one cluster) and the size of the records's struct

  recordsPerDir = clusterSize/sizeof(Record);
  // The maximum number of records in a sector depends on the SECTOR_SIZE and
  // the size of the record
  recordsPerSector = SECTOR_SIZE/sizeof(Record);

  if(initRoot() != 0) {
    return READ_ERROR;
  }
  //initFat();

  return FUNC_WORKING;

}

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

  initT2fs();
  Record *dir;

  char *token = malloc(strlen(path) * sizeof(char));
  int tokenSize = sizeof(token);
  memset(token, '\0', tokenSize);
  strcpy(token, path);
  // The first call to strtok needs the the starting string to scan tokens
  // The next calls receives a null pointer and uses the position right after the
  // end of the last token to start scanning for the tokens.
  token = strtok(token, "/");
  // The first token will be the first dir or file name, whether the path is
  // absolute or not.

  if (isAbsolute) { // If the given path is absolute, starts from the root dir
    dir = root;
  }
  else { // If it is a relative path, starts from the cwd
    dir = cwd;
  }

  int i = 0;
  // A NULL pointer is returned at the end of the string
  while(token != NULL) {
    // given the dir selected above (based on the path),
    // now we have to search for the dir record which name equals the token
    while(strncmp(dir[i].name, token, strlen(token)) != 0 && i < recordsPerDir) {
      printf("File name: %s\n", dir[i].name);
      i++;
    }
    // at this point, use a flag variable to check if the token was found or not
    // if it was found, then access the cluster and generates a new token
    // otherwise, return
    printf("File name: %s\n", dir[i].name);
    //strtok again
    token = strtok(NULL, "/");
  }


  return token;
}
