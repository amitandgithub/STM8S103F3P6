/******************
** CLASS: pulseIn2
**
** DESCRIPTION:
**  Gpio input functionality
**
** CREATED: 25 Jan 2021, by Amit Chaudhary
**
** FILE: pulseIn2.h
**
******************/
#ifndef pulseIn2_h
#define pulseIn2_h

#include"system.h"
#include"gpt2.h"
#include"interrupt.h"
#include"optional_bytes.h"
#include"queue.h"
#include"DigitalIn.h"

template<Timer2_Channel_t Channel, 
         uint16_t tick = 4, uint16_t Autoreload = 0xffff, 
         TimerPol_t Polarity = TimerPol_Rising, uint8_t SampleSize = 8, uint8_t Filter = 0, 
         uint8_t ChannelPrescaler = 0>
           
class pulseIn2 : public gpt2<tick,Autoreload,0>
{
  
public:
  
  typedef gpt2<tick,Autoreload,0> Gpt;
  
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
    
    Gpt::HwInit();
    
    Gpt::InterruptDisable(0xff);    
    
    GpioHwInit();
    
    TIM2->CCER1 = 0;
    TIM2->CCMR1 = 0;
    TIM2->CCMR2 = 0;
    
    if( (Channel == TIM2_CH1_D4)  || (Channel == TIM2_CH1_C5_RMP) )
    {
      IC1Selection = TIM2_ICSELECTION_DIRECTTI;
      IC2Selection = TIM2_ICSELECTION_INDIRECTTI;
      
      TIM2->CCMR1 |= (uint8_t)ChannelPrescaler | (uint8_t)(((uint8_t)(IC1Selection)) | ((uint8_t)(Filter << 4)));
      TIM2->CCMR2 |= (uint8_t)ChannelPrescaler | (uint8_t)(((uint8_t)(IC2Selection)) | ((uint8_t)(Filter << 4)));    
    }
    else
    {
      IC1Selection = TIM2_ICSELECTION_INDIRECTTI;
      IC2Selection = TIM2_ICSELECTION_DIRECTTI;  
      
      TIM2->CCMR1 |= (uint8_t)ChannelPrescaler | (uint8_t)(((uint8_t)(IC1Selection)) | ((uint8_t)(Filter << 4)));
      TIM2->CCMR2 |= (uint8_t)ChannelPrescaler | (uint8_t)(((uint8_t)(IC2Selection)) | ((uint8_t)(Filter << 4)));    
    }   
    
    if (Polarity == TimerPol_Falling)
    {
      TIM2->CCER1 |= TIM2_CCER1_CC1P | TIM2_CCER1_CC1E | TIM2_CCER1_CC2E;
    }
    else
    {
      TIM2->CCER1 |= TIM2_CCER1_CC2P | TIM2_CCER1_CC1E | TIM2_CCER1_CC2E ;
    }          
    
    Gpt::Start();
    
    return 0;
  }
  
  void GpioHwInit(void)
  {
    if(Channel == TIM2_CH1_D4)
    {
      DigitalIn<D4> PD4; 
      PD4.HwInit();
    }
    else if(Channel == TIM2_CH1_C5_RMP)
    {
      DigitalIn<C5> PC5; 
      opt::EnableRemap(opt::C5_TIM2_CH1__C6_TIM1_CH1__C7_TIM1_CH2);
      PC5.HwInit();
    }
    else if(Channel == TIM2_CH2_D3)
    {
      DigitalIn<D3> PD3; 
      PD3.HwInit();
    }
    else
    {
      // Error
    }
  }
  
  void Start(void)
  {
    Gpt::InterruptEnable(TIM2_IER_CC2IE); //TIM2_IER_UIE
  }
  
  void Stop(void)
  {
    Gpt::InterruptDisable(TIM2_IER_CC2IE);
  }  
  
  void CaptureEnable(void)
  {
    TIM2->CCER1 |=  TIM2_CCER1_CC1E | TIM2_CCER1_CC2E;
  }
  
  void CaptureDisable(void)
  {
    TIM2->CCER1 &=  (uint8_t)~(TIM2_CCER1_CC1E | TIM2_CCER1_CC2E);
  }
    
  uint16_t GetCapture1(void)
  {
    Capture_t capture;
    
    capture.Array[0] = TIM2->CCR1H;
    capture.Array[1] = TIM2->CCR1L;
    
    return capture.Word;
  }
  
  uint16_t GetCapture2(void)
  {    
    Capture_t capture;
    
    capture.Array[0] = TIM2->CCR2H;
    capture.Array[1] = TIM2->CCR2L;
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
    
    capture1.Array[0] = TIM2->CCR1H;
    capture1.Array[1] = TIM2->CCR1L;
    
    capture2.Array[0] = TIM2->CCR2H;
    capture2.Array[1] = TIM2->CCR2L;
    
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




#endif // pulseIn2_h