/******************
** CLASS: DigitalIn
**
** DESCRIPTION:
**  Gpio input functionality
**
** CREATED: 29-12-2020, by Amit Chaudhary
**
** FILE: gpt4.h
**
******************/
#ifndef gpt4_h
#define gpt4_h

#include"system.h"

template<uint8_t Prescalar, uint8_t Autoreload, uint8_t mode = 0>
class gpt4
{
  uint32_t Ticks;
public:
  
  uint8_t HwInit(void)
  {
    disableInterrupts();

    CLK->PCKENR1 |= (uint8_t) CLK_PCKENR1_TIM4;    
    TIM4->SR1 = 0;
    TIM4->ARR = Autoreload;
    TIM4->PSCR = Prescalar;
    TIM4->CR1 = TIM4_CR1_ARPE | mode;
    TIM4->EGR |= TIM4_EGR_UG;

    enableInterrupts();
    
    return 0;
  }
  
  void InterruptEnable(void)
  {
    TIM4->IER = TIM4_IER_UIE;
  }
  
  void InterruptDisable(void)
  {
    TIM4->IER = 0;
  }
  
  void Start(void)
  {
    TIM4->CR1 |= TIM4_CR1_CEN;
  }
  
  void Stop(void)
  {
    TIM4->IER = 0;
    TIM4->CR1 &= (uint8_t)(~TIM4_CR1_CEN);
  }
  
  uint32_t GetTicks(void)
  {
    uint32_t ticks;
    disableInterrupts();
    ticks = Ticks;
    enableInterrupts();
    return ticks;
  }
  
  uint8_t GetCounter(void){ return TIM4->CNTR;} 
  
  // This delay will work for max of 255 us 
  // if you need more delay then call this function multiple times
  void Delay_us(uint8_t us)
  {
    //PB1.High();
    TIM4->ARR = us;
    TIM4->SR1 = 0;
    TIM4->CR1 |= TIM4_CR1_CEN;
    //PB1.Low();
    while(TIM4->SR1 != 0x01); 
    //PB1.High();
  }
  
  void Delay_US(uint16_t us)
  {
    uint16_t US = us;
    do
    {
      if(US > 255)
      {
        Delay_us(255-1); // 1 us for compensation
        US = US - 255;
      }
      else
      {
        Delay_us(US-1);  // 1 us for compensation
        US = 0;
      }             
    }while(US);
  }
  
  void Delay_ms(uint16_t ms)
  {
    while(ms--)
    {
      Delay_us(250);
      Delay_us(250);
      Delay_us(250);
      Delay_us(250 - 3);
    }
  }
  
  void DelayTicks(uint8_t  ticks)
  {
    uint8_t start, end;
    start = GetCounter();
    end = start + ticks;
    if (start < end)
    {
      while ((GetCounter() >= start) && (GetCounter() < end))
      {
        // do nothing
      }
    }
    else
    {
      while ((GetCounter() >= start) || (GetCounter() < end))
      {
        // do nothing
      }
    }
  }  
  
  void IRQ_Handler(void)
  {
    TIM4->SR1 = (uint8_t)0x00;
    Ticks++;
  }
  
};


#endif // gpt4_h