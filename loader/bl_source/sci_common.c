//*****************************************************************************
//
// sci_common.c: Common used SCI functions
// Author      : QJ Wang. qjwang@ti.com
// Date        : 9-19-2012
//
// Copyright (c) 2006-2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
//*****************************************************************************

#include "bl_config.h"
#include "sci_common.h"
#include "sci.h"
#include "string.h"
#include "bl_input_queue.h"
#include "het.h"
#include "bl_watchdog.h"

#if defined (UART_ENABLE_UPDATE)

/**
 * @brief  Convert an Integer to a string
 * @param  str: The string
 * @param  intnum: The intger to be converted
 * @retval None
 */
void Int2Str(char *str, int intnum) {
	uint32_t i, Div = 1000000000, j = 0, Status = 0;

	for (i = 0; i < 10; i++) {
		str[j++] = (intnum / Div) + 48;

		intnum = intnum % Div;
		Div /= 10;
		if ((str[j - 1] == '0') & (Status == 0)) {
			j = 0;
		} else {
			Status++;
		}
	}
}

/**
 * @brief  Convert a string to an integer
 * @param  inputstr: The string to be converted
 * @param  intnum: The integer value
 * @retval 1: Correct
 *         0: Error
 */
uint32_t Str2Int(unsigned char *inputstr, int *intnum) {
	uint32_t i = 0, res = 0;
	uint32_t val = 0;

	if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X')) {
		if (inputstr[2] == '\0') {
			return 0;
		}
		for (i = 2; i < 11; i++) {
			if (inputstr[i] == '\0') {
				*intnum = val;
				/* return 1; */
				res = 1;
				break;
			}
			if (ISVALIDHEX(inputstr[i])) {
				val = (val << 4) + CONVERTHEX(inputstr[i]);
			} else {
				/* return 0, Invalid input */
				res = 0;
				break;
			}
		}
		/* over 8 digit hex --invalid */
		if (i >= 11) {
			res = 0;
		}
	} else /* max 10-digit decimal input */
	{
		for (i = 0; i < 11; i++) {
			if (inputstr[i] == '\0') {
				*intnum = val;
				/* return 1 */
				res = 1;
				break;
			} else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0)) {
				val = val << 10;
				*intnum = val;
				res = 1;
				break;
			} else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0)) {
				val = val << 20;
				*intnum = val;
				res = 1;
				break;
			} else if (ISVALIDDEC(inputstr[i])) {
				val = val * 10 + CONVERTDEC(inputstr[i]);
			} else {
				/* return 0, Invalid input */
				res = 0;
				break;
			}
		}
		/* Over 10 digit decimal --invalid */
		if (i >= 11) {
			res = 0;
		}
	}

	return res;
}

/* Return the next character on the serial line,
 * block until one is ready
 */
char UART_getKey(sciBASE_t *sci) {
	while(getQueueSize() == 0){
	    kickWatchdog();
	}
		;
	return dequeue();
}

/*
 * Get a character from serial line, if one is available within timeout,
 * otherwise return -1
 */
int UART_getChar(sciBASE_t *sci, uint32_t timeout) {
	while(timeout-- > 0){
	    kickWatchdog();
		if(getQueueSize() > 0)
			return dequeue();
	}
	return(-1);
}

const char* U32toStr(uint32_t val) {
	/* Maximum number of decimal digits in u32 is 10 */
	static char num_str[11];
	int pos = 10;
	num_str[10] = 0;

	if (val == 0) {
		/* If already zero then just return zero */
		return "0";
	}

	while ((val != 0) && (pos > 0)) {
		num_str[--pos] = (val % 10) + '0';
		val /= 10;
	}

	return &num_str[pos];
}

#endif

void UART_send32BitData(sciBASE_t *sci, uint32_t data) {
	uint8_t c_get;
	volatile int i = 0;
	for (i = 8; i > 0; i--) {
		c_get = (data >> 28) & 15;
		if (c_get > 9)
			c_get += 7;
		c_get += 48;
		UART_txByte(sci, c_get);
		data = data << 4;
	}
}

/**
 * @brief  Print a string on the HyperTerminal
 * @param  s: The string to be printed
 * @retval None
 */
void UART_putString(sciBASE_t *sci, const char *s) {
	while (*s != '\0') {
		UART_putChar(sci, *s);
		s++;
	}
}

/**
 * @brief  Print a character on the HyperTerminal
 * @param  c: The character to be printed
 * @retval None
 */
void UART_putChar(sciBASE_t *sci, char c) {
#ifndef USE_N2HET
	while ((sci->FLR & (uint32)SCI_TX_INT) == 0)
		;
	sci->TD = c;
#else
	kickWatchdog();
	HetUART1PutChar(c);
#endif
}

/**
 * @brief  Send a byte
 * @param  c: Character
 * @retval 0: Byte sent
 */
uint32_t UART_txByte(sciBASE_t *sci, char c) {
    kickWatchdog();
#ifndef USE_N2HET
	while ((sci->FLR & (uint32)SCI_TX_INT) == 0)
		;
	sci->TD = c;
#else
	HetUART1PutChar(c);
#endif
	return 0;
}

void HetUART1PutChar(char data)
{
  unsigned int Tmp = data;

  Tmp <<= 1;                                    // Shift in start bit (0)
  Tmp |= 0x00000200;                            // Add stop bit (1)
  while(hetRAM1->Instruction[2].Data != 0)		// Wait for previous character to be sent
	  ;
  hetRAM1->Instruction[4].Data = Tmp << 7;  // Load TX buffer
  hetRAM1->Instruction[2].Data =  10 << 7;  // Load bit count
}
