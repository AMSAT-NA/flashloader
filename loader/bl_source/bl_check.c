//*****************************************************************************
//
// bl_check.c: Code to check for a forced update.
// Author    : QJ Wang. qjwang@ti.com
// Date      : 9-19-2012
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

#include "gio.h"
#include "bl_config.h"
#include "bl_check.h"
#include "sci.h"
#include "het.h"
#include "bl_run_target.h"

//*****************************************************************************
//
//  bl_check_api
//
//*****************************************************************************

extern unsigned int g_pulUpdateSuccess[8];
extern unsigned int g_pulUpdateFail[8];
extern unsigned int g_ulUpdateStatusAddr;

#if (FORCED_UPDATE_PORT == GPIO_PORTA_BASE)
#define gioPORT gioPORTA
#else
#define gioPORT gioPORTB
#endif

bool isGPIOactive(void) {
#ifndef N2HET_INPUT_PIN
	/** bring GIO module out of reset */
	gioREG->GCR0 = 1;
	gioREG->ENACLR = 0xFF;
	gioREG->LVLCLR = 0xFF;

	// Set the pin as input
	gioPORT->DIR &= ~(1 << FORCED_UPDATE_PIN);

	// Check the pin to see if an update is being requested.

	if (!((gioPORT->DIN & (0x1 << FORCED_UPDATE_PIN)) == 0)) {
		return (1);
	}
#else	// We are using N2HET pin rather than GPIO
	hetREG1->DIR &= ~(1 << N2HET_INPUT_PIN);
	if (!((hetREG1->DIN & (0x1 << N2HET_INPUT_PIN)) == 0)) {
		return (true);
	}
#endif
	return (false);
}

// Return true if we should run the application
// false if we should stay in the flash loader
bool runApplication(void){
	enum target_value target;
	target = get_target();
	if(isGPIOactive()){
		switch(target){
			case FLASH_LOADER: return false;
			case APPLICATION:  return true;
			case UNKNOWN:      return false;
			default:		   return false;
		}
	} else {
		switch(target){
			case FLASH_LOADER: return false;
			case APPLICATION:  return true;
			case UNKNOWN:      return true;
			default:           return true;
		}
	}
}

bool isApplicationValid(void) {
	uint32_t *pulApp;

	// Read in the status area of the application
	pulApp = (uint32_t*) g_ulUpdateStatusAddr;

	if ((pulApp[0] == g_pulUpdateSuccess[0])) { // Pattern is 0x5A5A5A5A
		return (true);    //1 means that pattern is there
	}
    return(false);
}
