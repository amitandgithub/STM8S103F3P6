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
    Gpt::HwInit();
    
    // IO mapping need to be changed from default to TIM1 for Ch1 and Ch1
    if( ((Channel == Timer1_Channel_1) || (Channel == Timer1_Channel_2)) && ( (*((uint8_t*)0x4803) & 0x01) != 0x01) )
    {
      eeprom_Unlock();
      if(eeprom_IsReady(250))
      {
        FLASH->CR2 = 0X80;
        FLASH->NCR2 = 0X7F;
        *((uint8_t*)0x4803) = (uint8_t)1; 
        *((uint8_t*)0x4804) = (uint8_t)~1; 
      }
    }    
    
    if(Channel == Timer1_Channel_1)
    {
      DigitalOut<C6> PC6; 
      PC6.HwInit();
        
      TIM1->CCMR1 = (uint8_t)(0x60 | TIM1_CCMR_OCxPE); // 0x60 = PWM1
      
      TIM1->CCER1 |= TIM1_CCER1_CC1E;  
    }      
    else if(Channel == Timer1_Channel_2)
    {
      DigitalOut<C7> PC7; 
      PC7.HwInit();
      
      TIM1->CCMR2 = (uint8_t)(0x60 | TIM1_CCMR_OCxPE); // 0x60 = PWM1
      
      TIM1->CCER1 |= TIM1_CCER1_CC2E;           
    }
    else if(Channel == Timer1_Channel_3)
    {
      DigitalOut<C3> PC3; 
      PC3.HwInit();
      
      TIM1->CCMR3 = (uint8_t)(0x60 | TIM1_CCMR_OCxPE); // 0x60 = PWM1
      
      TIM1->CCER2 |= TIM1_CCER2_CC3E;           
    }
    else
    {
      DigitalOut<C4> PC4; 
      PC4.HwInit();
      
      TIM1->CCMR4 = (uint8_t)(0x60 | TIM1_CCMR_OCxPE); // 0x60 = PWM1
      
      TIM1->CCER2 |= TIM1_CCER2_CC4E;           
    }
    
    Gpt::EnableAutoLoadPreload();
    
    Gpt::InterruptDisable(0xff);
    
    SetDutyCycle(50);
      
    TIM1->BKR |= TIM1_BKR_MOE;
    

    
    return 0;
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
    
    if(Channel == Timer1_Channel_1)
    {
      TIM1->CCR1H = (uint8_t)( (uint16_t)arr >> 8);
      TIM1->CCR1L = (uint8_t)((uint16_t)arr); 
    }
    else if(Channel == Timer1_Channel_2)
    {
      TIM1->CCR2H = (uint8_t)( (uint16_t)arr >> 8);
      TIM1->CCR2L = (uint8_t)((uint16_t)arr); 
    }
    else if(Channel == Timer1_Channel_3)
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