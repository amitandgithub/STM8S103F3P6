
/******************
** CLASS: IrEncoder1
**
** DESCRIPTION:
**  Implements the IrEncoder1 driver
**
** CREATED: 4 Feb 2021, by Amit Chaudhary
**
** FILE: IrEncoder1.h
**
******************/


#ifndef IrEncoder1_h
#define IrEncoder1_h


#include"pulseOut1.h"

//extern gpt<TIMER3,Tick_1us,0xffff,0> timer3;
extern gpt4<0x04,255> timer4;

template<Timer1_Channel_t Channel, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE>
class IrEncoder1 : public pulseOut1<Channel,38000> 
{
  typedef pulseOut1<Channel,38000> PulseOut;
public:       
  
  void HwInit(){ PulseOut::HwInit(); }  
  
  // Start for "delay" time
  void Start(uint16_t delay)
  {
    PulseOut::Start();
    timer4.Delay_US(delay);
  }
  
  // Stop for "delay" time
  void Stop(uint16_t delay)
  {
    PulseOut::Stop();
    timer4.Delay_US(delay);
  }
  
  // Send Header + 32 bit data + Footer over IR
  void TxProtocol(uint32_t data)
  {
    // Header    
    Start(HDR_MARK);    
    Stop(HDR_MARK);
    
    // Data
    TxByte(data>>24);
    TxByte(data>>16);
    TxByte(data>>8);
    TxByte(data);
    
    // Footer
    Start(BIT_MARK);
    PulseOut::Stop();
  }
  
  // Just send one byte over IR
  void TxByte(uint8_t data)
  {
    for (uint8_t  mask = 0x80;  mask;  mask >>= 1)
    {
      Start(BIT_MARK);
      
      if (data & mask)
      {        
        Stop(ONE_SPACE);  
      }
      else
      {        
        Stop(ZERO_SPACE);  
      }
    }
  }
  
  
};

#endif //IrDecoder_h
