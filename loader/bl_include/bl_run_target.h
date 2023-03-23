//-------------------------------------------------------------------------------
// bl_run_target.h
//
// Heimir Thor Sverrisson, w1ant, heimir.sverrisson@gmail.com
//
// Definitions to set and read the run target value stored in SRAM
//
//-------------------------------------------------------------------------------

#ifndef __BL_RUN_TARGET_H__
#define __BL_RUN_TARGET_H__

#include "sys_common.h"

extern int run_target;  // Defined in run_target_address.asm

enum target_value {
	FLASH_LOADER,
	APPLICATION,
	UNKNOWN
};

const char * get_target_name(enum target_value target);

void set_target(enum target_value target);

enum target_value get_target();

void softReset();

#endif /* __BL_RUN_TARGET_H__ */
