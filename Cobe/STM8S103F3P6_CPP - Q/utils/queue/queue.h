/******************
** FILE: queue.h
**
** DESCRIPTION:
**  queue implementation
**
** CREATED: 6 Feb 2021, by Amit Chaudhary
******************/

#ifndef queue_h
#define queue_h

#include "system.h"

template<typename T, typename Base_t = uint8_t, uint32_t QSize = 10>
class queue
{
public:
    typedef enum
    {
        QUEUE_OK,
        QUEUE_INVALID_PARAMS,
        QUEUE_FULL,
        QUEUE_EMPTY,		
    }Queue_Status_t;
    
    typedef T QueueSize_t;
    typedef Base_t Basic_t;
    
    queue();
    
    bool Full();
    
    bool Empty();
    
    Basic_t Available();
    
    Queue_Status_t Write(QueueSize_t data);
    
    Queue_Status_t Read(QueueSize_t* pdata);
    
    QueueSize_t Read(void)
    {
      
    }
    
private:
        Basic_t          ReadHead;
        Basic_t          WriteHead;
        Basic_t          Size;
        QueueSize_t      Buf[QSize];    
};


template<typename T, typename Base_t, uint32_t QSize>
queue<T,Base_t,QSize>::queue()
{
    WriteHead =  QSize - 1; // rear
    ReadHead =  0; // front
}
template<typename T, typename Base_t, uint32_t QSize>
bool queue<T,Base_t,QSize>::Full()
{	
  if(Size == QSize)
  	return true;
  else
  	return false;
}
template<typename T, typename Base_t, uint32_t QSize>
bool queue<T,Base_t,QSize>::Empty()
{
  if(Size == 0)
  	return true;
  else
  	return false;
}
template<typename T, typename Base_t, uint32_t QSize>
queue<T,Base_t,QSize>::Basic_t queue<T,Base_t,QSize>::Available()
{
  return Size;
}

/**
 * Enqueue/Insert an element to the queue. 
 */
 template<typename T, typename Base_t, uint32_t QSize>
queue<T,Base_t,QSize>::Queue_Status_t queue<T,Base_t,QSize>::Write(QueueSize_t data)
{
    if (Full()) 
    {
        return QUEUE_FULL;
    }

    WriteHead += 1;// (WriteHead + 1) % QSize;
    
    if(WriteHead == QSize)
        WriteHead = 0;

    // Increment queue size
    Size++;

    // Enqueue new element to queue
    Buf[WriteHead] = data;

    // Successfully enqueued element to queue
    return QUEUE_OK;
}

/**
 * Dequeue/Remove an element from the queue. 
 */
template<typename T, typename Base_t, uint32_t QSize>
queue<T,Base_t,QSize>::Queue_Status_t queue<T,Base_t,QSize>::Read(QueueSize_t* pdata)
{    
    if (Empty()) 
    {
        return QUEUE_EMPTY;
    }

    // Dequeue element from queue
    *pdata = Buf[ReadHead];

    // Ensure front never crosses array bounds
    ReadHead +=1;// (front + 1) % QSize;
    
     if(ReadHead == QSize)
        ReadHead = 0;

    // Decrease queue size
    Size--;

    return QUEUE_OK;
}


#endif //queue_h







