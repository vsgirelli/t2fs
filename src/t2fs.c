/*
 *  Source code for the libt2fs library, implemented as task II
 *  on the course of Operational Systems, at INF - UFRGS.
 *  2018-2
 *  Authors:
 *  Leandro Pereira
 *  Pedro Ceriotti Trindade
 *  Valéria S. Girelli
 */

#include "../include/cutils.h"

/*-----------------------------------------------------------------------------
Função: Usada para identificar os desenvolvedores do T2FS.
	Essa função copia um string de identificação para o ponteiro indicado por "name".
	Essa cópia não pode exceder o tamanho do buffer, informado pelo parâmetro "size".
	O string deve ser formado apenas por caracteres ASCII (Valores entre 0x20 e 0x7A) e terminado por \0.
	O string deve conter o nome e número do cartão dos participantes do grupo.

Entra:	name -> buffer onde colocar o string de identificação.
	size -> tamanho do buffer "name" (número máximo de bytes a serem copiados).

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int identify2 (char *name, int size) {
  // 00 representa o null character
  // char* componentes = "4C 65 61 6E 64 72 6F 20 50 65 72 65 69 72 61 20 2D 20 30 30 32 37 33 31 31 34 5C 6E 50 65 64 72 6F 20 54 72 69 6E 64 61 64 65 20 2D 20 30 30 32 36 34 38 34 36 5C 6E 56 61 6C 65 72 69 61 20 47 69 72 65 6C 6C 69 20 2D 20 30 30 32 36 31 35 39 36 00";

  initT2fs();
  char *componentes = "Leandro Pereira - 00273114\nPedro Trindade - 00264846\nVal�ria S. Girelli - 00261596\n";

  if (strlen(componentes) > size)
  {
    return INSUFICIENT_SIZE;
  }

  strcpy(name, componentes);

  return FUNC_WORKING;
}

/*-----------------------------------------------------------------------------
Função: Criar um novo arquivo e abrir ele.
	O nome desse novo arquivo é aquele informado pelo parâmetro "filename".
	O contador de posição do arquivo (current pointer) deve ser colocado na posição zero.
	Caso já exista um arquivo ou diretório com o mesmo nome, a função deverá retornar um erro de criação.
	A função deve retornar o identificador (handle) do arquivo.
	Esse handle será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do arquivo criado.

Entra:	filename -> nome do arquivo a ser criado.

Saída:	Se a operação foi realizada com sucesso, a função retorna o handle do arquivo (número positivo).
	Em caso de erro, deve ser retornado um valor negativo.

  - Observaões:
  * O path pode ser absoluto ou relativo.
-----------------------------------------------------------------------------*/
FILE2 create2 (char *filename) {
  initT2fs();
  Record *dir = getLastDir(filename);

  // filehandler
  FILE2 file = 0;

  char *name = getFileName(filename);

  // check if the file already exists
  int i;
  for (i = 0; i < recordsPerDir; i++) {
    if (strncmp(name, dir[i].name, strlen(name)) == 0) {
      printf("File already exists\n");
      return CREATE_FILE_ERROR;
    }
  }

  // finds a invalid Record on the dir where the file can be allocated
  i = 0;
  while (dir[i].TypeVal != TYPEVAL_INVALIDO) {
    i++;
  }
  if (i > recordsPerDir) {
    printf("Directory already full\n");
    return CREATE_FILE_ERROR;
  }

  // creating a Record for the file
  Record frecord;
  frecord.TypeVal = TYPEVAL_REGULAR;
  strcpy(frecord.name, name);
  // when a new file is created, one sector must be allocated. Thus, the inital
  // file size, in bytes, is the cluster size, even if the file is empty.
  frecord.bytesFileSize = clusterSize;
  frecord.clustersFileSize = 1;
  frecord.firstCluster = getNextFreeFATIndex();
  FAT[frecord.firstCluster] = FAT_EOF;

  // Allocates the Record on the dir and writes it to the disk
  dir[i] = frecord;

  if (writeFAT() != FUNC_WORKING) {
    return WRITE_ERROR;
  }
  if (writeCluster((BYTE *)dir, dir[0].firstCluster) != FUNC_WORKING) {
    return WRITE_ERROR;
  }

  // open the file and allocates it in the opened_files array
  file = open2(filename);

  if (file == OPEN_ERROR) {
    return CREATE_FILE_ERROR;
  }

  printf("File created successfully\n");
  return file;
}

/*-----------------------------------------------------------------------------
Função:	Apagar um arquivo do disco.
	O nome do arquivo a ser apagado é aquele informado pelo parâmetro "filename".

Entra:	filename -> nome do arquivo a ser apagado.

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int delete2 (char *filename) {
  initT2fs();

  // get the file name to delete
  char *name = getFileName(filename);
  Record *parent = getLastDir(filename);

  int i = 0;
  // search for the file in the dir
  while (strncmp(parent[i].name, name, strlen(name)) != 0 && i < recordsPerDir) {
    i++;
  }

  // if found the file inside its parent
  if (i < recordsPerDir) {
    // search for and free all the file's clusters
    DWORD cluster = parent[i].firstCluster;
    DWORD nextCluster = 0;

    while (FAT[cluster] != FAT_EOF) {
      // saves the pointer to the next file cluster
      nextCluster = FAT[cluster];
      // free the current cluster
      FAT[cluster] = FAT_FREE_CLUSTER;
      cluster = nextCluster;
    }
    FAT[cluster] = FAT_FREE_CLUSTER;

    // transform the file Record into a invalid one
    parent[i].TypeVal = TYPEVAL_INVALIDO;
    //int nameSize = sizeof(parent[i].name);
    memset(parent[i].name, '\0', 51);
    parent[i].bytesFileSize = 0;
    parent[i].clustersFileSize = 0;
    parent[i].firstCluster = 0;

    if (writeFAT() != FUNC_WORKING) {
      return WRITE_ERROR;
    }
    if (writeCluster((BYTE *)parent, parent[0].firstCluster) != FUNC_WORKING) {
      return WRITE_ERROR;
    }
  }
  else {
    printf("File does not exist.\n");
    return NO_SUCH_FILE;
  }

  ls(parent);
  printf("File deleted successfully\n");
  return FUNC_WORKING;
}


/*-----------------------------------------------------------------------------
Função:	Abre um arquivo existente no disco.
	O nome desse novo arquivo é aquele informado pelo parâmetro "filename".
	Ao abrir um arquivo, o contador de posição do arquivo (current pointer) deve ser colocado na posição zero.
	A função deve retornar o identificador (handle) do arquivo.
	Esse handle será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do arquivo criado.
	Todos os arquivos abertos por esta chamada são abertos em leitura e em escrita.
	O ponto em que a leitura, ou escrita, será realizada é fornecido pelo valor current_pointer (ver função seek2).

Entra:	filename -> nome do arquivo a ser apagado.

Saída:	Se a operação foi realizada com sucesso, a função retorna o handle do arquivo (número positivo)
	Em caso de erro, deve ser retornado um valor negativo
-----------------------------------------------------------------------------*/
FILE2 open2 (char *filename) {
    initT2fs();

    ls(root);
    ls(&root[8]);
    if (numberOfOpenedFiles == 10){
    // Maximum number of files opened
        printf("Maximum number of opened files reached");
        return OPEN_ERROR;
    }

    Record* openedRecord;

    if ( (openedRecord = openFile(filename)) == NULL)
    {
        return OPEN_ERROR;
    }

    if ( openedRecord->TypeVal != TYPEVAL_REGULAR)
    {
        printf("File is not regular\n");
        return OPEN_ERROR;
    }


    numberOfOpenedFiles += 1;
    FILE2 FILE_HANDLE;
    /*
        Allocate a struct of oFile type to represent this opened file
    */

    oFile openedFile;

    openedFile.curr_pointer =  0;
    openedFile.frecord = openedRecord;

    //printf("fileName: %s\n", openedRecord->name);
    FILE_HANDLE = getNextHandleNum();

    opened_files[FILE_HANDLE] = openedFile;
    opened_files_map[FILE_HANDLE] = 1;

    return FILE_HANDLE;
}


/*-----------------------------------------------------------------------------
Função:	Fecha o arquivo identificado pelo parâmetro "handle".

Entra:	handle -> identificador do arquivo a ser fechado

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int close2 (FILE2 handle) {
    initT2fs();

    if (handle > 0 && handle < MAX_OPEN_FILES)
    {
        return NO_SUCH_FILE;
    }
    if (opened_files_map[handle] == 0)
    {
        return NO_SUCH_FILE;
    }

    opened_files_map[handle] = 1;

    return FUNC_WORKING;
}



/*-----------------------------------------------------------------------------
Função:	Realiza a leitura de "size" bytes do arquivo identificado por "handle".
	Os bytes lidos são colocados na área apontada por "buffer".
	Após a leitura, o contador de posição (current pointer) deve ser ajustado para o byte seguinte ao último lido.

Entra:	handle -> identificador do arquivo a ser lido
	buffer -> buffer onde colocar os bytes lidos do arquivo
	size -> número de bytes a serem lidos

Saída:	Se a operação foi realizada com sucesso, a função retorna o número de bytes lidos.
	Se o valor retornado for menor do que "size", então o contador de posição atingiu o final do arquivo.
	Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
int read2 (FILE2 handle, char *buffer, int size) {
  initT2fs();
//Record *record = getLastDir("dir1/file");
//ls(record);

  //printf("%d", opened_files_map[handle]);

  // check if it is opened
  if(opened_files_map[handle] == 0)
    return READ_ERROR;

  Record *rec = opened_files[handle].frecord;
  // trying to read a size greater than the record
  if(size + opened_files[handle].curr_pointer > rec->bytesFileSize)
    return READ_ERROR;

  // get first cluster
  DWORD clusterToRead = rec->firstCluster;

  if(FAT[clusterToRead] == FAT_BAD_CLUSTER)
    return READ_ERROR;

  // int division -> return flow
  // don't need to round up, beacause we are reading the first one out side the for
  int numberOfclusterToRead = size / clusterSize;
  //printf("\n numero de cluster para ler: %d", numberOfclusterToRead);

  // get the first cluster according to the curr_pointer, skipping when size is greater than cluster size
  int numberOfClusterToSkip = opened_files[handle].curr_pointer / clusterSize;
  int j;
  for(j = 0; j < numberOfClusterToSkip; j++){
    clusterToRead = FAT[clusterToRead];
    if(FAT[clusterToRead] == FAT_BAD_CLUSTER)
      return READ_ERROR;
  }
 // printf("\n numero de cluster para ler: %d", numberOfClusterToSkip);

  // read the first cluster out of the for
  char *clusterVal = readCluster(clusterToRead);
  // indicates how many bytes inside the cluster must be skiped due to the curr_pointer
  int bytesToSkip = (opened_files[handle].curr_pointer - (numberOfClusterToSkip * clusterSize)) - 1;
  if (bytesToSkip < 0 )
    bytesToSkip = 0;

  // count how many bytes are left on the current cluster
  int bytesLeftInCluster = (clusterSize - bytesToSkip + 1);
  if (bytesLeftInCluster > clusterSize)
    bytesLeftInCluster = clusterSize;

  if (size < bytesLeftInCluster) {
    memcpy(buffer, (clusterVal + bytesToSkip), size);
    return FUNC_WORKING;
  }
  memcpy(buffer, (clusterVal + bytesToSkip), bytesLeftInCluster);

  // from the size bytes, already read the bytes above
  int bytesLeftToRead = size - bytesLeftInCluster;

  int i;
  clusterToRead = FAT[clusterToRead];
  if(FAT[clusterToRead] == FAT_BAD_CLUSTER)
    return READ_ERROR;

  for (i = 1; i <= numberOfclusterToRead - numberOfClusterToSkip; i++){
    clusterVal = readCluster(clusterToRead);
    // if it is the last cluster, copy just the necessary size
    if(numberOfclusterToRead - numberOfClusterToSkip == i){
      memcpy(buffer + (clusterSize*i), clusterVal, bytesLeftToRead);
    }
    else {
      memcpy(buffer + (clusterSize*i), clusterVal, clusterSize);
      bytesLeftToRead -= clusterSize;
    }

    if(FAT[clusterToRead] == FAT_EOF)
      break;
    if(FAT[clusterToRead] == FAT_BAD_CLUSTER)
      return READ_ERROR;
    clusterToRead = FAT[clusterToRead];
  }

  // update current pointer
  opened_files[handle].curr_pointer += size;

  return FUNC_WORKING;
}



/*-----------------------------------------------------------------------------
Função:	Realiza a escrita de "size" bytes no arquivo identificado por "handle".
	Os bytes a serem escritos estão na área apontada por "buffer".
	Após a escrita, o contador de posição (current pointer) deve ser ajustado para o byte seguinte ao último escrito.

Entra:	handle -> identificador do arquivo a ser escrito
	buffer -> buffer de onde pegar os bytes a serem escritos no arquivo
	size -> número de bytes a serem escritos

Saída:	Se a operação foi realizada com sucesso, a função retorna o número de bytes efetivamente escritos.
	Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
int write2 (FILE2 handle, char *buffer, int size) {
  initT2fs();

  return FUNC_NOT_WORKING;
}



/*-----------------------------------------------------------------------------
Função:	Função usada para truncar um arquivo.
	Remove do arquivo todos os bytes a partir da posição atual do contador de posição (CP)
	Todos os bytes a partir da posição CP (inclusive) serão removidos do arquivo.
	Após a operação, o arquivo deverá contar com CP bytes e o ponteiro estará no final do arquivo

Entra:	handle -> identificador do arquivo a ser truncado

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int truncate2 (FILE2 handle) {
  initT2fs();

  return FUNC_NOT_WORKING;
}



/*-----------------------------------------------------------------------------
Função:	Reposiciona o contador de posições (current pointer) do arquivo identificado por "handle".
	A nova posição é determinada pelo parâmetro "offset".
	O parâmetro "offset" corresponde ao deslocamento, em bytes, contados a partir do início do arquivo.
	Se o valor de "offset" for "-1", o current_pointer deverá ser posicionado no byte seguinte ao final do arquivo,
		Isso é útil para permitir que novos dados sejam adicionados no final de um arquivo já existente.

Entra:	handle -> identificador do arquivo a ser escrito
	offset -> deslocamento, em bytes, onde posicionar o "current pointer".

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int seek2 (FILE2 handle, DWORD offset) {
  initT2fs();

  return FUNC_NOT_WORKING;
}



/*-----------------------------------------------------------------------------
Função:	Criar um novo diretório.
	O caminho desse novo diretório é aquele informado pelo parâmetro "pathname".
		O caminho pode ser ser absoluto ou relativo.
	São considerados erros de criação quaisquer situações em que o diretório não possa ser criado.
		Isso inclui a existência de um arquivo ou diretório com o mesmo "pathname".

Entra:	pathname -> caminho do diretório a ser criado

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int mkdir2 (char *pathname) {
  initT2fs();

  // dir in which the user wants to create the new directory
  Record *dir = getLastDir(pathname);

  char *name = getFileName(pathname);

  // check if the directory already exists in dir
  int i;
  for (i = 0; i < recordsPerDir; i++) {
    if (strncmp(name, dir[i].name, strlen(name)) == 0) {
      printf("Directory already exists\n");
      return CREATE_FILE_ERROR;
    }
  }

  // finds a invalid Record on the dir where the file can be allocated
  i = 0;
  while (dir[i].TypeVal != TYPEVAL_INVALIDO) {
    i++;
  }
  if (i > recordsPerDir) {
    printf("Directory already full\n");
    return CREATE_FILE_ERROR;
  }

  // creating a Record for the new directory
  Record drecord;
  drecord.TypeVal = TYPEVAL_DIRETORIO;
  strcpy(drecord.name, name);
  // when a new dir is created, one sector must be allocated.
  drecord.bytesFileSize = clusterSize;
  drecord.clustersFileSize = 1;
  drecord.firstCluster = getNextFreeFATIndex();
  FAT[drecord.firstCluster] = FAT_EOF;

  // Allocates the Record on the dir and writes it to the disk
  // (only the information about the new dir's record)
  dir[i] = drecord;
  if (writeFAT() != FUNC_WORKING) {
    return WRITE_ERROR;
  }
  if (writeCluster((BYTE *)dir, dir[0].firstCluster) != FUNC_WORKING) {
    return WRITE_ERROR;
  }

  // Now we need to add the . and the .. directories inside the new dir
  // Creating the . and .. entries on the new dir
  Record this;
  this.TypeVal = TYPEVAL_DIRETORIO;
  char thisChar[] = ".\0";
  strcpy(this.name, thisChar);
  this.bytesFileSize = clusterSize;
  this.clustersFileSize = 1;
  // points to it's own firstCluster
  this.firstCluster = drecord.firstCluster;

  Record parent;
  parent.TypeVal = TYPEVAL_DIRETORIO;
  char parentChar[] = "..\0";
  strcpy(parent.name, parentChar);
  parent.bytesFileSize = clusterSize;
  parent.clustersFileSize = 1;
  // points to the parent's firstCluster
  parent.firstCluster = dir[0].firstCluster;

  Record *newDir = malloc(clusterSize); 
  newDir[0] = this;
  newDir[1] = parent;

  // make sure the other entries are invalid and clean
  Record nullRecord;
  nullRecord.TypeVal = TYPEVAL_INVALIDO;
  memset(nullRecord.name, '\0', 51);
  nullRecord.bytesFileSize = 0;
  nullRecord.clustersFileSize = 0;
  nullRecord.firstCluster = 0;
  // sets a null record for all the remain records inside the new dir
  for (i = 2; i < recordsPerDir; i++) {
    newDir[i] = nullRecord;
  }

  if (writeCluster((BYTE *)newDir, newDir[0].firstCluster) != FUNC_WORKING) {
    return WRITE_ERROR;
  }

  //ls(dir);
  //ls(newDir);
  printf("Directory created successfully\n");

  return FUNC_WORKING;
}



/*-----------------------------------------------------------------------------
Função:	Apagar um subdiretório do disco.
	O caminho do diretório a ser apagado é aquele informado pelo parâmetro "pathname".
	São considerados erros quaisquer situações que impeçam a operação.
		Isso inclui:
			(a) o diretório a ser removido não está vazio;
			(b) "pathname" não existente;
			(c) algum dos componentes do "pathname" não existe (caminho inválido);
			(d) o "pathname" indicado não é um diretório;

Entra:	pathname -> caminho do diretório a ser removido

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int rmdir2 (char *pathname) {
  initT2fs();
  // TODO primeiras funcs a serem feitas
  return FUNC_NOT_WORKING;
}



/*-----------------------------------------------------------------------------
Função:	Altera o diretório atual de trabalho (working directory).
		O caminho desse diretório é informado no parâmetro "pathname".
		São considerados erros:
			(a) qualquer situação que impeça a realização da operação
			(b) não existência do "pathname" informado.

Entra:	pathname -> caminho do novo diretório de trabalho.

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int chdir2 (char *pathname) {
    initT2fs();
    if (strcmp(pathname, "/") == 0) {
      cwd = root;
      strcpy(currentPath, "/");
      return FUNC_WORKING;
    }

    Record* dir;

    if ( (dir = openFile(pathname)) == NULL)
    {
        printf("Invalid Dir\n");
        return CH_ERROR;
    }

    if ( dir->TypeVal != TYPEVAL_DIRETORIO && dir->TypeVal != TYPEVAL_LINK)
    {
        printf("File is not a directory\n");
        return CH_ERROR;
    }

    if (dir->TypeVal == TYPEVAL_LINK)
    {
        char* linkContent = readCluster(dir->firstCluster);
        return chdir2(linkContent);
    }


    if (*pathname == '/') {
      strncpy(currentPath, pathname, MAX_FILE_NAME_SIZE+1);
      fixPath(currentPath);
    }
    else {
      char path[MAX_FILE_NAME_SIZE * 2 + 2] = {0};
      strncpy(path, currentPath, MAX_FILE_NAME_SIZE + 1);
      strcat(path, pathname);

      fixPath(path);
      strncpy(currentPath, path, MAX_FILE_NAME_SIZE + 1);
    }

    cwd =  (Record *) readCluster(dir->firstCluster);

    return FUNC_WORKING;
}



/*-----------------------------------------------------------------------------
Função:	Informa o diretório atual de trabalho.
		O "pathname" do diretório de trabalho deve ser copiado para o buffer indicado por "pathname".
			Essa cópia não pode exceder o tamanho do buffer, informado pelo parâmetro "size".
		São considerados erros:
			(a) quaisquer situações que impeçam a realização da operação
			(b) espaço insuficiente no buffer "pathname", cujo tamanho está informado por "size".

Entra:	pathname -> buffer para onde copiar o pathname do diretório de trabalho
		size -> tamanho do buffer pathname

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int getcwd2 (char *pathname, int size) {
  initT2fs();

  if (size > MAX_FILE_NAME_SIZE) {
    return INSUFICIENT_SIZE;
  }

  strncpy(pathname, currentPath, size);

  return FUNC_WORKING;
}



/*-----------------------------------------------------------------------------
Função:	Abre um diretório existente no disco.
	O caminho desse diretório é aquele informado pelo parâmetro "pathname".
	Se a operação foi realizada com sucesso, a função:
		(a) deve retornar o identificador (handle) do diretório
		(b) deve posicionar o ponteiro de entradas (current entry) na primeira posição válida do diretório "pathname".
	O handle retornado será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do diretório.

Entra:	pathname -> caminho do diretório a ser aberto

Saída:	Se a operação foi realizada com sucesso, a função retorna o identificador do diretório (handle).
	Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
DIR2 opendir2 (char *pathname) {
    initT2fs();
    if (numberOfOpenedFiles == 10){
    // Maximum number of files opened
        printf("Maximum number of opened files reached");
        return OPEN_ERROR;
    }

    Record* openedRecord;

    if ( (openedRecord = openFile(pathname)) == NULL)
    {
        printf("Invalid Dir\n");
        return OPEN_ERROR;
    }

    if ( openedRecord->TypeVal != TYPEVAL_DIRETORIO)
    {
        printf("File is not a directory\n");
        return OPEN_ERROR;
    }

    numberOfOpenedFiles += 1;
    DIR2 DIR_HANDLE;
    /*
        Allocate a struct of oFile type to represent this opened file
    */

    oFile openedFile;

    openedFile.curr_pointer =  0;
    openedFile.frecord = openedRecord;

    DIR_HANDLE = getNextHandleNum();

    opened_files[DIR_HANDLE] = openedFile;
    opened_files_map[DIR_HANDLE] = 1;

    return DIR_HANDLE;
}



/*-----------------------------------------------------------------------------
Função:	Realiza a leitura das entradas do diretório identificado por "handle".
	A cada chamada da função é lida a entrada seguinte do diretório representado pelo identificador "handle".
	Algumas das informações dessas entradas serão colocadas no parâmetro "dentry".
	Após realizada a leitura de uma entrada, o ponteiro de entradas (current entry) deve ser ajustado para a próxima entrada válida, seguinte à última lida.
	São considerados erros:
		(a) qualquer situação que impeça a realização da operação
		(b) término das entradas válidas do diretório identificado por "handle".

Entra:	handle -> identificador do diretório cujas entradas deseja-se ler.
	dentry -> estrutura de dados onde a função coloca as informações da entrada lida.

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero ( e "dentry" não será válido)
-----------------------------------------------------------------------------*/
int readdir2 (DIR2 handle, DIRENT2 *dentry) {
    initT2fs();

    oFile dirRecord;

    dirRecord = opened_files[handle];

    Record* dirContent;

    dirContent =  (Record *) readCluster(dirRecord.frecord->firstCluster);

    long int i = dirRecord.curr_pointer;

    while ( i < recordsPerDir ){

        if ( isValidDirEntry(dirContent[i].TypeVal) == VALID_TYPE) {

            strncpy(dentry->name, dirContent[i].name, strlen(dirContent[i].name));
            dentry->name[strlen(dirContent[i].name)] = '\0';
            dentry->fileType = dirContent[i].TypeVal;
            dentry->fileSize = dirContent[i].bytesFileSize;
            opened_files[handle].curr_pointer = i + 1;
            return FUNC_WORKING;
        }

        i++;

    }

    return FUNC_NOT_WORKING;

}



/*-----------------------------------------------------------------------------
Função:	Fecha o diretório identificado pelo parâmetro "handle".

Entra:	handle -> identificador do diretório que se deseja fechar (encerrar a operação).

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int closedir2 (DIR2 handle) {
    initT2fs();

    if (opened_files_map[handle] == 0)
    {
        return NO_SUCH_FILE;
    }

    opened_files_map[handle] = 1;

    return FUNC_WORKING;
}



/*-----------------------------------------------------------------------------
Função:	Função usada para criar um caminho alternativo (softlink) com o nome dado por linkname (relativo ou absoluto) para um arquivo ou diretório fornecido por filename.

Entra:	linkname -> nome do link a ser criado
	filename -> nome do arquivo ou diretório apontado pelo link

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int ln2(char *linkname, char *filename) {
    initT2fs();

    Record * orig_file;
    Record * link_file;

    if ( (orig_file = openFile(filename)) == NULL)
    {
        printf("No such file\n");
        return NO_SUCH_FILE;
    }

    if ( orig_file->TypeVal != TYPEVAL_REGULAR && orig_file->TypeVal != TYPEVAL_DIRETORIO)
    {
        printf("Invalid link type\n");
        return INVALID_LINK_TYPE;

    }

    if ( strlen(filename) > clusterSize)
    {
        printf("Maximum filename reached\n");
        return INVALID_LINK_TYPE;
    }

    if (strlen(linkname) > MAX_FILE_NAME_SIZE)
    {
        printf("Link name exceeds maximum size\n");
        return INVALID_LINK_TYPE;
    }

    FILE2 link_handle = createFile(linkname, TYPEVAL_LINK);

    if (link_handle == CREATE_FILE_ERROR)
    {
        printf("Couldn't create the link\n");
        return CREATE_FILE_ERROR;
    }

    close2(link_handle);

    link_file = openFile(linkname);
    writeCluster( (BYTE *)filename, link_file->firstCluster);

    return FUNC_WORKING;
}

