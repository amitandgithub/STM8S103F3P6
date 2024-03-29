/******************
** FILE: system.c
**
** DESCRIPTION:
**  System level utilities implementation
**
** CREATED: 7/31/2019, by Amit Chaudhary
******************/
#include "system.h"
#include"interrupt.h"
inline void Dummy_ISR(){};


//interrupt<30> SystemInterrupts;

/* Array of EXTI interrupts*/
GpioFptr_t EXTI_ISRs[4*8];





uint8_t WaitOnFlag(volatile uint8_t* reg, uint8_t bitmask, uint8_t status, uint16_t timeout) // 35(0x25)bytes
{
  while( ((*reg & bitmask) == status) && timeout-- );    
  return ((*reg & bitmask) == status);
}






#if 0


const uint8_t CLZ[0xff] = 
{
255,
0,                      /* 1,1 */
1,1,                    /* 2,2 */
2,2,2,2,                /* 3,4 */
3,3,3,3,3,3,3,3,        /* 4,8 */
4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,        /* 5,16 */
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,        /* 6,32 */
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,        /* 7,64 */
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,        /* 7,128 */
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};

#endif