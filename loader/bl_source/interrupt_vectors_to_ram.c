/*
 * interrupt_vectors_to_ram.c
 *
 * Heimir Thor Sverrisson, w1ant, heimir.sverrisson@gmail.com, February 2023
 *
 * Fill the the SRAM interrupt table with current interrupt service routines.
 *
 * This method is described in TI Application Note spna236 - March 2017
 *
 */

#include <interrupt_vectors_to_ram.h>

// Exception handler implementations - if needed
void undef_handler(void){
	return;
}
void swi_handler(void){
	return;
}
void pabt_handler(void){
	return;
}
void dabt_handler(void){
	return;
}
