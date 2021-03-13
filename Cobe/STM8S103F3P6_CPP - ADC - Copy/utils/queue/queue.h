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
    
    typedef T DataSize_t;
    
    queue()
    {
      WriteHead =  QSize - 1; // rear
      ReadHead =  0; // front
    }
    
    bool Full() const {return Size == QSize;}
    
    bool Empty() const {return Size == 0;}
    
    Base_t Available() const {return Size;}
    
    Queue_Status_t Write(DataSize_t data)
    {
      if(Full()) { return QUEUE_FULL; }
      
      WriteHead += 1;// (WriteHead + 1) % QSize;
      
      if(WriteHead == QSize) { WriteHead = 0; }
      
      Size++;
      
      Buf[WriteHead] = data;

      return QUEUE_OK;
    }
    
    void Write_opti(DataSize_t data)
    {
      if(Full()) return;
      
      Buf[WriteHead++] = data;  
      
      if(WriteHead == QSize) { WriteHead = 0; }   
       
      Size++;
    }
    
    Queue_Status_t Read(DataSize_t* pdata)
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
    
private:
        Base_t          ReadHead;
        Base_t          WriteHead;
        Base_t          Size;
        DataSize_t      Buf[QSize];    
};

#endif //queue_h







