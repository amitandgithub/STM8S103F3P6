#ifndef _GPIO_OUT_H_
#define _GPIO_OUT_H_

#include "system.h"
#include "Gpio.h"

namespace HAL
{

  template <Gpio::Port_t Port, Gpio::Pin_t Pin, Gpio::Mode_t Mode = Gpio::OD_PUSH_PULL>
    class GpioOut : public Gpio
    {
    public:
      
        static void HwInit(void){ Gpio::SetOutputMode(Port, Pin, Mode); }

        void operator= (bool x){ x ? High() : Low(); }  
        
        void operator= (GpioOut& x) const{ x.Read() ? High() : Low();}
        
        operator bool() { return Read(); }
        
        static bool Read(void) { return (bool)((GPIO_TypeDef *)Port)->ODR & Pin; }

        static void High(void) { ((GPIO_TypeDef *)Port)->ODR |= Pin; }

        static void Low(void) { ((GPIO_TypeDef *)Port)->ODR &= ~Pin; }

        static void Toggle(void) { ((GPIO_TypeDef *)Port)->ODR ^= Pin; }
    };
}

#endif // _GPIO_OUT_H_