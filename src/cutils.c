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
char *getFileName(char *path);
void ls(Record *dir);
DWORD getNextFreeFATIndex(void);
int writeCluster(BYTE *buffer, int clusterNumber);
Record *getFileRecord(char *path);

/*
 *  Function that initializes the root dir
 */
int initRoot() {
  // malloc allocates based on the byte count, not on the type, so there is no
  // need for cast.
  // All directories occupy ONE cluster
  root = malloc(clusterSize);
  BYTE rootBuffer[SECTOR_SIZE];
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

  int i;
  // INITIALIZE OPENEDFILES ARRAY
  for (i=0; i<MAX_OPEN_FILES; i++){
    opened_files_map[i] = 0;
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

  pointersPerSector = SECTOR_SIZE/sizeof(DWORD);

  fatSizeInSectors = superblock.DataSectorStart - superblock.pFATSectorStart;

  if(initRoot() != 0) {
    return READ_ERROR;
  }
  cwd = root;
  initFat();

  initializedT2fs = 1;
  return FUNC_WORKING;
}

/*
*   Function that initializes FAT
*/
int initFat() {
    currentFreeFATIndex = 0;
    lastFATIndex = (fatSizeInSectors  * pointersPerSector) - 1;

    /* I'm FAT and I know it,
        FAT é um vetor de unsigned int (4 bytes)
        Começa em pFATSectorStart e termina em DataSectorStart
    */
    FAT = malloc(SECTOR_SIZE * fatSizeInSectors);

    BYTE readBuffer[SECTOR_SIZE];
    int i;
    for (i = 0; i < fatSizeInSectors; i++) {

        int adds = superblock.pFATSectorStart;
        if (read_sector((adds + i), readBuffer) != 0) {
          return READ_ERROR;
        }
        memcpy((FAT + (pointersPerSector * i)), readBuffer, SECTOR_SIZE);
    }

    return FUNC_WORKING;
}

/*
 * Searches for the next free FAT cluster
 */
DWORD getNextFreeFATIndex() {
    // This flag determines if all FAT indexes where searched
    short int scanComplete = 0;
    // This flag determines if we reseted the Index to search from the beginning
    short int reseted = 0;

    DWORD start = currentFreeFATIndex;

    while (FAT[currentFreeFATIndex] != 0 && scanComplete == 0)
    {
        if (currentFreeFATIndex == start && reseted == 1)
        {
            scanComplete = 1;
        }
        if (currentFreeFATIndex == lastFATIndex) {

            currentFreeFATIndex = 0;
            reseted = 1;
        }
        currentFreeFATIndex += 1;
    }
    if (scanComplete == 1)
    {
        return NO_FREE_INDEXES;
    }

    return currentFreeFATIndex;
}

/*
 *  Function that reads the Superblock from the disk.
 */
int readSuperblock() {
  BYTE buffer[SECTOR_SIZE];

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
    strncpy(currentToken, token, sizeof(token));

    // and generates the next token
    token = strtok(NULL, "/");

    // it means that we already are in the right dir
    if(token == NULL) {
      return dir;
    }
    else {
      // given the dir selected above (based on the path),
      // now we have to search for the dir record which name equals the token
      while(strncmp(dir[i].name, currentToken, strlen(currentToken)) != 0 && i < recordsPerDir) {
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
          dir = (Record *) readCluster(dir[i].firstCluster);
        }
      }
      else {
        return NULL; //dir not found inside the current dir
      }
    }
  }

  free(token);
  // if the first strtok results in NULL, it means that the desired dir
  // is the root dir, so just return a pointer to the root Record
  if (found == 0 && isAbsolute) {
    return root;
  }
  else { // if the path was empty
    return NULL;
  }
}


/*
 * Function that formats the string to a full format
 * Had to do it because some strings are passed without the \0 Null terminator
 */
void formatString(char * string, char* new_string )
{

}

/*
 * Function that determines if a file specified by pathname
 * exists, opens it and returns its pointer.
*/
Record* openFile(char *pathname) {
    Record* fileDir;

    if (( fileDir = getLastDir(pathname) ) == NULL)
    {
      printf("No such directory\n");
      return NULL;
    }

    // Goes through the file's dir to search for the file
    char* file_name = getFileName(pathname);
    short int  i;
    for (i=2; i < recordsPerDir; i ++)
    {
        // Found the file, now we must check if it's a link
        if (strncmp(file_name, fileDir[i].name, sizeof(file_name)) == 0)
        {
            // It's a bloody link, we read the link's content
            // and open the respective file
            if (fileDir[i].TypeVal == TYPEVAL_LINK) {
                char* linkContent = readCluster(fileDir[i].firstCluster);
                return openFile(linkContent);
            }
            else {
                return &fileDir[i];
            }
        }
    }
    printf("No such file\n");
    return NULL;
}

/*
 * This function returns the next free handle
 */
int getNextHandleNum(){
    int handle;
    for (handle=0; handle < MAX_OPEN_FILES; handle++)
    {
        if (opened_files_map[handle] == 0){
            return handle;
        }
    }

    return NO_FREE_HANDLES;
}

/*
 * Given a cluster number this functions reads it, allocates memory to store it
 * and returns its pointer.
 */
char * readCluster(int clusterNumber){
    char* cluster = malloc(clusterSize);
    BYTE readBuffer[SECTOR_SIZE];

    int i;
    for (i = 0; i < superblock.SectorsPerCluster; i++) {
        int adds = superblock.DataSectorStart + (clusterNumber * superblock.SectorsPerCluster);
        if (read_sector((adds + i), readBuffer) != 0) {
          printf("Error reading cluster %d\n", (adds + i));
          return NULL;
        }
        memcpy((cluster + (SECTOR_SIZE * i)), readBuffer, SECTOR_SIZE);
    }

    return cluster;
}

/*
 *  Writes a cluster on the disk.
 */
int writeCluster(BYTE *buffer, int clusterNumber) {
  int i;
  BYTE writeBuffer[SECTOR_SIZE];
  memset(writeBuffer, '\0', SECTOR_SIZE);

  for(i = 0; i < superblock.SectorsPerCluster; i++) {
    int adds = superblock.DataSectorStart + (clusterNumber * superblock.SectorsPerCluster);
    memcpy(writeBuffer, (buffer + (SECTOR_SIZE * i)), SECTOR_SIZE);
    if (write_sector((adds + i), writeBuffer) != 0) {
      return WRITE_ERROR;
    }
  }

  return FUNC_WORKING;
}

/*
 *  Function that returns the Record from a given file (dir, regular or link)
 */
Record *getFileRecord(char *path) {
  int isAbsolute = (*path == '/');

  Record *dir;
  Record *currentRecord = NULL;

  char *token = malloc(strlen(path) * sizeof(char));
  int tokenSize = sizeof(token);
  memset(token, '\0', tokenSize);
  strcpy(token, path);
  token = strtok(token, "/");

  if (isAbsolute) { // If the given path is ABSOLUTE, starts from the root dir
    dir = root;
  }
  else { // If it is a RELATIVE path, starts from the cwd
    dir = cwd;
  }

  int i = 0, found = 0;
  // A NULL pointer is returned at the end of the string
  while(token != NULL) {
    // given the dir selected above (based on the path),
    // now we have to search for the dir record which name equals the token
    while(strncmp(dir[i].name, token, strlen(token)) != 0 && i < recordsPerDir) {
      i++;
    }
    found = 1;

    // if found something without reaching the end of the dir
    if(i<recordsPerDir) {
      // updates the currentRecord
      currentRecord = &dir[i];

      // check if the Record is a directory
      if(dir[i].TypeVal != TYPEVAL_DIRETORIO) {
        // if it is not, it means that we can't open it, so we can already return
        return currentRecord;
      }
      else{ // if it is a dir
        token = strtok(NULL, "/");
        if (token != NULL) { // and there is more in the path
          // then reads the cluster and continues
          dir = (Record *) readCluster(dir[i].firstCluster);
        }
        else { // else, if that was the whole path, just returns the Record
          return currentRecord;
        }
      }
    }
    else {
      return NULL; // dir not found inside the current dir
    }
  }

  free(token);
  // if the first strtok results in NULL, it means that the desired dir
  // is the root dir, or the path was empty
  if (found == 0 && isAbsolute) {
    printf("Can not get the given Record\n");
    return dir;
  }
  else {
    return NULL;
  }
}

/*
 * Given a dir Record, this function reads its entries
 * and returns them in a DIRENT format.
 */
DIRENT2* getDirEnt(Record* dir)
{
    /*BYTE buffer[SECTOR_SIZE];
    Record * dir_ent = malloc(clusterSize);

    int i;
    for (i = 0; i < superblock.SectorsPerCluster; i++) {
    int adds = superblock.DataSectorStart + (dir->firstCluster * superblock.SectorsPerCluster);
    if (read_sector((adds + i), buffer) != 0) {
      return NULL;
    }
    memcpy((dir_ent + (recordsPerSector * i)), buffer, SECTOR_SIZE);
    }

    return dir_ent;*/

    return NULL;
}

/*
 * Function that iterates over the path to get the file name
 * (removing the path)
 */
char *getFileName(char *path) {
    char* pathCopy = strdup(path);

    char *last = strrchr(pathCopy, '/');
    if (last != NULL && (last + 1) != '\0')
    {

        return (last + 1);
    } else {
        return NULL;
    }

    return NULL;
}

/*
 *  Function that prints on the screen the informed dir.
 */
void ls(Record *dir) {
  int i = 0;
  while(i < recordsPerDir) {
    if (dir[i].TypeVal != TYPEVAL_INVALIDO) {
      printf("**********************************************************\n");
      printf("Type: %x\nName: %s\nSize (bytes): %d\nSize (clusters): %d\nFirst cluster: %d\n", dir[i].TypeVal, dir[i].name, dir[i].bytesFileSize, dir[i].clustersFileSize, dir[i].firstCluster);
    }
    i++;
  }
}

/*
 *  Function that reads the sectors of a Directory
 */
int readDir(Record *dir) {
  BYTE buffer[SECTOR_SIZE];
  ls(dir);
  int i;
  for (i = 0; i < superblock.SectorsPerCluster; i++) {
    int adds = superblock.DataSectorStart + (dir->firstCluster * superblock.SectorsPerCluster);
    if (read_sector((adds + i), buffer) != 0) {
      return READ_ERROR;
    }
    memcpy((dir + (recordsPerSector * i)), buffer, SECTOR_SIZE);
  }

  return FUNC_WORKING;
}


int isValidDirEntry(BYTE typeVal){

    if (typeVal == TYPEVAL_DIRETORIO || typeVal == TYPEVAL_REGULAR || typeVal == TYPEVAL_LINK)
    {
        return VALID_TYPE;
    }

    return NOT_VALID_TYPE;

}

