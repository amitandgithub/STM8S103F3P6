
/******************
** CLASS: adc
**
** DESCRIPTION:
**  Implements the adc driver
**
** CREATED: 13 Feb 2021, by Amit Chaudhary
**
** FILE: adc.h
**
******************/


#ifndef adc_h
#define adc_h

#include"system.h"
#include"DigitalOut.h"
#include"DigitalIn.h"
#include "stm8s_adc1.h"

typedef enum 
{
  ADC_CH_0,
  ADC_CH_1,
  ADC_CH_2,
  ADC_CH_3,
  ADC_CH_4,
  ADC_CH_5,
  ADC_CH_6,
  ADC_CH_7,
  ADC_CH_8,
  ADC_CH_9,
  ADC_CH_12,
  ADC_CH_ALL = (uint8_t)0xff
} ADC_Ch_t;

typedef enum 
{
  ADC_PSC_2  = (uint8_t)0x00, 
  ADC_PSC_3  = (uint8_t)0x10, 
  ADC_PSC_4  = (uint8_t)0x20,
  ADC_PSC_6  = (uint8_t)0x30, 
  ADC_PSC_8  = (uint8_t)0x40, 
  ADC_PSC_10 = (uint8_t)0x50,
  ADC_PSC_12 = (uint8_t)0x60, 
  ADC_PSC_18 = (uint8_t)0x70 
} ADC_PSC_t;

typedef enum { Ext_Trig_TIM = (uint8_t)0x00, Ext_Trig_GPIO = (uint8_t)0x10 } Ext_Trig_t;
typedef enum { ADC_Align_LEFT = (uint8_t)0x00, ADC_Align_RIGHT = (uint8_t)0x08 } ADC_Align_t;

typedef enum 
{ 
  ADC_Mode_Sngle = (uint8_t)0x00, // only one input channel must be selected
  ADC_Mode_Continous = (uint8_t)0x01,
  ADC_Mode_Single_Scan = (uint8_t)0x02,
  ADC_Mode_Continous_Scan = (uint8_t)0x03,
  ADC_Mode_ContinousBuffered = (uint8_t)0x04
} ADC_Mode_t;

typedef void (*Adc_Callback_t)(void);

//Voltage measured =  (ADC reading * System voltage) / Adc Max value
#define ADC_VOLTAGE(ADC_VAL) (uint16_t)((ADC_VAL) * 32 + 300) //(3300/ 1023) = 3.225 // 300 is for calibration

template<ADC_Mode_t Mode, ADC_Ch_t Ch>
class adc 
{
public:       
  
  typedef union
  {
    uint16_t Word;
    uint8_t Array[2];
  }Adc_Data_t;
  
  uint8_t HwInit(void)
  {  
    CLK->PCKENR2 |= (uint8_t) CLK_PCKENR2_ADC;    
    SetAlignment(ADC_Align_RIGHT);
    m_Adc_Callback = 0;
    if(Mode == ADC_Mode_Sngle)
    {
      Disable_Schmit_Trig(Ch);
      
      GpioHwInit(Ch);
    }
    else if(Mode == ADC_Mode_Single_Scan)
    {
      Disable_Schmit_Trig(ADC_CH_ALL);
      
      GpioHwInit(ADC_CH_2);
      GpioHwInit(ADC_CH_3);
      GpioHwInit(ADC_CH_4);
      GpioHwInit(ADC_CH_5);
      GpioHwInit(ADC_CH_6);

      Enable_ScanMode();
    }
    
    ADC1->CSR &= (uint8_t)(~ADC1_CSR_CH);
    ADC1->CSR |= (uint8_t)(Ch); 
      
    return 0;
  }  
  
  void GpioHwInit(ADC_Ch_t ch)
  {    
    if(ch == ADC_CH_2)
    {
       DigitalIn<C4,INPUT_ANALOG> PC4;
       PC4.HwInit();
    }
    else if(ch == ADC_CH_3)
    {
       DigitalIn<D2,INPUT_ANALOG> PD2;
       PD2.HwInit();
    }
    else if(ch == ADC_CH_4)
    {
       DigitalIn<D3,INPUT_ANALOG> PD3;
       PD3.HwInit();
    }
    else if(ch == ADC_CH_5)
    {
       DigitalIn<D5,INPUT_ANALOG> PD5;
       PD5.HwInit();
    }
    else if(ch == ADC_CH_6)
    {
       DigitalIn<D6,INPUT_ANALOG> PD6;
       PD6.HwInit();
    }
    else
    {
      // Error
    }
  }
  
  void StartConversion(void){ PowerOn(); }
  
  bool ConversionComplete(void){ return (ADC1->CSR & ADC1_CSR_EOC); }
  
  void ClearEOC(void) { ADC1->CSR &= (uint8_t)(~ADC1_CSR_EOC); }
  
  void ClearAWD(void) { ADC1->CSR &= (uint8_t)(~ADC1_CSR_AWD); }
  
  void Stop(void){ PowerOff(); }
  
  void PowerOn(void){ ADC1->CR1 |= ADC1_CR1_ADON; }
  
  void PowerOff(void){ ADC1->CR1 &= (uint8_t)(~ADC1_CR1_ADON);}
  
  void Enable_ScanMode() { ADC1->CR2 |= ADC1_CR2_SCAN; }
  
  void Disable_ScanMode(){ ADC1->CR2 &= (uint8_t)(~ADC1_CR2_SCAN); }
  
  void Enable_Interrupt(uint8_t Intr){ ADC1->CSR |= (uint8_t)Intr; }

  void Disable_Interrupt(uint8_t Intr) {  ADC1->CSR &= (uint8_t)(~(uint8_t)Intr); }
  
  void SetAlignment(ADC_Align_t Align) { ADC1->CR2 |= (uint8_t)(Align); }
  
  uint16_t Read(void)
  {
    Adc_Data_t Data;
    Data.Array[0] = ADC1->DRH;
    Data.Array[1] = ADC1->DRL;
    return Data.Word;
  }
  
  uint16_t Read(ADC_Ch_t ch)
  {
    return ((Adc_Data_t*)(ADC1_BaseAddress + 2 * (uint8_t)ch))->Word;
  }
    /*
      Adc Max value / System voltage = ADC reading/ Voltage measured
      Voltage measured =  (ADC reading * System voltage) / Adc Max value
    */
  uint16_t GetVoltage(uint16_t adc_reading)
  {
    //return adc_reading * (3300/ 1023)
     return (uint16_t) (adc_reading * 32 + 300); // (3300/ 1023) = 3.225 // 300 is for calibration
  }
  
  void SetMode(ADC_Mode_t mode)
  {
    if(mode == ADC_Mode_Continous)
    {
      ADC1->CR1 |= ADC1_CR1_CONT;
    }
    else
    {
      ADC1->CR1 &= (uint8_t)(~ADC1_CR1_CONT);
    }
  }
  
  void SetPrescalar(ADC_PSC_t prescalar )
  {
    ADC1->CR1 &= (uint8_t)(~ADC1_CR1_SPSEL);
    ADC1->CR1 |= (uint8_t)(prescalar);
  }

  void Enable_Ext_Trig(Ext_Trig_t ext_Trig )
  {
    ADC1->CR2 &= (uint8_t)(~ADC1_CR2_EXTSEL);
    ADC1->CR2 |= (uint8_t)(ext_Trig | ADC1_CR2_EXTTRIG);
  }
  
  void Disable_Ext_Trig(Ext_Trig_t ext_Trig )
  {
    ADC1->CR2 &= (uint8_t)(~ADC1_CR2_EXTSEL);
    ADC1->CR2 |= (uint8_t)(ext_Trig);
    ADC1->CR2 &= (uint8_t)(~ADC1_CR2_EXTTRIG);
  }
  
  void Enable_Schmit_Trig(ADC_Ch_t ADC_Ch)
  {
    if(ADC_Ch == ADC_CH_ALL)
    {
      ADC1->TDRL &= (uint8_t)0x0;
      ADC1->TDRH &= (uint8_t)0x0;
    }
    else if(ADC_Ch < ADC_CH_8)
    {
      ADC1->TDRL &= (uint8_t)(~(uint8_t)((uint8_t)0x01 << (uint8_t)ADC_Ch));
    }
    else
    {
      ADC1->TDRH &= (uint8_t)(~(uint8_t)((uint8_t)0x01 << (uint8_t)ADC_Ch));
    }
  }
  
  void Disable_Schmit_Trig(ADC_Ch_t ADC_Ch)
  {
    if(ADC_Ch == ADC_CH_ALL)
    {
      ADC1->TDRL |= (uint8_t)0xFF;
      ADC1->TDRH |= (uint8_t)0xFF;
    }
    else if(ADC_Ch < ADC_CH_8)
    {
      ADC1->TDRL |= (uint8_t)((uint8_t)0x01 << (uint8_t)ADC_Ch);
    }
    else
    {
      ADC1->TDRH |= (uint8_t)((uint8_t)0x01 << (uint8_t)ADC_Ch);
    }
  }
  
  void AnalogWatchDogConfig(ADC_Ch_t ADC_Ch, uint16_t Low, uint16_t High, Adc_Callback_t Adc_Callback)
  {
    if(Adc_Callback)
    {
      m_Adc_Callback = Adc_Callback;
      
      ADC1->LTRL = (uint8_t)Low;
      ADC1->LTRH = (uint8_t)(Low >> (uint8_t)2);
      
      ADC1->HTRL = (uint8_t)High;
      ADC1->HTRH = (uint8_t)(High >> (uint8_t)2);
      
      if(Mode != ADC_Mode_Sngle)
      {
        if(ADC_Ch < ADC_CH_8)
        {
          ADC1->AWCRL |= (uint8_t)((uint8_t)1 << ADC_Ch);
        }
        else
        {
          ADC1->AWCRH |= (uint8_t)((uint8_t)1 << (ADC_Ch - 8) );
        }    
      }
      
      Enable_Interrupt(ADC1_CSR_AWDIE);      
    }
  }
  
  void IRQ_Handler(void)
  {
    if(ADC1->CSR & ADC1_CSR_AWD)
    {      
      m_Adc_Callback();
      ClearAWD();
    }
  }

private: 
   Adc_Callback_t m_Adc_Callback;
};

#endif //adc_h
