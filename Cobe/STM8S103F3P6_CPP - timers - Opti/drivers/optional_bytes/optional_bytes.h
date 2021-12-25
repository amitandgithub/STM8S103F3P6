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

template<uint16_t Reg>
struct opt
{
  uint8_t Read(void)
  {
    return *(reinterpret_cast<uint8_t*>(Reg));
  }
  
  void Write(uint8_t val)
  {
    uint8_t temp = Read();
    
    FLASH->CR2  = 0X80;
    FLASH->NCR2 = 0X7F;
        
    temp |= val; 
    *(reinterpret_cast<uint8_t*>(Reg)) = val;
    *(reinterpret_cast<uint8_t*>(Reg+1)) = (uint8_t)~val;
  }
};


//template<uint16_t Prescalar, uint16_t Autoreload, uint8_t mode>
//class optional_bytes
//{
//
//
//};




#endif // optional_bytes_h