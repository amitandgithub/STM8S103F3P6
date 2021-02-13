/******************
** CLASS: DigitalIn
**
** DESCRIPTION:
**  Gpio input functionality
**
** CREATED: 8/9/2019, by Amit Chaudhary
**
** FILE: DigitalIn.h
**
******************/
#ifndef DigitalIn_h
#define DigitalIn_h

#include"DigitalInOut.h"

extern void Dummy_ISR();

template<Port_t Port, PIN_t Pin, Mode_t Mode = INPUT_PULLUP, IntEdge_t Edge = NO_INTERRUPT, GpioFptr_t GpioFptr = Dummy_ISR>
class DigitalIn
{
public:
    //DigitalIn(){};
    
    //~DigitalIn(){};
    
    void HwInit();
    
    void SetInputMode();
    
    void SetInterrupt();
    
    void SetPortSensitivity();
    
    //void SetHalfPortSelection(EXTI_HalfPort_TypeDef EXTI_HalfPort, FunctionalState NewState);
        
    bool Read(){ return (bool)(((GPIO_TypeDef*)Port)->IDR & Pin);}  
    
    void Register_ISR();
    
    void IRQ_Handler(void)
    {
      if(Edge == INTERRUPT_ON_RISING)
      {
        if(Read() == true)
        {
          GpioFptr();
        }
      }
      else
      {
        if(Read() == false)
        {
          GpioFptr();
        }
      }
    }
    

};


template<Port_t Port, PIN_t Pin,Mode_t Mode,IntEdge_t Edge, GpioFptr_t GpioFptr>
void DigitalIn<Port,Pin,Mode,Edge,GpioFptr>::HwInit()
{
    __disable_interrupt();
    SetInputMode();    
    SetInterrupt();  
    __enable_interrupt();
}

template<Port_t Port, PIN_t Pin,Mode_t Mode,IntEdge_t Edge, GpioFptr_t GpioFptr>
void DigitalIn<Port,Pin,Mode,Edge,GpioFptr>::SetInputMode()
{
    switch(Mode)
    {
    case INPUT_FLOATING:
        ((GPIO_TypeDef*)Port)->DDR &= ~Pin; // 0
        ((GPIO_TypeDef*)Port)->CR1 &= ~Pin; // 0
        ((GPIO_TypeDef*)Port)->CR2 &= ~Pin; // 0
        break;
    case INPUT_PULLUP:
        ((GPIO_TypeDef*)Port)->DDR &= ~Pin;  // 0
        ((GPIO_TypeDef*)Port)->CR1 |=  Pin;  // 1
        ((GPIO_TypeDef*)Port)->CR2 &= ~Pin;  // 0
        break;
    case INPUT_FLOATING_INTERRUPT:
        ((GPIO_TypeDef*)Port)->DDR &= ~Pin; // 0
        ((GPIO_TypeDef*)Port)->CR1 &= ~Pin; // 0
        ((GPIO_TypeDef*)Port)->CR2 |=  Pin; // 1
        break;
    case INPUT_PULLUP_INTERRUPT:
        ((GPIO_TypeDef*)Port)->DDR &= ~Pin; // 0
        ((GPIO_TypeDef*)Port)->CR1 |=  Pin; // 1
        ((GPIO_TypeDef*)Port)->CR2 |=  Pin; // 1
        break;
    default: break;  
    }  
}

template<Port_t Port, PIN_t Pin,Mode_t Mode,IntEdge_t Edge, GpioFptr_t GpioFptr>
void DigitalIn<Port,Pin,Mode,Edge,GpioFptr>::SetInterrupt()
{
  switch(Port)
  {
  case PORTA:
    EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PAIS);
    EXTI->CR1 |= (uint8_t)((Edge) << 0);
    EXTI_ISRs[0 + Gpio_Register_Isr(Pin)] = GpioFptr;
    break;
  case PORTB:
    EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PBIS);
    EXTI->CR1 |= (uint8_t)((Edge) << 2);
    EXTI_ISRs[8 + Gpio_Register_Isr(Pin)] = GpioFptr;
    break;
  case PORTC:
    EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PCIS);
    EXTI->CR1 |= (uint8_t)((Edge) << 4);
    EXTI_ISRs[16 + Gpio_Register_Isr(Pin)] = GpioFptr;
    break;
  case PORTD:
    EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PDIS);
    EXTI->CR1 |= (uint8_t)((Edge) << 6);
    EXTI_ISRs[24 + Gpio_Register_Isr(Pin)] = GpioFptr;
    break;

  default: break;
  } 
}


#endif //DigitalIn_h