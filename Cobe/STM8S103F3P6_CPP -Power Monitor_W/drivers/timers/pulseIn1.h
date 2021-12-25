/******************
** CLASS: pulseIn1
**
** DESCRIPTION:
**  Gpio input functionality
**
** CREATED: 25 Jan 2021, by Amit Chaudhary
**
** FILE: pulseIn1.h
**
******************/
#ifndef pulseIn1_h
#define pulseIn1_h

#include"system.h"
#include"gpt1.h"
#include"interrupt.h"
#include"optional_bytes.h"
#include"queue.h"
#include"DigitalIn.h"

template<Timer1_Channel_t Channel, 
         uint16_t tick = 15, uint16_t Autoreload = 0xffff, 
         TimerPol_t Polarity = TimerPol_Rising, uint8_t SampleSize = 8, uint8_t Filter = 0, 
         uint8_t ChannelPrescaler = 0>
           
class pulseIn1 : public gpt1<tick,Autoreload,0>
{
public:
  
  typedef gpt1<tick,Autoreload,0> Gpt;
  
  typedef union
  {
    uint16_t Word;
    uint8_t Array[2];
  }Capture_t;  
  
  typedef queue<uint16_t, uint8_t, SampleSize> queue_t;
  queue_t Q;
  
public:
  
  uint8_t HwInit(void)
  {
    uint8_t IC1Selection, IC2Selection;
    
    //static_assert( (Timerx != 0), "Error: Both edges should not be same");
    
    if(Channel == TIM1_CH1_C6_RMP)
    {
      IC1Selection = TIM1_ICSELECTION_DIRECTTI;
      IC2Selection = TIM1_ICSELECTION_INDIRECTTI;  
    }
    else
    {
      IC1Selection = TIM1_ICSELECTION_INDIRECTTI;
      IC2Selection = TIM1_ICSELECTION_DIRECTTI;     
    }
    
    Gpt::HwInit();
    
    GpioHwInit();
    
    Gpt::InterruptDisable(0xff); 
    
    if( (Channel == TIM1_CH1_C6_RMP) || (Channel == TIM1_CH2_C7_RMP) )
    {
      if(Channel == TIM1_CH1_C6_RMP)
      {        
        IC1Selection = TIM1_ICSELECTION_DIRECTTI;
        IC2Selection = TIM1_ICSELECTION_INDIRECTTI;  
      }
      else
      {
        IC1Selection = TIM1_ICSELECTION_INDIRECTTI;
        IC2Selection = TIM1_ICSELECTION_DIRECTTI;     
      }
      
      TIM1->CCER1 = 0;
      TIM1->CCMR1 = 0;
      TIM1->CCMR2 = 0;
      
      TIM1->CCMR1 |= (uint8_t)ChannelPrescaler | (uint8_t)(((uint8_t)(IC1Selection)) | ((uint8_t)(Filter << 4)));
      TIM1->CCMR2 |= (uint8_t)ChannelPrescaler | (uint8_t)(((uint8_t)(IC2Selection)) | ((uint8_t)(Filter << 4)));    
      
      if (Polarity == TimerPol_Falling)
      {
        TIM1->CCER1 |= TIM1_CCER1_CC1P | TIM1_CCER1_CC1E | TIM1_CCER1_CC2E;
      }
      else
      {
        TIM1->CCER1 |= TIM1_CCER1_CC2P | TIM1_CCER1_CC1E | TIM1_CCER1_CC2E ;
      }    
    }
    else  if( (Channel == TIM1_CH3_C3) || (Channel == TIM1_CH4_C4) )
    {
      if(Channel == TIM1_CH3_C3)
      {        
        IC1Selection = TIM1_ICSELECTION_DIRECTTI;
        IC2Selection = TIM1_ICSELECTION_INDIRECTTI;  
      }
      else
      {
        IC1Selection = TIM1_ICSELECTION_INDIRECTTI;
        IC2Selection = TIM1_ICSELECTION_DIRECTTI;     
      }
      
      TIM1->CCER2 = 0;
      TIM1->CCMR3 = 0;
      TIM1->CCMR4 = 0;
      
      TIM1->CCMR3 |= (uint8_t)ChannelPrescaler | (uint8_t)(((uint8_t)(IC1Selection)) | ((uint8_t)(Filter << 4)));
      TIM1->CCMR4 |= (uint8_t)ChannelPrescaler | (uint8_t)(((uint8_t)(IC2Selection)) | ((uint8_t)(Filter << 4)));    
      
      if (Polarity == TimerPol_Falling)
      {
        TIM1->CCER2 |= TIM1_CCER2_CC3P | TIM1_CCER2_CC3E | TIM1_CCER2_CC4E;
      }
      else
      {
        TIM1->CCER2 |= TIM1_CCER2_CC4P | TIM1_CCER2_CC3E | TIM1_CCER2_CC4E ;
      }      
    }
    else
    {
      // Error
      while(1);
    }
    
    Gpt::Start();
    
    return 0;
  }
  
  
  void GpioHwInit(void)
  {
    if(Channel == TIM1_CH1_C6_RMP)
    {
      DigitalIn<C6> PC6; 
      opt::EnableRemap(opt::C5_TIM2_CH1__C6_TIM1_CH1__C7_TIM1_CH2);
      PC6.HwInit();
    }
    else if(Channel == TIM1_CH2_C7_RMP)
    {
      DigitalIn<C7> PC7; 
      opt::EnableRemap(opt::C5_TIM2_CH1__C6_TIM1_CH1__C7_TIM1_CH2);
      PC7.HwInit();
    }
    else if(Channel == TIM1_CH3_C3)
    {
      DigitalIn<C3> PC3; 
      PC3.HwInit();
    }
    else if(Channel == TIM1_CH4_C4)
    {
      DigitalIn<C4> PC4; 
      PC4.HwInit();
    }
    else
    {
      // Error
    }
  }
  
  void Start(void)
  {
    if( (Channel == TIM1_CH1_C6_RMP) || (Channel == TIM1_CH2_C7_RMP) )
    {
      Gpt::InterruptEnable(TIM1_IER_CC2IE); //TIM1_IER_UIE
    }
    else
    {
      Gpt::InterruptEnable(TIM1_IER_CC4IE); //TIM1_IER_UIE
    }
  }
  
  void Stop(void)
  {
    if( (Channel == TIM1_CH1_C6_RMP) || (Channel == TIM1_CH2_C7_RMP) )
    {
      Gpt::InterruptDisable(TIM1_IER_CC2IE);
    }
    else
    {
      Gpt::InterruptDisable(TIM1_IER_CC4IE);
    }    
  }  
  
  void CaptureEnable(void)
  {
    TIM1->CCER1 |=  TIM1_CCER1_CC1E | TIM1_CCER1_CC2E;
  }
  
  void CaptureDisable(void)
  {
    TIM1->CCER1 &=  (uint8_t)~(TIM1_CCER1_CC1E | TIM1_CCER1_CC2E);
  }
    
  uint16_t GetCapture1(void)
  {
    Capture_t capture;
    
    capture.Array[0] = TIM1->CCR1H;
    capture.Array[1] = TIM1->CCR1L;
    
    return capture.Word;
  }
  
  uint16_t GetCapture2(void)
  {    
    Capture_t capture;
    
    capture.Array[0] = TIM1->CCR2H;
    capture.Array[1] = TIM1->CCR2L;
    return capture.Word;
  }
  
  uint16_t GetDiff(void)
  {
    uint16_t Cap1,Cap2;
    
    Cap1 = GetCapture1();
    Cap2 = GetCapture2();
    
    if(Cap1 >= Cap2)
    {
      return Cap1 - Cap2;
    }
    else
    {
      return Cap2 - Cap1 ; //0xffff - Cap2 + Cap1 ;
    }
  }
  
  // Interrupt time  = 1.92us
  void GetPulseWidth(void)
  {    
    Capture_t capture1,capture2;
    
    if( (Channel == TIM1_CH1_C6_RMP) || (Channel == TIM1_CH2_C7_RMP) )
    {
      capture1.Array[0] = TIM1->CCR1H;
      capture1.Array[1] = TIM1->CCR1L;
      
      capture2.Array[0] = TIM1->CCR2H;
      capture2.Array[1] = TIM1->CCR2L;
    }
    else
    {
      capture1.Array[0] = TIM1->CCR3H;
      capture1.Array[1] = TIM1->CCR3L;
      
      capture2.Array[0] = TIM1->CCR4H;
      capture2.Array[1] = TIM1->CCR4L;
    }
    
    Q.Write(capture2.Word - capture1.Word); // 3.68 us
  }
  
  uint16_t GetFrequency(void)
  {
    return 500000/GetDiff();
  }
  
  void IRQ_Handler(void)
  {
    GetPulseWidth();
  }
  
};




#endif // pulseIn1_h