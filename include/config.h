/*
 *  Arquivo para conter códigos de erro,
 *  valores de prioridade,
 *  estados das threads
 */

#ifndef __cconfig__
#define __cconfig__

// CÓDIGOS DE ERRO
// aparentemente, ele quer valores negativos pra erro
#define FUNC_WORKING 0
#define VALID_TYPE 0
#define FUNC_NOT_IMPLEMENTED -1
#define FUNC_NOT_WORKING -2
#define INSUFICIENT_SIZE -3
#define READ_ERROR -4
#define CREATE_FILE_ERROR -5
#define OPEN_ERROR -6
#define NO_SUCH_FILE -7
#define NO_FREE_HANDLES -8
#define NO_FREE_INDEXES -9
#define NOT_VALID_TYPE -10
#define WRITE_ERROR -10
#define CH_ERROR -11

#define MAX_OPEN_FILES 10

#define FAT_FREE_CLUSTER 0x00000000
#define FAT_BAD_CLUSTER 0xFFFFFFFE
#define FAT_EOF 0xFFFFFFFF

#endif
