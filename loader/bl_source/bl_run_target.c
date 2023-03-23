//-------------------------------------------------------------------------------
// bl_run_target.c
//
// Heimir Thor Sverrisson, w1ant, heimir.sverrisson@gmail.com
//
// Code to set and read the run target value stored in SRAM
//
//-------------------------------------------------------------------------------

#include "bl_run_target.h"
#include "system.h"

// The patterns stored in RAM for each run target.
// Actually Unknown is anything but FLASH_LOADER or APPLICATION

#define FLASH_LOADER_PATTERN 0x55555555
#define APPLICATION_PATTERN  0xAAAAAAAA
#define UNKNOWN_PATTERN      0x00000000

uint32_t target_pattern[] = {
		FLASH_LOADER_PATTERN,
		APPLICATION_PATTERN,
		UNKNOWN_PATTERN
};

const char * get_target_name(enum target_value target){
	switch(target){
		case FLASH_LOADER: 	return "Flash loader";
		case APPLICATION:	return "Application";
		case UNKNOWN:		return "Unknown";
	}
	return "Unknown";
}

void set_target(enum target_value target){
	run_target = target_pattern[target];
}

enum target_value get_target(){
	switch(run_target){
		case FLASH_LOADER_PATTERN:
			return FLASH_LOADER;
		case APPLICATION_PATTERN:
			return APPLICATION;
		default:
			return UNKNOWN;
	}
}
int main(void);

void softReset(){
	systemREG1->SYSECR |= 0x8000; // Set the 15th bit to perform software reset interrupt
}

