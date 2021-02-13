
/******************
** CLASS: IrEncoder2
**
** DESCRIPTION:
**  Implements the IrEncoder2 driver
**
** CREATED: 5 Feb 2021, by Amit Chaudhary
**
** FILE: IrEncoder2.h
**
******************/


#ifndef IrEncoder2_h
#define IrEncoder2_h


#include"pulseOut2.h"

extern gpt4<0x04,255> timer4;

template<Timer2_Channel_t Channel, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE>
class IrEncoder2 : public pulseOut2<Channel,38000> 
{
  typedef pulseOut2<Channel,38000> PulseOut;
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
    Stop(HDR_SPACE);
    
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
