
/******************
** CLASS: IrEncoder
**
** DESCRIPTION:
**  Implements the IrEncoder driver
**
** CREATED: 4 Feb 2021, by Amit Chaudhary
**
** FILE: IrEncoder.h
**
******************/


#ifndef IrEncoder_h
#define IrEncoder_h

//extern HAL::gpt4<0x04,255> timer4;

template<typename pulseOut_t, pulseOut_t& pulseOut, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE>
class IrEncoder 
{
public:       
  
  void HwInit(){ pulseOut.HwInit(); }  
  
  // Start for "delay" time
  void Start(uint16_t delay)
  {
    pulseOut.Start();
    timer4.Delay_US(delay);
  }
  
  // Stop for "delay" time
  void Stop(uint16_t delay)
  {
    pulseOut.Stop();
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
    pulseOut.Stop();
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
