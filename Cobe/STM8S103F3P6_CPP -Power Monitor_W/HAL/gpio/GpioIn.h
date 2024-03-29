#ifndef _GPIO_IN_H_
#define _GPIO_IN_H_

#include "system.h"
#include "Gpio.h"

namespace HAL
{

    template <Gpio::Port_t Port, Gpio::Pin_t Pin, Gpio::Mode_t Mode = Gpio::PU, Gpio::IntEdge_t Edge = Gpio::NO_INTERRUPT, typename Callback = DummyCallback>
    class GpioIn : public Gpio
    {

    public:
        static void HwInit(void)
        {
          
          SYS_ASSERT( ((Mode == FL_INT) || (Mode == PU_INT) ) && (Edge == NO_INTERRUPT) , "Mode as interrupt and Edge as NO_INTERRUPT must not be mixed " );
          SYS_ASSERT( ((Mode == FL) || (Mode == PU) && (Edge != NO_INTERRUPT) ), "Mode as not interrupt and Edge as INTERRUPT must not be mixed " );
            
            __disable_interrupt();

            Gpio::SetInputMode(Port, Pin, Mode);

            if (Edge != NO_INTERRUPT)
            {
                SetInterrupt(Port, Edge);
            }

            __enable_interrupt();
        }

        operator bool() { return Read(); }

        static bool Read(void)
        {
            return (bool)(((GPIO_TypeDef *)Port)->IDR & Pin);
        }

        /*
         * This "IRQ_Handler" will be instantiated for each Port,Pin pair
         * and if the "Edge" parameter is NO_INTERRUPT then no code
         * will be added in main ISR otherwise below code will be added
         * in main Isr based upon "Edge"
        */
        //template<Gpio::Port_t Port, Gpio::Pin_t Pin>
        static void IRQ_Handler(void)
        {          
          if (Edge == RISING)
          {
            if (Read() == true)
            {
              Callback::Handler();
            }
          }
          else if (Edge == FALLING)
          {
            if (Read() == false)
            {
              Callback::Handler();
            }
          }
          else if (Edge == RISING_FALLING)
          {
            Callback::Handler();
          }
          
        }
    };
}


#if 0


namespace HAL
{
    static uint8_t dummy;
    template < Gpio::Port_t Port, Gpio::Pin_t Pin, Gpio::Mode_t Mode = Gpio::PU, Gpio::IntEdge_t Edge = Gpio::NO_INTERRUPT,typename Callback = DummyCallback, Callback& CallbackObj = DummyCallbackObj >
    class GpioIn : public Gpio
    {

    public:
        static void HwInit(void)
        {
          
          SYS_ASSERT( ((Mode == FL_INT) || (Mode == PU_INT) ) && (Edge == NO_INTERRUPT) , "Mode as interrupt and Edge as NO_INTERRUPT must not be mixed " );
          SYS_ASSERT( ((Mode == FL) || (Mode == PU) && (Edge != NO_INTERRUPT) ), "Mode as not interrupt and Edge as INTERRUPT must not be mixed " );
            
            __disable_interrupt();

            Gpio::SetInputMode(Port, Pin, Mode);

            if (Edge != NO_INTERRUPT)
            {
                SetInterrupt(Port, Edge);
            }

            __enable_interrupt();
        }

        operator bool() { return Read(); }

        static bool Read(void)
        {
            return (bool)(((GPIO_TypeDef *)Port)->IDR & Pin);
        }

        /*
         * This "IRQ_Handler" will be instantiated for each Port,Pin pair
         * and if the "Edge" parameter is NO_INTERRUPT then no code
         * will be added in main ISR otherwise below code will be added
         * in main Isr based upon "Edge"
        */
        //template<Gpio::Port_t Port, Gpio::Pin_t Pin>
        static void IRQ_Handler(void)
        {          
          if (Edge == RISING)
          {
            if (Read() == true)
            {
              CallbackObj.Handler();
            }
          }
          else if (Edge == FALLING)
          {
            if (Read() == false)
            {
              CallbackObj.Handler();
            }
          }
          else if (Edge == RISING_FALLING)
          {
            CallbackObj.Handler();
          }
          
        }
    };
}

#endif
#endif // _GPIO_IN_H_