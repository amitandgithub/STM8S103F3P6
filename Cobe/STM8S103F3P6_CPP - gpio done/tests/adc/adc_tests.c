


#include"adc_tests.h"

adc_t Analog5;

void EOC_Handler(void)
{
  
}

uint16_t AdcVal2,AdcVal3,AdcVal4,AdcVal5,AdcVal6;


void adc_tests(void)
{
  adc_test_Single();
  //adc_test_SingleScan();
  
}


void adc_test_Single(void)
{
  //adc<ADC_Mode_Sngle,ADC_CH_5> Analog5;
  
  Analog5.HwInit();
  Analog5.PowerOn();  
  Analog5.AnalogWatchDogConfig(ADC_CH_5,650, 900, Adc_AWD_Callback );
  Analog5.StartConversion();
  while(1)
  {
    
    if(Analog5.ConversionComplete())
    {
      AdcVal5 = ADC_VOLTAGE(Analog5.Read()); //Analog5.Read();
      Analog5.ClearEOC();
      Analog5.StartConversion();
    }
  }  
}

void adc_test_SingleScan(void)
{
  //adc<ADC_Mode_Single_Scan,ADC_CH_6> Analog5;
 
  Analog5.HwInit();
  Analog5.AnalogWatchDogConfig(ADC_CH_5,650, 900, Adc_AWD_Callback );
  Analog5.PowerOn();  
  Analog5.StartConversion();
  
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
      Analog5.StartConversion();
    }
    //delay_ms(1);
  }  
}


void Adc_AWD_Callback(void)
{
  volatile static uint16_t AWD_Val;
  AWD_Val = ADC_VOLTAGE(Analog5.Read());
}
