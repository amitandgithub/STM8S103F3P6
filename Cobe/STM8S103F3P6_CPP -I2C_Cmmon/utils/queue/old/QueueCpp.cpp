/******************
** FILE: queue.c
**
** DESCRIPTION:
**  queue implementation
**
** CREATED: 8/5/2019, by Amit Chaudhary
******************/
#include"QueueCpp.h"

void queue::Init()
{
    m_Queue.WriteHead =  m_Queue.Capacity - 1; // rear
    m_Queue.ReadHead =  0; // front
}

bool queue::Full()
{
	
  if(m_Queue.Size == m_Queue.Capacity)
  	return true;
  else
  	return false;
}

bool queue::Empty()
{
  if(m_Queue.Size == 0)
  	return true;
  else
  	return false;
}

queue::QueueSize_t queue::AvailableEnteries()
{
  return m_Queue.Size;
}

/**
 * Enqueue/Insert an element to the queue. 
 */
queue::Queue_Status_t queue::Write(QueueSize_t data)
{
    if (Full()) 
    {
        return QUEUE_FULL;
    }

    m_Queue.WriteHead += 1;// (m_Queue.WriteHead + 1) % CAPACITY;
    
    if(m_Queue.WriteHead == m_Queue.Capacity)
        m_Queue.WriteHead = 0;

    // Increment queue size
    m_Queue.Size++;

    // Enqueue new element to queue
    m_Queue.Buf[m_Queue.WriteHead] = data;

    // Successfully enqueued element to queue
    return QUEUE_OK;
}

/**
 * Dequeue/Remove an element from the queue. 
 */
queue::Queue_Status_t queue::Read(QueueSize_t* pdata)
{    
    if (Empty()) 
    {
        return QUEUE_EMPTY;
    }

    // Dequeue element from queue
    *pdata = m_Queue.Buf[m_Queue.ReadHead];

    // Ensure front never crosses array bounds
    m_Queue.ReadHead +=1;// (front + 1) % CAPACITY;
    
     if(m_Queue.ReadHead == m_Queue.Capacity)
        m_Queue.ReadHead = 0;

    // Decrease queue size
    m_Queue.Size--;

    return QUEUE_OK;
}








