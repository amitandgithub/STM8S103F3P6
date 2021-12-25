


#include"adc_tests.h"


volatile static uint16_t AdcVal2,AdcVal3,AdcVal4,AdcVal5,AdcVal6;


void adc_tests()
{
  volatile uint16_t adcval = 0xffff;
  adc<ADC_CH_5> Analog5;
 
  Analog5.HwInit();
  
  while(1)
  {    
    if(Analog5.ConversionComplete())
    {      
      TestGPIO.High();
      AdcVal2 = Analog5.Read(ADC_CH_2);   
      AdcVal3 = Analog5.Read(ADC_CH_3);
      AdcVal4 = Analog5.Read(ADC_CH_4);   
      AdcVal5 = Analog5.Read(ADC_CH_5);
      AdcVal6 = Analog5.Read(ADC_CH_6);   
      Analog5.ClearEOC();
      TestGPIO.Low();
      Analog5.PowerOn();
    }
    //delay_ms(1);
  }
  
}

#if 0

void adc_tests()
{
  volatile uint16_t adcval = 0xffff;
  adc<ADC_CH_5> Analog5;
  
 
  Analog5.HwInit();
  Analog5.SetAlignment(ADC_Align_RIGHT);
  Analog5.SetMode(ADC_Mode_Continous);
  
  while(1)
  {    
    if(Analog5.ConversionComplete())
    {      
      TestGPIO.High();
      AdcVal5 = Analog5.Read();   
      TestGPIO.Low();
    }
    //delay_ms(1);
  }
  
}


#endif




















