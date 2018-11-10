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

  Record *dir;
  dir = getLastDir(filename);
  //ls(dir);

  // filehandler
  FILE2 file = 0;

  char *name = getFileName(filename);
  printf("FILENAME: %s\n", name);
  /*
   *  Após achar o path, e verificar que no cwd não há um registro de arquivo
   *  com mesmo nome do arquivo que se deseja criar, então cria-se o arquivo.
   *  Para criar o arquivo é necessário encontrar um cluster que esteja livre.
   *  Isso pode ser feito acessando a FAT e verificando algum cluster livre.
   *  Como é enderecamento encadeado, posso pegar qualquer um, pode ser o
   *  primeiro. Assim, requisito a apidisk a leitura do cluster x que está livre
   *  na FAT, altero a entrada da FAT, e devo criar um registro no cwd.
   *  Lembrar do current_pointer setado em 0.
   */
  if (!file) {
    return CREATE_FILE_ERROR;
  }
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


  return FUNC_NOT_WORKING;
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

  return FUNC_NOT_WORKING;
}


/*-----------------------------------------------------------------------------
Função:	Fecha o arquivo identificado pelo parâmetro "handle".

Entra:	handle -> identificador do arquivo a ser fechado

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int close2 (FILE2 handle) {
  initT2fs();

  return FUNC_NOT_WORKING;
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

  return FUNC_NOT_WORKING;
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

  return FUNC_NOT_WORKING;
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

  // TODO primeiras funcs a serem feitas
  return FUNC_NOT_WORKING;
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

  // TODO primeiras funcs a serem feitas
  return FUNC_NOT_WORKING;
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
        return OPEN_ERROR;
    }

    numberOfOpenedFiles += 1;
    int DIR_HANDLE;
    /*
        Allocate a struct of oFile type to represent this opened file
    */

    oFile openedFile;

    openedFile.curr_pointer =  0;
    openedFile.frecord = openedRecord;

    DIR_HANDLE = getNextHandleNum();

    opened_files[DIR_HANDLE] = openedFile;

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

  // TODO primeiras funcs a serem feitas
  return FUNC_NOT_WORKING;
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

  return FUNC_NOT_WORKING;
}

