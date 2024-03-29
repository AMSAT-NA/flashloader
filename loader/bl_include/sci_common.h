/**
  ******************************************************************************
  * @file    common.h 
  * @author  Safety MCU Application Team
  * @version V1.0
  * @date    10/1/2012
  * @brief   This file provides all the headers of the common functions.
  ******************************************************************************
  */ 

#ifndef _SCI_COMMON_H
#define _SCI_COMMON_H

#include "bl_ymodem.h"
#include "bl_config.h"
#include "sci.h"

typedef  void (*pFunction)(void);

#define CMD_STRING_SIZE       128
/* Common routines */
#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

#define CRLF "\r\n"

/* Exported functions ------------------------------------------------------- */
void Int2Str(char* str,int intnum);
uint32_t Str2Int(unsigned char *inputstr,int *intnum);
uint32_t UART_getInteger(sciBASE_t *sci, int * num);
char UART_getKey(sciBASE_t *sci);
void UART_putChar(sciBASE_t *sci, char c);
void UART_putString(sciBASE_t *sci, const char *s);
//int UART_rxByte (sciBASE_t *sci, char *c, uint32_t timeout);
uint32_t UART_txByte (sciBASE_t *sci, char c);
int UART_getChar(sciBASE_t *sci, uint32_t timeout);
const char *U32toStr(uint32_t val);
void UART_send32BitData(sciBASE_t *sci, uint32_t data);
void HetUART1PutChar(char data);

#endif  /* _SCI_COMMON_H */
