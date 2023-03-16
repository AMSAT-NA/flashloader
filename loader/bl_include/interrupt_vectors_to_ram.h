/*
 * interrupt_vectors_to_ram.h
 *
 * Heimir Thor Sverrisson, w1ant, heimir.sverrisson@gmail.com, February 2023
 *
 * Fill the the SRAM interrupt table with current interrupt service routines.
 *
 * This method is described in TI Application Note spna236 - March 2017
 *
 */

#ifndef __INTERRUPT_VECTORS_TO_RAM_H__
#define __INTERRUPT_VECTORS_TO_RAM_H__

// Exception handler declarations
void undef_handler(void);
void swi_handler(void);
void pabt_handler(void);
void dabt_handler(void);

#endif
