/******************
** CLASS: optional_bytes
**
** DESCRIPTION:
**  optional_bytes functionality
**
** CREATED: 30 Jan 2021, by Amit Chaudhary
**
** FILE: optional_bytes.h
**
******************/
#ifndef optional_bytes_h
#define optional_bytes_h

#include"system.h"
#include"eeprom.h"


struct opt
{
  enum ARR
  {
    C5_TIM2_CH1__C6_TIM1_CH1__C7_TIM1_CH2 = 1U<<0U,
    A3_SPI_NSS__D2_TIM2_CH3 = 1U<<1U,
    AFR2_Resevered = 1U<<2U,
    C3_TLI = 1U<<3U,
    B4_ADC_ETR__B5_TIM1_BKIN = 1U<<4U,
    AFR5_Resevered = 1U<<5U,
    AFR6_Resevered = 1U<<6U,
    C3_TIM1_CH1N__C4_TIM1_CH2N = 1U<<7U
  };
  
  static uint8_t Read(uint16_t Reg)
  {
    return *(reinterpret_cast<uint8_t*>(Reg));
  }
  
  static bool Write(uint16_t Reg, uint8_t val)
  {    
    eeprom_Unlock();
    
    if(eeprom_IsReady(250))
    {      
      FLASH->CR2  = 0X80;
      FLASH->NCR2 = 0X7F;
      
      *(reinterpret_cast<uint8_t*>(Reg)) = val;
      *(reinterpret_cast<uint8_t*>(Reg+1)) = (uint8_t)~val;
      
      system_Reset();
      
      return true;
    }
    
    return false;
  }
  
  static bool Set(uint16_t Reg, uint8_t val)
  {
    uint8_t temp;
    temp =  Read(Reg);
    
    // If already set then don't set it again
    if(temp & val)
    {
      return true;
    }
    return Write(Reg, temp | val);
  }
  
  static bool Clear(uint16_t Reg, uint8_t val)
  {
    uint8_t temp;
    temp =  Read(Reg);
    
    // If already cleared then don't clear it again
    if(temp & val)
    {
      return Write(Reg, temp & (uint8_t)~val);
    }    
    return true;
  }
  
  static void EnableRemap(uint8_t Remap)
  {
    Set(0x4803,Remap);
  }
  
  static void DisableRemap(uint8_t Remap)
  {
    Clear(0x4803,Remap);
  }  
};

//template<uint16_t Reg>
//class AFR : public opt
//{
//public:
//  typedef opt opt;
//  typedef enum
//  {
//    C5_TIM2_CH1__C6_TIM1_CH1__C7_TIM1_CH2 = 1U<<0U,
//    A3_SPI_NSS__D2_TIM2_CH3 = 1U<<1U,
//    AFR2_Resevered = 1U<<2U,
//    C3_TLI = 1U<<3U,
//    B4_ADC_ETR__B5_TIM1_BKIN = 1U<<4U,
//    AFR5_Resevered = 1U<<5U,
//    AFR6_Resevered = 1U<<6U,
//    C3_TIM1_CH1N__C4_TIM1_CH2N = 1U<<7U
//  }Remap_t;
//  
//  void EnableRemap(Remap_t Remap)
//  {
//    this->Set(0x4803,Remap);
//  }
//  
//  void DisableRemap(Remap_t Remap)
//  {
//    this->Clear(0x4803,Remap);
//  }
//
//};


//class AFR1 : public opt
//{
//public:
//  typedef opt opt;
//  typedef enum
//  {
//    C5_TIM2_CH1__C6_TIM1_CH1__C7_TIM1_CH2 = 1U<<0U,
//    A3_SPI_NSS__D2_TIM2_CH3 = 1U<<1U,
//    AFR2_Resevered = 1U<<2U,
//    C3_TLI = 1U<<3U,
//    B4_ADC_ETR__B5_TIM1_BKIN = 1U<<4U,
//    AFR5_Resevered = 1U<<5U,
//    AFR6_Resevered = 1U<<6U,
//    C3_TIM1_CH1N__C4_TIM1_CH2N = 1U<<7U
//  }Remap_t;
//  
//  void EnableRemap(Remap_t Remap)
//  {
//    this->Set(0x4805,Remap);
//  }
//  
//  void DisableRemap(Remap_t Remap)
//  {
//    this->Clear(0x4805,Remap);
//  }
//
//};

//typedef struct
//{
//  uint8_t ROP;
//  uint8_t N_ROP;
//  uint8_t UBC;
//  uint8_t N_UBC;
//  uint8_t AFR;
//  uint8_t N_AFR;
//  uint8_t SYS;
//  uint8_t N_SYS;
//  uint8_t CLK;
//  uint8_t N_CLK;
//  uint8_t HSC_CLK;
//  uint8_t N_HSC_CLK;  
//}optional_bytes_mem_t;


#endif // optional_bytes_h