/******************
** FILE: queueBase.h
**
** DESCRIPTION:
**  queueBase implementation
**
** CREATED: 17 Apr 2021, by Amit Chaudhary
******************/

#ifndef queueBase_h
#define queueBase_h

#include "system.h"

#if 0

template<typename T, typename Size_t>
class queueBase
{
public:
    typedef enum
    {
        OK,
        INVALID_PARAMS,
        FULL,
        EMPTY
    }Status_t;
    
    typedef T Data_t;
    typedef Data_t* pBuf;
    
    queueBase(pBuf buf, Size_t size )
    {
      QSize = size;
      WriteHead =  QSize - 1; // rear
      ReadHead =  0; // front
      Buf = 0;     
    }
    
    static bool Full() {return Size == QSize;}
    
    static bool Empty() {return Size == 0;}
    
    static Size_t Available() {return Size;}
    
    static Status_t Write(Data_t data) // 6.4 us on STM8
    {
      if(Full()) { return FULL; }
      
      WriteHead += 1;// (WriteHead + 1) % QSize;
      
      if(WriteHead == QSize) { WriteHead = 0; }
      
      Size++;
      
      Buf[WriteHead] = data;

      return OK;
    }
    
    static void Write_opti(Data_t data)
    {
      if(Full()) return;
      
      Buf[WriteHead++] = data;  
      
      if(WriteHead == QSize) { WriteHead = 0; }   
       
      Size++;
    }
    
    static Status_t Read(Data_t* pdata)
    {    
      if (Empty()) 
      {
        return EMPTY;
      }
      
      // DequeueBase element from queueBase
      *pdata = Buf[ReadHead];
      
      // Ensure front never crosses array bounds
      ReadHead +=1;// (front + 1) % QSize;
      
      if(ReadHead == QSize)
        ReadHead = 0;
      
      // Decrease queueBase size
      Size--;
      
      return OK;
    }
    
private:
        static Size_t          ReadHead;
        static Size_t          WriteHead;
        static Size_t          Size;
        static Data_t*         Buf;    
        static Size_t          QSize;
};

template<typename T, typename Size_t>
Size_t queueBase<T,Size_t>::ReadHead;
template<typename T, typename Size_t>
         Size_t queueBase<T,Size_t>::WriteHead;
template<typename T, typename Size_t>
         Size_t queueBase<T,Size_t>::Size;
template<typename T, typename Size_t>
        queueBase<T,Size_t>::Data_t* queueBase<T,Size_t>::Buf;  
template<typename T, typename Size_t>
        Size_t queueBase<T,Size_t>::QSize;

#else

template<typename T, typename Size_t>
class queueBase
{
public:
    typedef enum
    {
        OK,
        INVALID_PARAMS,
        FULL,
        EMPTY
    }Status_t;
    
    typedef T Data_t;
    typedef Data_t* pBuf;
    
    queueBase(pBuf buf, Size_t size )
    {
      QSize = size;
      WriteHead =  QSize - 1; // rear
      ReadHead =  0; // front
      Buf = 0;
     
    }
    
    bool Full() const {return Size == QSize;}
    
    bool Empty() const {return Size == 0;}
    
    Size_t Available() const {return Size;}
    
    Status_t Write(Data_t data) // 6.4 us on STM8
    {
      if(Full()) { return FULL; }
      
      WriteHead += 1;// (WriteHead + 1) % QSize;
      
      if(WriteHead == QSize) { WriteHead = 0; }
      
      Size++;
      
      Buf[WriteHead] = data;

      return OK;
    }
    
    void Write_opti(Data_t data)
    {
      if(Full()) return;
      
      Buf[WriteHead++] = data;  
      
      if(WriteHead == QSize) { WriteHead = 0; }   
       
      Size++;
    }
    
    Status_t Read(Data_t* pdata)
    {    
      if (Empty()) 
      {
        return EMPTY;
      }
      
      // DequeueBase element from queueBase
      *pdata = Buf[ReadHead];
      
      // Ensure front never crosses array bounds
      ReadHead +=1;// (front + 1) % QSize;
      
      if(ReadHead == QSize)
        ReadHead = 0;
      
      // Decrease queueBase size
      Size--;
      
      return OK;
    }
    
private:
        Size_t          ReadHead;
        Size_t          WriteHead;
        Size_t          Size;
        Data_t*         Buf;    
        Size_t          QSize;
};


#endif


#endif //queueBase_h




