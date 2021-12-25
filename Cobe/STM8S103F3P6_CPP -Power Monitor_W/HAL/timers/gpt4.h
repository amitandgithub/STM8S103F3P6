/******************
** CLASS: DigitalIn
**
** DESCRIPTION:
**  Gpio input functionality
**
** CREATED: 29-12-2020, by Amit Chaudhary
**
** FILE: gpt4.h
**
******************/
#ifndef gpt4_h
#define gpt4_h

#include"system.h"

namespace HAL
{

  //fCK_CNT = fCK_PSC/2^(PSCR[2:0])
  typedef enum
  { 
    gpt4_tick_125ns     = 1, // Main clock divide by 2  = 8Mhz
    gpt4_tick_250ns     = 2, // Main clock divide by 4  = 4Mhz
    gpt4_tick_500ns     = 3, // Main clock divide by 8  = 2Mhz
    gpt4_tick_1us       = 4, // Main clock divide by 16 = 1Mhz
    gpt4_tick_2us       = 5, // Main clock divide by 32 = 500Khz
    gpt4_tick_4us       = 6, // Main clock divide by 64 = 250Khz
    gpt4_tick_8us       = 7  // Main clock divide by 128 = 125Khz     
  } gpt4_tick_t;
  
  template<gpt4_tick_t Prescalar, uint8_t Autoreload, uint8_t mode = 0>
    class gpt4
    {
      static uint16_t Ticks;
    public:
      
      enum
      {
        UIE = (uint8_t)TIM4_IER_UIE,
      };
      static uint8_t HwInit(void)
      {
        disableInterrupts();
        
        CLK->PCKENR1 |= (uint8_t) CLK_PCKENR1_TIM4;    
        TIM4->SR1 = 0;
        TIM4->ARR = Autoreload;
        TIM4->PSCR = Prescalar;
        TIM4->CR1 = TIM4_CR1_ARPE | mode;
        TIM4->EGR |= TIM4_EGR_UG;
        
        enableInterrupts();
        
        return 0;
      }
      
      static void InterruptEnable(uint8_t intterupts)
      {
        TIM4->IER = intterupts;//TIM4_IER_UIE;
      }
      
      static void InterruptDisable(uint8_t intterupts)
      {
        TIM4->IER = 0;
      }
      
      static void Start(void)
      {
        TIM4->CR1 |= TIM4_CR1_CEN;
      }
      
      static void Stop(void)
      {
        TIM4->IER = 0;
        TIM4->CR1 &= (uint8_t)(~TIM4_CR1_CEN);
      }
      
      static uint16_t GetTicks(void)
      {    
        uint16_t ticks;
        
        disableInterrupts();
        ticks = Ticks;
        enableInterrupts();
        return ticks;
    
      }
      
      static uint8_t GetCounter(void){ return TIM4->CNTR;} 
      
      // This delay will work for max of 255 us 
      // if you need more delay then call this function multiple times
      static void Delay_us(uint8_t us)
      {
        //PB1.High();
        TIM4->ARR = us;
        TIM4->SR1 = 0;
        TIM4->CR1 |= TIM4_CR1_CEN;
        //PB1.Low();
        while(TIM4->SR1 != 0x01); 
        //PB1.High();
      }
      
      static void Delay_US(uint16_t us)
      {
        uint16_t US = us;
        do
        {
          if(US > 255)
          {
            Delay_us(255-1); // 1 us for compensation
            US = US - 255;
          }
          else
          {
            Delay_us(US-1);  // 1 us for compensation
            US = 0;
          }             
        }while(US);
      }
      
      static void Delay_ms(uint16_t ms)
      {
        while(ms--)
        {
          Delay_us(250);
          Delay_us(250);
          Delay_us(250);
          Delay_us(250 - 3);
        }
      }
      
      static void DelayTicks(uint8_t  ticks)
      {
        uint8_t start, end;
        start = GetCounter();
        end = start + ticks;
        if (start < end)
        {
          while ((GetCounter() >= start) && (GetCounter() < end))
          {
            // do nothing
          }
        }
        else
        {
          while ((GetCounter() >= start) || (GetCounter() < end))
          {
            // do nothing
          }
        }
      }  
      // uint8_t Ticks++ = 0.32 micro s
      // uint16_t Ticks++ = 0.48 micro s + 3b ROM
      static void IRQ_Handler(void)
      {
        //TestGPIO.High();
        TIM4->SR1 = (uint8_t)0x00;
        Ticks++;        
       //TestGPIO.Low();
      }
      
    };
  template<gpt4_tick_t Prescalar, uint8_t Autoreload, uint8_t mode>
    uint16_t gpt4<Prescalar,Autoreload,mode>::Ticks;
  
}

#endif // gpt4_h