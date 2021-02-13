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

template<Port_t Port, PIN_t Pin, Mode_t Mode = INPUT_PULLUP, IntEdge_t Edge = NO_INTERRUPT, GpioFptr_t Gpio_Callback = Dummy_ISR>
class DigitalIn
{
public:
    
  void HwInit()
  {
    __disable_interrupt();
    
    SetInputMode();  
    
    if(Edge != NO_INTERRUPT)
    {
      SetInterrupt();
    }
    
    __enable_interrupt();
    }
    
  void SetInputMode()
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
    
  void SetInterrupt()
  {
    switch(Port)
    {
    case PORTA:
      EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PAIS);
      EXTI->CR1 |= (uint8_t)((Edge) << 0);
      break;
    case PORTB:
      EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PBIS);
      EXTI->CR1 |= (uint8_t)((Edge) << 2);
      break;
    case PORTC:
      EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PCIS);
      EXTI->CR1 |= (uint8_t)((Edge) << 4);
      break;
    case PORTD:
      EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PDIS);
      EXTI->CR1 |= (uint8_t)((Edge) << 6);
      break;
      
    default: break;
    } 
  }
        
    bool Read()
    {
      return (bool)(((GPIO_TypeDef*)Port)->IDR & Pin);
    }  
    
    void IRQ_Handler()
    {
      if(Edge == INTERRUPT_ON_RISING)
      {
        if(Read() == true)
        {
          Gpio_Callback();
        }
      }
      else
      {
        if(Read() == false)
        {
          Gpio_Callback();
        }
      }
    }
};

#endif //DigitalIn_h