//*****************************************************************************
//
// bl_config.h - The configurable parameters of the boot loader.
// Author    : QJ Wang. qjwang@ti.com

// Copyright (c) 2008-2011 Texas Instruments Incorporated.  All rights reserved.
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
//
//*****************************************************************************

#ifndef __BL_CONFIG_H__
#define __BL_CONFIG_H__

//#define TMS570LS12
#define TMS570LS0914

#define CRYSTAL_FREQ            16             //MHz
#define SYS_CLK_FREQ           160            //MHz

//*****************************************************************************
// Selects the UART as the port for communicating with the boot loader.
// Exclusive of: CAN_ENABLE_UPDATE, SPI_ENABLE_UPDATE,
// Requires: UART_FIXED_BAUDRATE, BUFFER_SIZE
//*****************************************************************************
#define UART_ENABLE_UPDATE

// Use the N2HET as serial port instead of the proper SCI
#define USE_N2HET

// Use N2HET pin for power sense

#define N2HET_INPUT_PIN 8

// Port and pin to use for watchdog
#define WATCHDOG_PORT spiPORT1
#define WATCHDOG_PIN 1
//#define WATCHDOG_TIMED_WAIT /*Undefine this turn off clock waits for hitting watchdog*/
#define MIN_WATCHDOG_TOGGLE 1000 /*How many clock ticks must we wait between toggles*/


//*****************************************************************************
// The starting address of the application.  This must be a multiple of 32K(sector size)
// bytes (making it aligned to a page boundary), and can not be 0 (the first sector is 
// boot loader). 
//
// The flash image of the boot loader must not be larger than this value.
//*****************************************************************************
#define APP_START_ADDRESS       0x00010100

//*****************************************************************************
// The address to store the update status of the application image
// It contains Application Start Address, Application Image Size, etc
//
//*****************************************************************************
#define APP_STATUS_ADDRESS       0x00010000

#define UART_BAUDRATE     115200

#ifndef USE_N2HET
    #define UART              scilinREG   /* Use UART port 1 for UART boot */
#else
	#define UART 0   // Not used for HET port
#endif

#define BUFFER_SIZE             64       /*words in the data buffer used for receiving packets*/

//*****************************************************************************
// Enables the pin-based forced update check.  When enabled, the boot loader
// will go into update mode instead of calling the application if a pin is read
// at a particular polarity, forcing an update operation.  In either case, the
// application is still able to return control to the boot loader in order to
// start an update. 
//
// Requires: FORCED_UPDATE_PERIPH, FORCED_UPDATE_PORT, FORCED_UPDATE_PIN,
//           FORCED_UPDATE_POLARITY
//*****************************************************************************
#define ENABLE_UPDATE_CHECK

#if defined (ENABLE_UPDATE_CHECK)
//*****************************************************************************
//
// The GPIO port to check for a forced update.  This will be one of the
// GPIO_PORTx_BASE values, where "x" is replaced with the port name (A or B).
// Depends on: ENABLE_UPDATE_CHECK
//*****************************************************************************
#define FORCED_UPDATE_PORT      GPIO_PORTA_BASE

//*****************************************************************************
// The pin to check for a forced update.  This is a value between 0 and 7.
//
// Depends on: ENABLE_UPDATE_CHECK
//*****************************************************************************
#define FORCED_UPDATE_PIN       7

#endif

// #define DEBUG_MSG
#endif // __BL_CONFIG_H__
