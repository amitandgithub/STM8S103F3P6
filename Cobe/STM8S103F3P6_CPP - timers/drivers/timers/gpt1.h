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
#include"eeprom.h"

typedef enum{ Timer_Channel_1 = 0, Timer_Channel_2 = 1  } Timer_Channel_t;
typedef enum{ TimerPol_Rising = 0, TimerPol_Falling = 1 } TimerPol_t;

template<uint16_t Prescalar, uint16_t Autoreload, uint8_t mode>
class gpt1
{ 
  uint32_t Ticks;  
  
  typedef union
  {
    uint16_t Word;
    uint8_t Array[2];
  }Capture_t;
  
public:
  
  
  
  uint8_t HwInit(void)
  {
    disableInterrupts();
    
    CLK->PCKENR1 |= (uint8_t) CLK_PCKENR1_TIM1;    
    
    TIM1->SR1 = (uint8_t)0x00;
    TIM1->SR2 = (uint8_t)0x00;

    TIM1->ARRH = (uint8_t)(Autoreload >> 8);
    TIM1->ARRL = (uint8_t)(Autoreload);

    /* Set the Prescaler value */
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
  void InterruptEnable(uint8_t intterupts)
  {
    TIM1->IER |= intterupts;
  }
  
  void InterruptDisable(uint8_t intterupts)
  {
    TIM1->IER &= (uint8_t)(~(uint8_t)intterupts);
  }

  void OnePulseModeEnable(void)
  {
    TIM1->CR1 |= TIM1_CR1_OPM;
  }
  
  void OnePulseModeDisable(void)
  {
    TIM1->CR1 &= (uint8_t)(~TIM1_CR1_OPM);
  }

  void GeneratUpdateEvent(void)
  {
    TIM1->EGR |= (uint8_t)TIM1_EGR_UG;
  }
  
  void SetAutoReloadRegister(uint16_t Val)
  {
    TIM1->ARRH = (uint8_t)(Val >> 8);
    TIM1->ARRL = (uint8_t)(Val);
  }  
  
  void EnableAutoLoadPreload(void)
  {
    TIM1->CR1 |= (uint8_t)(TIM1_CR1_ARPE);    
  }  
  
  void DisableAutoLoadPreload(void)
  {
    TIM1->CR1 &= (uint8_t)(~TIM1_CR1_ARPE);    
  }
  
  void DisableUpdateEvent(void)
  {
    TIM1->CR1 |= TIM1_CR1_UDIS;
  }
  
  void EnableUpdateEvent(void)
  {
    TIM1->CR1 &= (uint8_t)(~TIM1_CR1_UDIS);
  }
  
  void Start(void)
  {
    EnableUpdateEvent();
    //InterruptEnable(TIM_IER_UIE);
    GeneratUpdateEvent();
    TIM1->CR1 |= TIM1_CR1_CEN;
  }
  
  void Stop(void)
  {
    //InterruptDisable(TIM_IER_UIE);
    TIM1->CR1 &= (uint8_t)(~TIM1_CR1_CEN);
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
    
    Capture.Array[0] = TIM1->CNTRH;
    Capture.Array[1] = TIM1->CNTRL;
    
    return Capture.Word;    
  } 
  
  void Delay_us(uint16_t us)
  {
    TIM1->ARRH = (uint8_t)(us >> 8);
    TIM1->ARRL = (uint8_t)(us);
    TIM1->SR1 = 0;
    TIM1->CR1 |= TIM1_CR1_CEN;
    while(( TIM1->SR1 & TIM1_SR1_UIF) != TIM1_SR1_UIF); 
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
    TIM1->SR1 &= (uint8_t)(~TIM1_SR1_UIF);
    Ticks++;
  }  
};

#endif // gpt.h