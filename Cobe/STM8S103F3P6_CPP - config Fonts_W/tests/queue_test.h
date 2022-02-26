/******************
** FILE: queue_test.h
**
** DESCRIPTION:
**  queue implementation
**
** CREATED: 6 Feb 2021, by Amit Chaudhary
******************/

#ifndef queue_test_h
#define queue_test_h

#include "system.h"
#include "queue.h"
#include "queueBase.h"
#include "queueG.h"




struct queue_test
{
  
  typedef queue<uint8_t,uint8_t,10> queue1_t;
  queue1_t queue1;
  typedef typename queue1_t::DataSize_t Data_t;
  typedef typename queue1_t::Queue_Status_t Status_t;
  
  typedef queue<uint8_t,uint8_t,20> queue2_t;
  queue2_t queue2;
  typedef typename queue2_t::DataSize_t Data2_t;
  typedef typename queue2_t::Queue_Status_t Status2_t;
  
  typedef queueG<uint8_t,4> queue3_t;  
  queue3_t queue3;
  typedef typename queue3_t::Data_t Data3_t;
  typedef typename queue3_t::Status_t Status3_t;
  
  typedef queueG<uint8_t,10> queue4_t;  
  queue4_t queue4;
  typedef typename queue4_t::Data_t Data4_t;
  typedef typename queue4_t::Status_t Status4_t;
  
  
  
  Data_t data;
  Status_t Status;
  
  Data2_t data2;
  Status2_t Status2;
  
  Data3_t data3;
  Status3_t Status3;
  
  Data4_t data4;
  volatile Status4_t Status4;
  
  void Run(void)
  {
    data = 0;
    
    while(1)
    {
      
#if 0
      //while(queue1.Write(++data) != queue_t::QUEUE_FULL);
      TestGPIO.High();
      Status = queue1.Write(++data);
      TestGPIO.Low();
      Status = queue1.Write(++data);
      Status = queue1.Write(++data);
      Status = queue1.Write(++data);
      
      //while(queue1.Read(&data) != queue_t::QUEUE_EMPTY);
      
      Status = queue1.Read(&data);
      Status = queue1.Read(&data);
      Status = queue1.Read(&data);
      Status = queue1.Read(&data);
      
      Status2 = queue2.Write(++data2);
      Status2 = queue2.Write(++data2);
      Status2 = queue2.Write(++data2);
      Status2 = queue2.Write(++data2);
      
      //while(queue2.Read(&data2) != queue_t::QUEUE_EMPTY);
      
      Status2 = queue2.Read(&data2);
      Status2 = queue2.Read(&data2);
      Status2 = queue2.Read(&data2);
      Status2 = queue2.Read(&data2);
      //TestGPIO.Low();
#else
      TestGPIO.High();
      Status3 = queue3.Write(++data3);
      TestGPIO.Low();
      Status3 = queue3.Write(++data3);
      Status3 = queue3.Write(++data3);
      Status3 = queue3.Write(++data3);
      
      
      Status4 = queue4.Read(&data4);
      Status4 = queue4.Read(&data4);
      Status4 = queue4.Read(&data4);
      Status4 = queue4.Read(&data4);
      //while(queue3.Read(&data3) != queue_t::QUEUE_EMPTY);
      
      
      
      data4 += 10;
      Status4 = queue4.Write(++data4);
      Status4 = queue4.Write(++data4);
      Status4 = queue4.Write(++data4);
      Status4 = queue4.Write(++data4);
      
      //while(queue3.Read(&data3) != queue_t::QUEUE_EMPTY);
      
      
      Status3 = queue3.Read(&data3);
      Status3 = queue3.Read(&data3);
      Status3 = queue3.Read(&data3);
      Status3 = queue3.Read(&data3);
      
      
#endif
      
      
    }
    
  }
  
  
};
/*

template < typename T, uint16_t size>
struct QueueFinal : public queueBase<T,uint8_t>
{
typedef queueBase<T,uint8_t> Base;
typedef typename Base::Data_t Data_t;
typedef typename Base::Status_t Status_t;

QueueFinal(): Base(&Array[0],size)
{

  }

Data_t Array[size];

};

*/


#endif //queue_test_h