/******************
** CLASS: pulseOut1
**
** DESCRIPTION:
**  Implements the pulseOut driver
**
** CREATED: 25 Jan 2021, by Amit Chaudhary
**
** FILE: pulseOut1.h
**
******************/
#ifndef pulseOut1_h
#define pulseOut1_h

#include"system.h"
#include"gpt1.h"
#include"interrupt.h"
#include"optional_bytes.h"

#define GPT_PARAMS 15,(uint16_t)(16000000/Frequency/2 - 1),1

template<Timer1_Channel_t Channel,uint32_t Frequency = 10000>

class pulseOut1 : public gpt1<GPT_PARAMS>
{
  typedef gpt1<GPT_PARAMS> Gpt;
protected:
  
  static const uint16_t ARR_F = (16000000/Frequency/2)/100;
public:
  
  uint8_t HwInit(void)
  {
    
    GpioHwInit();
    
    Gpt::HwInit();        
    
    if(Channel == TIM1_CH1_C6_RMP)
    {        
      TIM1->CCMR1 = (uint8_t)(0x60 | TIM1_CCMR_OCxPE); // 0x60 = PWM1
      
      TIM1->CCER1 |= TIM1_CCER1_CC1E;  
    }      
    else if(Channel == TIM1_CH2_C7_RMP)
    {
      TIM1->CCMR2 = (uint8_t)(0x60 | TIM1_CCMR_OCxPE); // 0x60 = PWM1
      
      TIM1->CCER1 |= TIM1_CCER1_CC2E;           
    }
    else if(Channel == TIM1_CH3_C3)
    {
      TIM1->CCMR3 = (uint8_t)(0x60 | TIM1_CCMR_OCxPE); // 0x60 = PWM1
      
      TIM1->CCER2 |= TIM1_CCER2_CC3E;           
    }
    else
    {
      TIM1->CCMR4 = (uint8_t)(0x60 | TIM1_CCMR_OCxPE); // 0x60 = PWM1
      
      TIM1->CCER2 |= TIM1_CCER2_CC4E;           
    }
    
    Gpt::EnableAutoLoadPreload();
    
    Gpt::InterruptDisable(0xff);
    
    SetDutyCycle(50);
      
    TIM1->BKR |= TIM1_BKR_MOE;
    
    return 0;
  }

  void GpioHwInit(void)
  {
    // IO mapping need to be changed from default to TIM1 for Ch1 and Ch1
//    if( ((Channel == TIM1_CH1_C6_RMP) || (Channel == TIM1_CH2_C7_RMP)) && ( (*((uint8_t*)0x4803) & 0x01) != 0x01) )
//    {
//      eeprom_Unlock();
//      if(eeprom_IsReady(250))
//      {
//        FLASH->CR2 = 0X80;
//        FLASH->NCR2 = 0X7F;
//        *((uint8_t*)0x4803) = (uint8_t)1; 
//        *((uint8_t*)0x4804) = (uint8_t)~1; 
//      }
//    }
     AFR Afr;
    
    if(Channel == TIM1_CH1_C6_RMP)
    {
      DigitalOut<C6> PC6; 
      Afr.EnableRemap(AFR::C5_TIM2_CH1__C6_TIM1_CH1__C7_TIM1_CH2);
      PC6.HwInit();
    }
    else if(Channel == TIM1_CH2_C7_RMP)
    {
      DigitalOut<C7> PC7; 
      Afr.EnableRemap(AFR::C5_TIM2_CH1__C6_TIM1_CH1__C7_TIM1_CH2);
      PC7.HwInit();
    }
    else if(Channel == TIM1_CH3_C3)
    {
      DigitalOut<C3> PC3; 
      PC3.HwInit();
    }
    else if(Channel == TIM1_CH4_C4)
    {
      DigitalOut<C4> PC4; 
      PC4.HwInit();
    }
    else
    {
      // Error
    }
  }
  
  void Start(void)
  {
    //Gpt::InterruptEnable(TIM1_IER_UIE | TIM1_IER_CC1IE); 
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
    
    if(Channel == TIM1_CH1_C6_RMP)
    {
      TIM1->CCR1H = (uint8_t)( (uint16_t)arr >> 8);
      TIM1->CCR1L = (uint8_t)((uint16_t)arr); 
    }
    else if(Channel == TIM1_CH2_C7_RMP)
    {
      TIM1->CCR2H = (uint8_t)( (uint16_t)arr >> 8);
      TIM1->CCR2L = (uint8_t)((uint16_t)arr); 
    }
    else if(Channel == TIM1_CH3_C3)
    {
      TIM1->CCR3H = (uint8_t)( (uint16_t)arr >> 8);
      TIM1->CCR3L = (uint8_t)((uint16_t)arr); 
    }
    else
    {
      TIM1->CCR4H = (uint8_t)( (uint16_t)arr >> 8);
      TIM1->CCR4L = (uint8_t)((uint16_t)arr); 
    }
  }      
  
  void SetFrequency(uint32_t Freq)
  {
    Gpt::SetAutoReloadRegister((16000000/Freq/2 - 1));
  }
  
};




#endif // pulseIn_h