/*
 *  Source code with the implemented auxiliar functions for the libt2fs.
 */

#include "../include/cutils.h"

int initT2fs(void);
int initRoot(void);
int initFat(void);
int readSuperblock(void);
int readDir(Record *dir);
Record* getLastDir(char *path);

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
 *  Function that get the last directory from the informed path.
 *  Uses strtok to generate the tokens, and opens the dir's cluster to
 *  check the file's records inside the dir.
 *  If the path exists, returns a pointer to the directory Record,
 *  otherwise, returns a NULL pointer.
 */
Record* getLastDir(char *path) {
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

  if (isAbsolute) { // If the given path is ABSOLUTE, starts from the root dir
    dir = root;
  }
  else { // If it is a RELATIVE path, starts from the cwd
    dir = cwd;
  }

  int i = 0, found = 0;
  // A NULL pointer is returned at the end of the string
  while(token != NULL) {
    // copy token to a local variable
    char currentToken[sizeof(token)];
    strcpy(currentToken, token);

    // and generates the next token
    token = strtok(NULL, "/");

    // it means that we already are in the right dir
    if(token == NULL)
      return dir;
    else {
      // given the dir selected above (based on the path),
      // now we have to search for the dir record which name equals the token
      while(strncmp(dir[i].name, currentToken, strlen(token)) != 0 && i < recordsPerDir) {
        i++;
      }
      found = 1;
      // if found something without reaching the end of the dir
      if(i<recordsPerDir) {
        // check if the Record is a directory
        if(dir[i].TypeVal != TYPEVAL_DIRETORIO) {
          // If the user passes a path which parts are not directories
          // (except by the last token, that can be a regular file),
          // then the path is not valid.
          return NULL; // error
        }
        else{ // it is a dir, then read its cluster
          readDir(dir);
        }
      }
      else {
        return NULL; //dir not found inside the current dir
      }
    }
  }

  // if the first strtok results in NULL, it means that the desired dir
  // is the root dir, so just return a pointer to the root Record
  if (found == 0 && isAbsolute) {
    return root;
  }
  else { // if the path was empty
    return NULL;
  }
}


int readDir(Record *dir) {
  unsigned char buffer[SECTOR_SIZE];
  int i;
  for (i = 0; i < superblock.SectorsPerCluster; i++) {
    int adds = superblock.DataSectorStart + (dir->firstCluster * superblock.SectorsPerCluster);
    if (read_sector((adds + i), buffer) != 0) {
      return READ_ERROR;
    }
    memcpy((dir + (recordsPerSector * i)), buffer, SECTOR_SIZE);
  }
}
