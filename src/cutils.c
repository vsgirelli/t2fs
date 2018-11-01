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
  
	//char    id[4];          	/* Identificação do sistema de arquivo. É formado pelas letras T2FS. */
	//WORD    version;        	/* Versão atual desse sistema de arquivos: (valor fixo 0x7E2=2018; 2=2º semestre). */
	//WORD    superblockSize; 	/* Quantidade de setores lógicos que formam o superbloco. (fixo em 1 setor) */
	//DWORD	DiskSize;			/* Tamanho total, em bytes, da partição T2FS. Inclui o superbloco, a área de FAT e os clusters de dados. */
	//DWORD	NofSectors;			/* Quantidade total de setores lógicos da partição T2FS. Inclui o superbloco, a área de FAT e os clusters de dados. */
	//DWORD	SectorsPerCluster;	/* Número de setores lógicos que formam um cluster. */
	//DWORD	pFATSectorStart;	/* Número do setor lógico onde a FAT inicia. */
	//DWORD	RootDirCluster;		/* Cluster onde inicia o arquivo correspon-dente ao diretório raiz */
	//DWORD	DataSectorStart;	/* Primeiro setor lógico da área de blocos de dados (cluster 0). */

  // since the ID uses 4 bytes, copy 4 bytes from the buffer
  strncpy(superblock.id, (char *)buffer, 4);
  superblock.version = (buffer + 4);
  printf("superblock.version: %d\n", superblock.version);

  


  return FUNC_NOT_WORKING;
}

/*
 *  Function that checks if the informed path exists.
 *  Uses strtok to generate the tokens, and opens the dir's cluster to
 *  check the file's records inside the dir.
 */
char* checkPath(char *path) {
  int isAbsolute = (*path == '/');

  //readSuperblock();

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
