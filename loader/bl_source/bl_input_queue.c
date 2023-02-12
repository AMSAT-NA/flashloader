#include "bl_input_queue.h"
#include "assert.h"
#include "sys_core.h"

/**
 * This code is NOT THREADSAFE, for that both enqueue() and dequeue()
 * must be treated as critical sections.
 *
 * The code works for our use case, where enqueue() is always called
 * in an interrupt service routine, by making sure that manipulations
 * of the size are atomic by disabling interrupts.
 *
 * Heimir Thor Sverrisson, Oct. 2022
 */

static uint32_t front;
static int32_t rear;
static uint32_t size;

static uint8 queue[QUEUE_SIZE];

void initQueue(){
	front = 0;
	rear = -1;
	size = 0;
}

uint32_t getQueueSize(){
	return size;
}

void enqueue(uint8 c){
	if(size == QUEUE_SIZE){
		// Overflow!!
		// assert(size < QUEUE_SIZE);
		return; // Simply drop the data!!!
	}
	rear = (rear + 1) % QUEUE_SIZE;
	queue[rear] = c;
	_disable_interrupt_();
	size++;
	_enable_interrupt_();
}

uint8 dequeue(){
	if(size == 0){
		// Underflow!!
		// assert(size > 0);
		return 0; // Not much else can be done!
	}
	uint8 item = queue[front];
	front = (front + 1) % QUEUE_SIZE;
	_disable_interrupt_();
	size--;
	_enable_interrupt_();
	return item;
}
