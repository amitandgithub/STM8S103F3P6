/******************
** CLASS: pulseOut2
**
** DESCRIPTION:
**  Implements the pulseOut driver
**
** CREATED: 5 Feb 2021, by Amit Chaudhary
**
** FILE: pulseOut2.h
**
******************/
#ifndef pulseOut2_h
#define pulseOut2_h

#include"system.h"
#include"gpt2.h"
#include"queue.h"
#include"GpioIn.h"

#define GPT2_PARAMS 1,(uint16_t)(16000000/Frequency/2 - 1),0

namespace HAL
{
  
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
    
    GpioHwInit();
    
    if( (Channel == TIM2_CH1_D4) || (Channel == TIM2_CH1_C5_RMP))
    {
      TIM2->CCMR1 = (uint8_t)(0x60 | TIM2_CCMR_OCxPE); // 0x60 = PWM1
      
      TIM2->CCER1 |= TIM2_CCER1_CC1E;
    }      
    else if(Channel == TIM2_CH2_D3)
    {      
      TIM2->CCMR2 = (uint8_t)(0x60 | TIM2_CCMR_OCxPE); // 0x60 = PWM1
      
      TIM2->CCER1 |= TIM2_CCER1_CC2E;           
    }
    else if( (Channel == TIM2_CH3_A3) || (Channel == TIM2_CH3_D2_RMP))
    {      
      TIM2->CCMR3 = (uint8_t)(0x60 | TIM2_CCMR_OCxPE); // 0x60 = PWM1
      
      TIM2->CCER2 |= TIM2_CCER2_CC3E;           
    }
    else
    {
      // Error
    }
    
    Gpt::EnableAutoLoadPreload();
    
    Gpt::InterruptDisable(Gpt::ALL);
    
    SetDutyCycle(50);
      
    //TIM2->BKR |= TIM2_BKR_MOE;
    
    return 0;
  }

  void GpioHwInit(void)
  {
    if(Channel == TIM2_CH1_D4)
    {
       GpioIn<D4> PD4; 
       //opt::DisableRemap(opt::C5_TIM2_CH1__C6_TIM1_CH1__C7_TIM1_CH2);
       PD4.HwInit();
    }
    else if(Channel == TIM2_CH1_C5_RMP)
    {
       GpioIn<C5> PC5; 
       //opt::EnableRemap(opt::C5_TIM2_CH1__C6_TIM1_CH1__C7_TIM1_CH2);
       PC5.HwInit();
    }
    else if(Channel == TIM2_CH2_D3)
    {
      GpioIn<D3> PD3; 
      PD3.HwInit();
    }
    else if(Channel == TIM2_CH3_A3)
    {
      GpioIn<A3> PA3; 
      //opt::DisableRemap(opt::A3_SPI_NSS__D2_TIM2_CH3);
      PA3.HwInit();
    }
    else if(Channel == TIM2_CH3_D2_RMP)
    {     
      GpioIn<D2> PD2;       
      //opt::EnableRemap(opt::A3_SPI_NSS__D2_TIM2_CH3);
      PD2.HwInit();
    }
    else
    {
      // Error
    }
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
    
    if( (Channel == TIM2_CH1_D4) || (Channel == TIM2_CH1_C5_RMP))
    {
      TIM2->CCR1H = (uint8_t)( (uint16_t)arr >> 8);
      TIM2->CCR1L = (uint8_t)((uint16_t)arr); 
    }
    else if(Channel == TIM2_CH2_D3)
    {
      TIM2->CCR2H = (uint8_t)( (uint16_t)arr >> 8);
      TIM2->CCR2L = (uint8_t)((uint16_t)arr); 
    }
    else if( (Channel == TIM2_CH3_A3) || (Channel == TIM2_CH3_D2_RMP))
    {
      TIM2->CCR3H = (uint8_t)( (uint16_t)arr >> 8);
      TIM2->CCR3L = (uint8_t)((uint16_t)arr); 
    }
  }      
  
  void SetFrequency(uint32_t Freq)
  {
    Gpt::SetAutoReloadRegister((16000000/Freq/2 - 1));
  }
  
};

}


#endif // pulseIn_h