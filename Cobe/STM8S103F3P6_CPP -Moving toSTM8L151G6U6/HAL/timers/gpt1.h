/******************
** CLASS: gpt1
**
** DESCRIPTION:
**  Gpio input functionality
**
** CREATED: 25 Jan 2021, by Amit Chaudhary
**
** FILE: gpt1.h
**
******************/
#ifndef gpt1_h
#define gpt1_h

#include"system.h"
//#include"eeprom.h"

namespace HAL
{
  
typedef enum
{
  TIM1_CH1_C6_RMP,
  TIM1_CH2_C7_RMP,
  TIM1_CH3_C3,
  TIM1_CH4_C4
}Timer1_Channel_t;

typedef enum{ TimerPol_Rising = 0, TimerPol_Falling = 1 } TimerPol_t;



template<uint16_t Prescalar, uint16_t Autoreload, uint8_t mode>
class gpt1
{ 
  static uint16_t Ticks;  
  
  typedef union
  {
    uint16_t Word;
    uint8_t Array[2];
  }Capture_t;
  
public:  
  
  typedef enum
  {
    BIE         = (uint8_t)TIM1_IER_BIE,
    TIE         = (uint8_t)TIM1_IER_TIE,
    CC1IE       = (uint8_t)TIM1_IER_CC1IE,
    CC2IE       = (uint8_t)TIM1_IER_CC2IE,
    CC3IE       = (uint8_t)TIM1_IER_CC3IE,
    CC4IE       = (uint8_t)TIM1_IER_CC4IE,
    UIE         = (uint8_t)TIM1_IER_UIE,
    ALL         = (uint8_t)0xFF
  }Interrupt_t;
  
  static uint8_t HwInit(void)
  {
    disableInterrupts();
    
    CLK->PCKENR1 |= (uint8_t) CLK_PCKENR1_TIM1;    
    
    TIM1->SR1 = (uint8_t)0x00;
    TIM1->SR2 = (uint8_t)0x00;

    TIM1->ARRH = (uint8_t)(Autoreload >> 8);
    TIM1->ARRL = (uint8_t)(Autoreload);

    /* Set the Prescaler value 
      fCK_CNT = fCK_PSC/(PSCR[15:0]+1) */
    TIM1->PSCRH = (uint8_t)(Prescalar >> 8);
    TIM1->PSCRL = (uint8_t)(Prescalar);
    
    TIM1->CR1 = mode;
    
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
  static void InterruptEnable(Interrupt_t intterupts)
  {
    TIM1->IER |= intterupts;
  }
  
  static void InterruptDisable(Interrupt_t intterupts)
  {
    TIM1->IER &= (uint8_t)(~(uint8_t)intterupts);
  }

  static void OnePulseModeEnable(void)
  {
    TIM1->CR1 |= TIM1_CR1_OPM;
  }
  
  static void OnePulseModeDisable(void)
  {
    TIM1->CR1 &= (uint8_t)(~TIM1_CR1_OPM);
  }

  static void GeneratUpdateEvent(void)
  {
    TIM1->EGR |= (uint8_t)TIM1_EGR_UG;
  }
  
  static void SetAutoReloadRegister(uint16_t Val)
  {
    TIM1->ARRH = (uint8_t)(Val >> 8);
    TIM1->ARRL = (uint8_t)(Val);
  }  
  
  static void EnableAutoLoadPreload(void)
  {
    TIM1->CR1 |= (uint8_t)(TIM1_CR1_ARPE);    
  }  
  
  static void DisableAutoLoadPreload(void)
  {
    TIM1->CR1 &= (uint8_t)(~TIM1_CR1_ARPE);    
  }
  
  static void DisableUpdateEvent(void)
  {
    TIM1->CR1 |= TIM1_CR1_UDIS;
  }
  
  static void EnableUpdateEvent(void)
  {
    TIM1->CR1 &= (uint8_t)(~TIM1_CR1_UDIS);
  }
  
  static void Start(void)
  {
    EnableUpdateEvent();
    //InterruptEnable(TIM_IER_UIE);
    GeneratUpdateEvent();
    TIM1->CR1 |= TIM1_CR1_CEN;
  }
  
  static void Stop(void)
  {
    //InterruptDisable(TIM_IER_UIE);
    TIM1->CR1 &= (uint8_t)(~TIM1_CR1_CEN);
  }
  
  static uint16_t GetTicks(void)
  {
//    uint16_t ticks;
//    
//    disableInterrupts();
//    ticks = Ticks;
//    enableInterrupts();
//    return ticks;
    return GetCounter();
  }
  
  static uint16_t GetCounter(void)
  {
    Capture_t Capture;
    
    Capture.Array[0] = TIM1->CNTRH;
    Capture.Array[1] = TIM1->CNTRL;
    
    return Capture.Word;    
  } 
  
  static void Delay_us(uint16_t us)
  {
    TIM1->ARRH = (uint8_t)(us >> 8);
    TIM1->ARRL = (uint8_t)(us);
    TIM1->SR1 = 0;
    TIM1->CR1 |= TIM1_CR1_CEN;
    while(( TIM1->SR1 & TIM1_SR1_UIF) != TIM1_SR1_UIF); 
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
    TIM1->SR1 &= (uint8_t)(~TIM1_SR1_UIF);
    Ticks++;
  }  
};

template<uint16_t Prescalar, uint16_t Autoreload, uint8_t mode>
    uint16_t gpt1<Prescalar,Autoreload,mode>::Ticks;

}
#endif // gpt.h