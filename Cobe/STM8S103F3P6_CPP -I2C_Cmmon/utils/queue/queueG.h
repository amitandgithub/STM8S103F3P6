/******************
** FILE: queueG.h
**
** DESCRIPTION:
**  queueG implementation
**
** CREATED: 17 Apr 2021, by Amit Chaudhary
******************/

#ifndef queueG_h
#define queueG_h

#include "queueBase.h"

template < typename T, uint16_t size>
class queueG : public queueBase<T, uint8_t>
{
public:
  typedef queueBase<T, uint8_t> Base;
  typedef typename Base::Data_t Data_t;
  typedef typename Base::Status_t Status_t;

  queueG() : Base(&Array[0], size)
  {
    
  }

private:
  Data_t Array[size];
};

#endif //queueG_h
