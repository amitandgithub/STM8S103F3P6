/******************
** CLASS: Interrupt
**
** DESCRIPTION:
**  Interrupt functionality
**
** CREATED: 06-Jan-2021, by Amit Chaudhary
**
** FILE: Interrupt.h
**
******************/
#ifndef Interrupt_h
#define Interrupt_h

#include"system.h"

  typedef enum
  {
    FLASH_IRQ = 1,
    EXTIB_IRQ = 6,
    EXTID_IRQ = 7    
  }ISR_Id_t;

typedef void (*ISR_t)(void);

template<uint8_t TotalISRs>
class interrupt
{ 
  ISR_t ISR_Table[TotalISRs];
  
public:
  
  template<typename T>
  void Register(T anISR, ISR_Id_t ISR_Id)
  {
    ISR_Table[ISR_Id] = reinterpret_cast<T*>(anISR);
  }
  
  void Execute(ISR_Id_t ISR_Id)
  {
    ISR_Table[ISR_Id]();
  }
  
};


#endif // Interrupt_h