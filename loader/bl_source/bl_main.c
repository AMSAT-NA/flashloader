/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "sys_common.h"
#include "sci_common.h"
#include "bl_config.h"
#include "bl_uart.h"
#include "system.h"
#include "bl_check.h"
#include "sci.h"
#include "bl_input_queue.h"

/*****************************************************************************
 *
 * This holds the current remaining size in bytes to be downloaded.
 *
 ******************************************************************************/
uint32_t g_ulTransferSize;

/*****************************************************************************
 *
 * This holds the current address that is being written to during a download
 * command.
 *
 ******************************************************************************/
uint32_t g_ulTransferAddress;

/*****************************************************************************
 *
 * This is the data buffer used during transfers to the boot loader.
 *
 ******************************************************************************/
uint32_t g_pulDataBuffer[BUFFER_SIZE];

/*****************************************************************************
 *
 * This is the data buffer used for update status.
 *
 * g_pulUpdateSuccess[] are used to store application update status and application
 * image's version etc
 ******************************************************************************/

uint32_t g_pulUpdateSuccess[] =
		{ 0x5A5A5A5A, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
uint32_t g_ulUpdateStatusAddr = APP_STATUS_ADDRESS;

uint32_t g_ulUpdateBufferSize = 32; /*32 bytes or 8 32-bit words*/

#define E_PASS     		0
#define E_FAIL     		0x1U
#define E_TIMEOUT  		0x2U

/*****************************************************************************
 *
 * This is an specially aligned buffer pointer to g_pulDataBuffer to make
 * copying to the buffer simpler.  It must be offset to end on an address that
 * ends with 3.
 *
 ******************************************************************************/
uint8_t *g_pucDataBuffer;

static uint8 c;

void loader_main(void) {
	_enable_IRQ();
	/* Initialize SCI Routines to receive Command and transmit data */
	sciInit();
	sciSetBaudrate(UART, UART_BAUDRATE);
	initQueue();

	// Bring GIO out of reset before the call to CheckForceUpdate() below
	CheckGPIOForceUpdate();

	if (!CheckGPIOForceUpdate() && !CheckForceUpdate()) {
		g_ulTransferAddress = (uint32_t) APP_START_ADDRESS;
		((void (*)(void)) g_ulTransferAddress)();
	}
	sciReceive(UART, 1, &c); // As we are in Interrupt mode this call will not block
	UpdaterUART();
}

/**
 * This function fires for each character received
 */
void sciNotification(sciBASE_t *sci, uint32 flags)
{
	sciReceive(sci, 1, &c);
	enqueue(c);
}

void esmGroup1Notification(uint32 channel)
{
	return;
}

void esmGroup2Notification(uint32 channel)
{
	return;
}
