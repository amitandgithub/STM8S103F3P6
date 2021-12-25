/******************
** CLASS: gpt2
**
** DESCRIPTION:
**  Gpio input functionality
**
** CREATED: 25 Jan 2021, by Amit Chaudhary
**
** FILE: gpt2.h
**
******************/
#ifndef gpt2_h
#define gpt2_h

#include"system.h"

typedef enum
{
  TIM2_CH1_D4,
  TIM2_CH1_C5_RMP,
  TIM2_CH2_D3,
  TIM2_CH3_A3,  
  TIM2_CH3_D2_RMP
}Timer2_Channel_t;

template<uint8_t Prescalar, uint16_t Autoreload, uint8_t mode>
class gpt2
{ 
  uint32_t Ticks;  
  
public:
  
  typedef union
  {
    uint16_t Word;
    uint8_t Array[2];
  }Capture_t;
  
  uint8_t HwInit(void)
  {
    disableInterrupts();
    
    CLK->PCKENR1 |= (uint8_t) CLK_PCKENR1_TIM2;    
    
    TIM2->SR1 = (uint8_t)0x00;
    TIM2->SR2 = (uint8_t)0x00;

    TIM2->ARRH = (uint8_t)(Autoreload >> 8);
    TIM2->ARRL = (uint8_t)(Autoreload);

    /* Set the Prescaler value */
    TIM2->PSCR = (uint8_t)(Prescalar);
    
    TIM2->CR1 = mode;
    
    enableInterrupts();
    
    return 0;
  }
  
 /* 
 TIM_IER_BIE  
 TIM_IER_TIE   
 TIM_IER_CC2IE 
 TIM_IER_CC1IE 
 TIM_IER_UIE 
*/
  void InterruptEnable(uint8_t intterupts)
  {
    TIM2->IER |= intterupts;
  }
  
  void InterruptDisable(uint8_t intterupts)
  {
    TIM2->IER &= (uint8_t)(~(uint8_t)intterupts);
  }

  void OnePulseModeEnable(void)
  {
    TIM2->CR1 |= TIM2_CR1_OPM;
  }
  
  void OnePulseModeDisable(void)
  {
    TIM2->CR1 &= (uint8_t)(~TIM2_CR1_OPM);
  }

  void GeneratUpdateEvent(void)
  {
    TIM2->EGR |= (uint8_t)TIM2_EGR_UG;
  }
  
  void SetAutoReloadRegister(uint16_t Val)
  {
    TIM2->ARRH = (uint8_t)(Val >> 8);
    TIM2->ARRL = (uint8_t)(Val);
  }  
  
  void EnableAutoLoadPreload(void)
  {
    TIM2->CR1 |= (uint8_t)(TIM2_CR1_ARPE);    
  }  
  
  void DisableAutoLoadPreload(void)
  {
    TIM2->CR1 &= (uint8_t)(~TIM2_CR1_ARPE);    
  }
  
  void DisableUpdateEvent(void)
  {
    TIM2->CR1 |= TIM2_CR1_UDIS;
  }
  
  void EnableUpdateEvent(void)
  {
    TIM2->CR1 &= (uint8_t)(~TIM2_CR1_UDIS);
  }
  
  void Start(void)
  {
    EnableUpdateEvent();
    //InterruptEnable(TIM_IER_UIE);
    GeneratUpdateEvent();
    TIM2->CR1 |= TIM2_CR1_CEN;
  }
  
  void Stop(void)
  {
    //InterruptDisable(TIM_IER_UIE);
    TIM2->CR1 &= (uint8_t)(~TIM2_CR1_CEN);
  }
  
  uint32_t GetTicks(void) const
  {
    uint32_t ticks;
    
    disableInterrupts();
    ticks = Ticks;
    enableInterrupts();
    return ticks;
  }
  
  uint16_t GetCounter(void) const
  {
    Capture_t Capture;
    
    Capture.Array[0] = TIM2->CNTRH;
    Capture.Array[1] = TIM2->CNTRL;
    
    return Capture.Word;    
  } 
  
  void Delay_us(uint16_t us)
  {
    TIM2->ARRH = (uint8_t)(us >> 8);
    TIM2->ARRL = (uint8_t)(us);
    TIM2->SR1 = 0;
    TIM2->CR1 |= TIM2_CR1_CEN;
    while(( TIM2->SR1 & TIM2_SR1_UIF) != TIM2_SR1_UIF); 
  }
  
  void Delay_ms(uint16_t ms)
  {
    while(ms--)
    {
      Delay_us(989); // 989 works well for Milli Second delay
    }
  }
  
  void IRQ_Handler(void)
  {
    TIM2->SR1 &= (uint8_t)(~TIM2_SR1_UIF);
    Ticks++;
  }  
};

#endif // gpt.h