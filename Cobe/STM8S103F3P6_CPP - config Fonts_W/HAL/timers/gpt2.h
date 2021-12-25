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

namespace HAL
{
  
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
  static uint16_t Ticks;  
  
public:
  
  typedef enum
  {
    CC1IE       = (uint8_t)TIM2_IER_CC1IE,
    CC2IE       = (uint8_t)TIM2_IER_CC2IE,
    CC3IE       = (uint8_t)TIM2_IER_CC3IE,
    UIE         = (uint8_t)TIM2_IER_UIE,
    ALL         = (uint8_t)0xFF
  }Interrupt_t;
  
  typedef union
  {
    uint16_t Word;
    uint8_t Array[2];
  }Capture_t;
  
  static uint8_t HwInit(void)
  {
    disableInterrupts();
    
    CLK->PCKENR1 |= (uint8_t) CLK_PCKENR1_TIM2;    
    
    TIM2->SR1 = (uint8_t)0x00;
    TIM2->SR2 = (uint8_t)0x00;

    TIM2->ARRH = (uint8_t)(Autoreload >> 8);
    TIM2->ARRL = (uint8_t)(Autoreload);

    /* Set the Prescaler value 
    fCK_CNT = fCK_PSC/2^(PSCR[3:0]) */
    TIM2->PSCR = (uint8_t)(Prescalar);
    
    TIM2->CR1 = mode;
    
    enableInterrupts();
    
    return 0;
  }

  static void InterruptEnable(Interrupt_t intterupts)
  {
    TIM2->IER |= intterupts;
  }
  
  static void InterruptDisable(Interrupt_t intterupts)
  {
    TIM2->IER &= (uint8_t)(~(uint8_t)intterupts);
  }

  static void OnePulseModeEnable(void)
  {
    TIM2->CR1 |= TIM2_CR1_OPM;
  }
  
  static void OnePulseModeDisable(void)
  {
    TIM2->CR1 &= (uint8_t)(~TIM2_CR1_OPM);
  }

  static void GeneratUpdateEvent(void)
  {
    TIM2->EGR |= (uint8_t)TIM2_EGR_UG;
  }
  
  static void SetAutoReloadRegister(uint16_t Val)
  {
    TIM2->ARRH = (uint8_t)(Val >> 8);
    TIM2->ARRL = (uint8_t)(Val);
  }  
  
  static void EnableAutoLoadPreload(void)
  {
    TIM2->CR1 |= (uint8_t)(TIM2_CR1_ARPE);    
  }  
  
  static void DisableAutoLoadPreload(void)
  {
    TIM2->CR1 &= (uint8_t)(~TIM2_CR1_ARPE);    
  }
  
  static void DisableUpdateEvent(void)
  {
    TIM2->CR1 |= TIM2_CR1_UDIS;
  }
  
  static void EnableUpdateEvent(void)
  {
    TIM2->CR1 &= (uint8_t)(~TIM2_CR1_UDIS);
  }
  
  static void Start(void)
  {
    EnableUpdateEvent();
    //InterruptEnable(TIM_IER_UIE);
    GeneratUpdateEvent();
    TIM2->CR1 |= TIM2_CR1_CEN;
  }
  
  static void Stop(void)
  {
    //InterruptDisable(TIM_IER_UIE);
    TIM2->CR1 &= (uint8_t)(~TIM2_CR1_CEN);
  }
  
  static uint16_t GetTicks(void) 
  {
    uint16_t ticks;
    
    disableInterrupts();
    ticks = Ticks;
    enableInterrupts();
    return ticks;
  }
  
  static uint16_t GetCounter(void)
  {
    Capture_t Capture;
    
    Capture.Array[0] = TIM2->CNTRH;
    Capture.Array[1] = TIM2->CNTRL;
    
    return Capture.Word;    
  } 
  
  static void Delay_us(uint16_t us)
  {
    TIM2->ARRH = (uint8_t)(us >> 8);
    TIM2->ARRL = (uint8_t)(us);
    TIM2->SR1 = 0;
    TIM2->CR1 |= TIM2_CR1_CEN;
    while(( TIM2->SR1 & TIM2_SR1_UIF) != TIM2_SR1_UIF); 
  }
  
  static void Delay_ms(uint16_t ms)
  {
    while(ms--)
    {
      Delay_us(989); // 989 works well for Milli Second delay
    }
  }
  
  static void IRQ_Handler(void)
  {
    TIM2->SR1 &= (uint8_t)(~TIM2_SR1_UIF);
    Ticks++;
  }  
};

template<uint8_t Prescalar, uint16_t Autoreload, uint8_t mode>
    uint16_t gpt2<Prescalar,Autoreload,mode>::Ticks;

}
#endif // gpt.h