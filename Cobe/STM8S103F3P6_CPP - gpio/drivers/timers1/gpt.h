/******************
** CLASS: DigitalIn
**
** DESCRIPTION:
**  Gpio input functionality
**
** CREATED: 29-12-2020, by Amit Chaudhary
**
** FILE: gpt.h
**
******************/
#ifndef gpt_h
#define gpt_h

#include"system.h"

extern uint32_t TIM2Ticks;
extern uint32_t TIM3Ticks;

typedef enum{ Timer_Channel_1 = 0, Timer_Channel_2 = 1  } Timer_Channel_t;
typedef enum{ TimerPol_Rising = 0, TimerPol_Falling = 1 } TimerPol_t;

typedef enum
{
  TIMER2 = TIM2_BaseAddress,
  TIMER3 = TIM3_BaseAddress
}TIMx_t;

typedef union
{
  uint16_t Word;
  uint8_t Array[2];
}Capture_t;

typedef enum{ Tick_125ns=1,Tick_250ns=2,Tick_500ns=3,Tick_1us=4,Tick_2us=5,Tick_4us=6,Tick_8us=7}Tick_t;

template<TIMx_t Timerx, Tick_t tick, uint16_t Autoreload, uint8_t mode>
class gpt
{
  uint32_t* pTicks;
public:
  
  uint8_t HwInit(void)
  {
    disableInterrupts();
    
    if(((TIM_TypeDef*)Timerx) == TIM2)
    {
      pTicks = &TIM2Ticks;
      CLK->PCKENR1 |= (uint8_t) CLK_PCKENR1_TIM2;
    }
    else if(((TIM_TypeDef*)Timerx) == TIM3)
    {
      pTicks = &TIM3Ticks;
      CLK->PCKENR1 |= (uint8_t) CLK_PCKENR1_TIM3;
    }
    else
    {
      return 0xEE;
    }    
    
    ((TIM_TypeDef*)Timerx)->SR1 = (uint8_t)0x00;
    ((TIM_TypeDef*)Timerx)->SR2 = (uint8_t)0x00;

    ((TIM_TypeDef*)Timerx)->ARRH = (uint8_t)(Autoreload >> 8);
    ((TIM_TypeDef*)Timerx)->ARRL = (uint8_t)(Autoreload);

    /* Set the Prescaler value */
    ((TIM_TypeDef*)Timerx)->PSCR = (uint8_t)(tick);
    
    ((TIM_TypeDef*)Timerx)->CR1 = mode;
    
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
    ((TIM_TypeDef*)Timerx)->IER |= intterupts;
  }
  
  void InterruptDisable(uint8_t intterupts)
  {
    ((TIM_TypeDef*)Timerx)->IER &= (uint8_t)(~(uint8_t)intterupts);
  }

  void OnePulseModeEnable(void)
  {
    ((TIM_TypeDef*)Timerx)->CR1 |= TIM_CR1_OPM;
  }
  
  void OnePulseModeDisable(void)
  {
    ((TIM_TypeDef*)Timerx)->CR1 &= (uint8_t)(~TIM_CR1_OPM);
  }

  void GeneratUpdateEvent(void)
  {
    ((TIM_TypeDef*)Timerx)->EGR |= (uint8_t)TIM_EGR_UG;
  }
  
  void SetAutoReloadRegister(uint16_t Val)
  {
    ((TIM_TypeDef*)Timerx)->ARRH = (uint8_t)(Val >> 8);
    ((TIM_TypeDef*)Timerx)->ARRL = (uint8_t)(Val);
  }  
  
  void EnableAutoLoadPreload(void)
  {
    ((TIM_TypeDef*)Timerx)->CR1 |= (uint8_t)(TIM_CR1_ARPE);    
  }  
  
  void DisableAutoLoadPreload(void)
  {
    ((TIM_TypeDef*)Timerx)->CR1 &= (uint8_t)(~TIM_CR1_ARPE);    
  }
  
  void DisableUpdateEvent(void)
  {
    ((TIM_TypeDef*)Timerx)->CR1 |= TIM_CR1_UDIS;
  }
  
  void EnableUpdateEvent(void)
  {
    ((TIM_TypeDef*)Timerx)->CR1 &= (uint8_t)(~TIM_CR1_UDIS);
  }
  
  void Start(void)
  {
    EnableUpdateEvent();
    //InterruptEnable(TIM_IER_UIE);
    GeneratUpdateEvent();
    ((TIM_TypeDef*)Timerx)->CR1 |= TIM_CR1_CEN;
  }
  
  void Stop(void)
  {
    //InterruptDisable(TIM_IER_UIE);
    ((TIM_TypeDef*)Timerx)->CR1 &= (uint8_t)(~TIM_CR1_CEN);
  }
  
  uint32_t GetTicks(void)
  {
    uint32_t ticks;
    
    disableInterrupts();
    ticks = *pTicks;
    enableInterrupts();
    return ticks;
  }
  
  uint16_t GetCounter(void)
  {
    Capture_t Capture;
    
    Capture.Array[0] = ((TIM_TypeDef*)Timerx)->CNTRH;
    Capture.Array[1] = ((TIM_TypeDef*)Timerx)->CNTRL;
    
    return Capture.Word;    
  } 
  
  void Delay_us(uint16_t us)
  {
    //PB1.High();
    ((TIM_TypeDef*)Timerx)->ARRH = (uint8_t)(us >> 8);
    ((TIM_TypeDef*)Timerx)->ARRL = (uint8_t)(us);
    ((TIM_TypeDef*)Timerx)->SR1 = 0;
    ((TIM_TypeDef*)Timerx)->CR1 |= TIM_CR1_CEN;
    //PB1.Low();
    while(( ((TIM_TypeDef*)Timerx)->SR1 & TIM_SR1_UIF) != TIM_SR1_UIF); 
    //PB1.High();
  }
  
  void Delay_ms(uint16_t ms)
  {
    while(ms--)
    {
      Delay_us(1000);
    }
  }
  
  
};
#endif // gpt.h