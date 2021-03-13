


#include"adc_tests.h"


volatile static uint16_t AdcVal5,AdcVal6;
void adc_tests()
{
  volatile uint16_t adcval = 0xffff;
  
  adc<ADC_CH_5> Analog5;
  adc<ADC_CH_6> Analog6;
  
  Analog5.HwInit();
  Analog6.HwInit();
  while(1)
  {
    Analog5.Start();
    while(adcval == 0xffff)
    {
      adcval = Analog5.Read();
    }
    AdcVal5 = adcval;
    adcval = 0xffff;
    
    Analog6.Start();
    while(adcval == 0xffff)
    {
      adcval = Analog6.Read();
    }
    AdcVal6 = adcval;
    adcval = 0xffff;
   
    delay_ms(1);
  }
  
}