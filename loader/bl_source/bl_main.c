/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com 
* 
* 
*/

#include "bl_config.h"
#include "sys_common.h"
#include "sci_common.h"
#include "bl_uart.h"
#include "system.h"
#include "bl_check.h"
#include "sci.h"
#include "bl_input_queue.h"
#include "het.h"
#include "bl_watchdog.h"
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
    startCounter();
#ifndef USE_N2HET
	sciInit();
	sciSetBaudrate(UART, UART_BAUDRATE);
#else
	// Initialize N2HET and input queue
	hetInit();
#endif

	initQueue();
	asm(" cpsie i"); // Enable processor interrupts

#ifdef USE_N2HET
	// Clear all interrupts for N2HET
	hetREG1->GCR = 0x01030001;
    hetREG1->INTENAC = 0xFFFFFFFF;
    hetREG1->FLG = 0xFFFFFFFF;
#endif

	if (isApplicationValid() && runApplication()) {
		g_ulTransferAddress = (uint32_t) APP_START_ADDRESS;
		((void (*)(void)) g_ulTransferAddress)();
	}

#ifndef USE_N2HET
	sciReceive(UART, 1, &c); // As we are in Interrupt mode this call will not block
#else
	hetREG1->INTENAS = 1 << 23; // Enable HET Receive interrupt
#endif
	UpdaterUART();

    while(1);
}

/*
 * This is a Interrupt callback routine from the N2HET processor.
 * Offset refers to the line number in the HET program.
 * Instruction[25].Data is the 32-bit data field of the 96-bit instruction number 25,
 * which will contain the data byte received.
 */
void hetNotification(hetBASE_t *het, uint32 offset)
{
    char c;
    if(offset==24){
        c = hetRAM1->Instruction[25].Data;
        enqueue(c);
    }
}

/**
 * This function fires for each character received on SCI interface
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
