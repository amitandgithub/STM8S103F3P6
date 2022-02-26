/******************
** FILE: queue_16bit.c
**
** DESCRIPTION:
**  queue implementation
**
** CREATED: 8/5/2019, by Amit Chaudhary
******************/
#include"queue_16bit.h"

Queue_Status_t QueueInit(Queue_16bit_t* pQueue)
{
  if(pQueue)
  {
    pQueue->WriteHead =  pQueue->Capacity - 1; // rear
    pQueue->ReadHead =  0; // front
    return QUEUE_OK;
  }
  return QUEUE_INVALID_PARAMS;
}

Queue_Status_t QueueFull(Queue_16bit_t* pQueue)
{
  if(pQueue == 0)
		return QUEUE_INVALID_PARAMS;
	
  if(pQueue->Size == pQueue->Capacity)
  	return QUEUE_FULL;
  else
  	return QUEUE_OK;
}

Queue_Status_t QueueEmpty(Queue_16bit_t* pQueue)
{
	if(pQueue == 0)
			return QUEUE_INVALID_PARAMS;

  if(pQueue->Size == 0)
  	return QUEUE_EMPTY;
  else
  	return QUEUE_OK;
}

Queue_Status_t AvailableEnteries(Queue_16bit_t* pQueue, uint8_t* pAvailableData)
{
	if(pQueue == 0)
			return QUEUE_INVALID_PARAMS;

  *pAvailableData =  pQueue->Size;

  return QUEUE_OK;
}
/**
 * Enqueue/Insert an element to the queue. 
 */
Queue_Status_t QueueWrite(Queue_16bit_t* pQueue,uint8_t data)
{
	Queue_Status_t status;

	status = QueueEmpty(pQueue);
	
    if (status != QUEUE_OK) 
    {
        return status;
    }

    pQueue->WriteHead += 1;// (pQueue->WriteHead + 1) % CAPACITY;
    
    if(pQueue->WriteHead == pQueue->Capacity)
        pQueue->WriteHead = 0;

    // Increment queue size
    pQueue->Size++;

    // Enqueue new element to queue
    pQueue->Buf[pQueue->WriteHead] = data;

    // Successfully enqueued element to queue
    return QUEUE_OK;
}

/**
 * Dequeue/Remove an element from the queue. 
 */
Queue_Status_t QueueRead(Queue_16bit_t* pQueue, uint8_t* pdata)
{
	Queue_Status_t status;

	status = QueueEmpty(pQueue);
	
    // Queue is empty, throw Queue underflow error
    if (status != QUEUE_OK)
    {
        return status;
    }

    // Dequeue element from queue
    *pdata = pQueue->Buf[pQueue->ReadHead];

    // Ensure front never crosses array bounds
    pQueue->ReadHead +=1;// (front + 1) % CAPACITY;
    
     if(pQueue->ReadHead == pQueue->Capacity)
        pQueue->ReadHead = 0;

    // Decrease queue size
    pQueue->Size--;

    return QUEUE_OK;
}








