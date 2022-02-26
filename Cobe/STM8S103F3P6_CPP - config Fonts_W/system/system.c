/******************
** FILE: system.c
**
** DESCRIPTION:
**  System level utilities implementation
**
** CREATED: 7/31/2019, by Amit Chaudhary
******************/
#include "system.h"


TestGPIO_t TestGPIO;
TestLED_t TestLED;


void Dummy_ISR(void)
{
  
}

uint8_t WaitOnFlag(volatile uint8_t* reg, uint8_t bitmask, uint8_t status, uint16_t timeout) // 35(0x25)bytes
{
  while( ((*reg & bitmask) == status) && timeout-- );    
  return ((*reg & bitmask) == status);
}

typedef void (*ILP_Fptr_t)(void);
uint8_t Ilegal_Opcode = 0x75;
const ILP_Fptr_t ILP_Fptr = (ILP_Fptr_t)&Ilegal_Opcode;

void system_Reset(void)
{
    (*ILP_Fptr)(); // "Illegal opcode execution" Reset
}

//  void MemSet(uint8_t *mem, uint8_t data, uint16_t len)
//  {
//    if (mem)
//    {
//      while (len--)
//        *mem++ = data;
//    }
//  }

void delay_ms(unsigned int n)
{
  while (n-- > 0)
  {
    delay_1ms();
  }
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