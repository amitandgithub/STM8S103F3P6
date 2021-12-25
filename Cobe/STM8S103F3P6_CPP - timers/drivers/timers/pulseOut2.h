/******************
** CLASS: pulseOut2
**
** DESCRIPTION:
**  Implements the pulseOut driver
**
** CREATED: 25 Jan 2021, by Amit Chaudhary
**
** FILE: pulseOut2.h
**
******************/
#ifndef pulseOut2_h
#define pulseOut2_h

#include"system.h"
#include"gpt2.h"
#include"interrupt.h"

#define GPT2_PARAMS 1,(uint16_t)(16000000/Frequency/2 - 1),0

template<Timer2_Channel_t Channel,uint32_t Frequency = 10000>

class pulseOut2 : public gpt2<GPT2_PARAMS>
{
  typedef gpt2<GPT2_PARAMS> Gpt;
protected:
  
  static const uint16_t ARR_F = (16000000/Frequency/2)/100;
public:
  
  uint8_t HwInit(void)
  {
    Gpt::HwInit();
    
    //GpioHwInit();
    
    if(Channel == Timer2_Channel_1)
    {
      DigitalOut<C6> PC6; 
      PC6.HwInit();
        
      TIM2->CCMR1 = (uint8_t)(0x60 | TIM2_CCMR_OCxPE); // 0x60 = PWM1
      
      TIM2->CCER1 |= TIM2_CCER1_CC1E;
    }      
    else if(Channel == Timer2_Channel_2)
    {
      DigitalOut<C7> PC7; 
      PC7.HwInit();
      
      TIM2->CCMR2 = (uint8_t)(0x60 | TIM2_CCMR_OCxPE); // 0x60 = PWM1
      
      TIM2->CCER1 |= TIM2_CCER1_CC2E;           
    }
    else if(Channel == Timer2_Channel_3)
    {
      DigitalOut<A3> PA3; 
      PA3.HwInit();
      
      TIM2->CCMR3 = (uint8_t)(0x60 | TIM2_CCMR_OCxPE); // 0x60 = PWM1
      
      TIM2->CCER2 |= TIM2_CCER2_CC3E;           
    }
//    else
//    {
//      DigitalOut<C4> PC4; 
//      PC4.HwInit();
//      
//      TIM2->CCMR4 = (uint8_t)(0x60 | TIM2_CCMR_OCxPE); // 0x60 = PWM1
//      
//      TIM2->CCER2 |= TIM2_CCER2_CC4E;           
//    }
    
    Gpt::EnableAutoLoadPreload();
    
    Gpt::InterruptDisable(0xff);
    
    SetDutyCycle(50);
      
    //TIM2->BKR |= TIM2_BKR_MOE;
    
    return 0;
  }

  void Start(void)
  {
    //Gpt::InterruptEnable(TIM2_IER_UIE | TIM2_IER_CC1IE); 
    Gpt::Start();
  }
  
  void Stop(void)
  {
    Gpt::Stop();
  }  
  
  void IRQ_Handler(void)
  {
    
  }
  
  void SetDutyCycle(uint8_t dutycycle)
  {
    uint16_t arr = ARR_F*(dutycycle%99); 
    
    if(Channel == Timer2_Channel_1)
    {
      TIM2->CCR1H = (uint8_t)( (uint16_t)arr >> 8);
      TIM2->CCR1L = (uint8_t)((uint16_t)arr); 
    }
    else if(Channel == Timer2_Channel_2)
    {
      TIM2->CCR2H = (uint8_t)( (uint16_t)arr >> 8);
      TIM2->CCR2L = (uint8_t)((uint16_t)arr); 
    }
  }      
  
  void SetFrequency(uint32_t Freq)
  {
    Gpt::SetAutoReloadRegister((16000000/Freq/2 - 1));
  }
  
};




#endif // pulseIn_h