
#ifndef SYSTEM_H
#define SYSTEM_H


#include <string.h>
#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_spi.h"
#include "GpioOut.h"

#define INLINE static inline
#define SVLREG

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE (!TRUE)
#endif

void Dummy_ISR(void);

using namespace HAL;

typedef GpioOut<A3> TestGPIO_t;
typedef GpioOut<B2> TestLED_t;

extern TestGPIO_t TestGPIO;
extern TestLED_t TestLED;

extern const uint8_t CLZ[];

void system_Reset(void);

 void MemSet(uint8_t *mem, uint8_t data, uint16_t len);

#define SIZE_OF_ARRAY(__array) sizeof(__array)/sizeof(__array[0])
//
// Delay loop
//
// Actual delay depends on clock settings
// and compiler optimization settings.
//

#define delay_1us(); delay(0);

#define delay_10us(); delay(18);

#define delay_50us(); delay(98);

#define delay_100us(); delay(198);

#define delay_1ms(); delay(1998);

#define delay_10ms(); delay(19998);

static inline void delay(unsigned int n)
{
  volatile unsigned int m = n;
    while (m-- > 0);
}


static inline void delay1(void)
{
  __asm("NOP");
  __asm("NOP");
  __asm("NOP");
  __asm("NOP");
  __asm("NOP");
//  __asm("NOP");
//  __asm("NOP");
//  __asm("NOP");
}

static inline void delay_Us(unsigned int n)
{
    while (n-- > 0)
	{
		delay1();
	}
}


 void delay_ms(unsigned int n);



uint8_t WaitOnFlag(volatile uint8_t* reg, uint8_t bitmask, uint8_t status, uint16_t timeout);



#define SYS_ASSERT(condition,message) static_assert( !(condition),message)


struct DummyCallback
{
  static void Handler(void)
  {
    
  }
};

template<class T> 
const T& min(const T& a, const T& b)
{
    return (b < a) ? b : a;
}




#endif