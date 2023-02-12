#ifndef __BL_INPUT_QUEUE_H__
#define __BL_INPUT_QUEUE_H__

#include "sys_common.h"

#define QUEUE_SIZE 1040

void initQueue();
uint32_t getQueueSize();
void enqueue(uint8 c);
uint8 dequeue();

#endif /* __BL_INPUT_QUEUE_H__ */
