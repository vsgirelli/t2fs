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
#define FUNC_NOT_IMPLEMENTED -1
#define FUNC_NOT_WORKING -2
#define INSUFICIENT_SIZE -3
#define READ_ERROR -4
#define CREATE_FILE_ERROR -5
#define OPEN_ERROR -6
#define NO_SUCH_FILE -7
#define NO_FREE_HANDLES 8

#define MAX_OPEN_FILES 10

#endif
