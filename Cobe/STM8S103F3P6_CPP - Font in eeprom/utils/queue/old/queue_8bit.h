/******************
** FILE: queue.h
**
** DESCRIPTION:
**  queue implementation
**
** CREATED: 8/5/2019, by Amit Chaudhary
******************/

#ifndef queue_h
#define queue_h

//#include <stdint.h>
#include "stm8l10x.h"

typedef enum
{
	QUEUE_OK,
    QUEUE_INVALID_PARAMS,
	QUEUE_FULL,
	QUEUE_EMPTY,
		
}Queue_Status_t;

typedef struct
{
  uint8_t   ReadHead;
  uint8_t   WriteHead;
  uint8_t   Size;
  uint8_t   Capacity;
  uint8_t*  Buf;
}Queue_8bit_t;

typedef struct
{
  uint16_t   ReadHead;
  uint16_t   WriteHead;
  uint16_t   Size;
  uint16_t   Capacity;
  uint8_t*   Buf;
}Queue_16bit_t;

Queue_Status_t QueueInit(Queue_8bit_t* pQueue);

Queue_Status_t QueueFull(Queue_8bit_t* pQueue);

Queue_Status_t QueueEmpty(Queue_8bit_t* pQueue);

Queue_Status_t AvailableEnteries(Queue_8bit_t* pQueue, uint8_t* AvailableData);

Queue_Status_t QueueWrite(Queue_8bit_t* pQueue,uint8_t data);

Queue_Status_t QueueRead(Queue_8bit_t* pQueue, uint8_t* pdata);

#endif