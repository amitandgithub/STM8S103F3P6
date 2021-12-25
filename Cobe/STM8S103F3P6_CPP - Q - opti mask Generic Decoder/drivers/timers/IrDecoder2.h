
/******************
** CLASS: IrDecoder2
**
** DESCRIPTION:
**  Implements the IrDecoder driver
**
** CREATED: 25 Jan 2021, by Amit Chaudhary
**
** FILE: IrDecoder2.h
**
******************/


#ifndef IrDecoder2_h
#define IrDecoder2_h

#include"TvCodes.h"

#include"pulseIn2.h"

template<Timer2_Channel_t Channel, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE>
class IrDecoder2 : public pulseIn2<Channel,4,0xffff,TimerPol_Rising,34,0> 
{
  
  typedef enum
  {
    IDLE,
    REPEATED_CODE,
    HEADER_FOUND
  }State_t;  
  
  typedef pulseIn2<Channel,4,0xffff,TimerPol_Rising,34,0> PulseIn;
  static const uint16_t ONE_MARK_TOLERANCE = 300;
  static const uint8_t HDR_MARK_POS = 1;
  State_t State;
  uint32_t Result;
  uint32_t Mask;
public:       
  
  
  
  void HwInit()
  {
    PulseIn::HwInit(); 
  }
  
  uint8_t GetStatus()
  {
      return PulseIn::m_Status;
  }
  
  void Start()
  {
    PulseIn::Start();
    State = IDLE;
  }
  
  void Stop()
  {
    PulseIn::Stop();
  }
    
  uint32_t Read(void)
  {
    return Result;
  }
  
  void Run(void)
  {    
    uint32_t data;
    
    // 1. if IR frame is available
    // 2. seek for Header
    // 3. decode data from header
    //if(PulseIn::Q.Available() >= PulseIn::SampleSize)
    
    if(PulseIn::Q.Available())
    {
      PulseIn::Q.Read(&data);
      
      if(State == IDLE)
      {       
        if( (data > RPT_SPACE - ONE_MARK_TOLERANCE) &&  (data < RPT_SPACE + ONE_MARK_TOLERANCE) ) // Repeated frame found
        {
          //PulseIn::Stop();
          //PulseIn::SampleIndex = 0;
          State = REPEATED_CODE;       
        }
        else if( (data > HDR_MARK - ONE_MARK_TOLERANCE) &&  (data < HDR_MARK + ONE_MARK_TOLERANCE) ) // Header found
        {          
          Result = 0;
          Mask = (1UL << (IR_BITS-1));
          State = HEADER_FOUND;
        }
      }
      else if(State == HEADER_FOUND)
      {
        //uint32_t Mask = ( 1UL << (IR_BITS-1) );
        
        if( (data > ONE_SPACE - ONE_MARK_TOLERANCE) && (data < ONE_SPACE + ONE_MARK_TOLERANCE) )
        {
          Result |= Mask;  
        }            
        Mask = Mask >> 1;
        
        if(Mask == 0)
        {
          //Mask = ( 1UL << (IR_BITS-1) );
          State = IDLE;
        }
      }
    }    
  }
  
  void Run1(void)
  {    
    if( PulseIn::SampleIndex >= 2)
    {
      if(((PulseIn::Samples[HDR_MARK_POS] > RPT_SPACE - ONE_MARK_TOLERANCE) && (PulseIn::Samples[HDR_MARK_POS] < RPT_SPACE + ONE_MARK_TOLERANCE)) )
      {
        PulseIn::Stop();
        //PulseIn::SampleIndex = 0;
        PulseIn::m_Status = 2;
      }
      else
      {
        delay_ms(100);
        if( !((PulseIn::Samples[HDR_MARK_POS] > HDR_MARK - ONE_MARK_TOLERANCE) && (PulseIn::Samples[HDR_MARK_POS] < HDR_MARK + ONE_MARK_TOLERANCE)) ) 
        {            
          PulseIn::SampleIndex = 0;
        }
      }             
    }    
  }
  
  // This function Looks for Packet Timeout, Repeated code handling etc.
  
  /*
  if we don't define this IRQ_Handler function in this class then,
  the IRQ_Handler of the pulseIn will get called automatically with this class object.
  So, If we want to add some functionality to the IRQ_Handler of this calss then
  better to define one here and call the pulseIn in IRQ_Handler that with extra functionality here like below
  Finally, IRQ_Handler of this class = IRQ_Handler of pulseIn + extra Functionality of this class 
  */
  void IRQ_Handler(void)
  {
    PulseIn::GetPulseWidth2();    
  } 
  
  uint32_t Decode(void)
  {
    uint32_t Result = 0;
    uint32_t Mask = ( 1UL << (IR_BITS-1) );
    uint8_t i;

     if( (PulseIn::Samples[HDR_MARK_POS] > HDR_MARK - ONE_MARK_TOLERANCE) && (PulseIn::Samples[HDR_MARK_POS] < HDR_MARK + ONE_MARK_TOLERANCE) ) 
      {          
        // Decode Bits
        for(i = 2; i < IR_BITS+2; i++)  
        {            
          if( (PulseIn::Samples[i] > ONE_SPACE - ONE_MARK_TOLERANCE) && (PulseIn::Samples[i] < ONE_SPACE + ONE_MARK_TOLERANCE) )
            {
              Result |= Mask;  
            }
            
            Mask = Mask >> 1;
        }          
      }     
     return Result;
  }
  
};
#endif //IrDecoder_h



#if 0
template< typename T, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
bool IrDecoder<T,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
Decode(uint32_t* pResult)
{
  uint32_t Result = 0;
  uint32_t Mask = ( 1UL << (IR_BITS-1) );
  uint8_t i;
  
  if(m_Status == 1)  // IR packet received
  {
    if( IN_RANGE(m_Buf[HDR_MARK_POS],HDR_MARK,IR_TOERANCE) && IN_RANGE(m_Buf[HDR_SPACE_POS],HDR_SPACE,IR_TOERANCE) ) // Header atched
    {
      // Decode Bits
      for(i = 3; i < IR_BITS*2+2 ; i += 2)  
      {
        if( IN_RANGE(m_Buf[i],BIT_MARK,IR_TOERANCE) )
        {
          
          if(IN_RANGE(m_Buf[i+1],ONE_SPACE,IR_TOERANCE))
          {
            Result |= Mask;  
          }
        }
        else
        {
          //Sync( ONE_SPACE*(IR_BITS - i) );
          //Sync(ONE_SPACE*IR_BITS);
          // Error in packed, reset to receive new packet
          m_Status = 0;
          *pResult = Result;
          m_PulseBuf.Idx = 0;              
          Start();              
          return false;
        }
        Mask = Mask >> 1;
      }
      // Packet decoded succesfully, get set for new packet
      m_Status = 0;
      *pResult = Result;
      m_PulseBuf.Idx = 0;
      Start();
      return true;
    }
    else
    {
      // Sync(ONE_SPACE*IR_BITS);
      // Header is corrupted discard the packed, reset to receive new packet
      m_Status = 0;
      *pResult = Result;
      m_PulseBuf.Idx = 0;
      Start();
      return false;
    }        
  }
  return false;      
}    

template< typename T, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
void IrDecoder<T,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
CallbackFunction()
{
  m_Status = 1;      
}

template< typename T, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
bool IrDecoder<T,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
DecodeRaw(RawIrFrmae_t* pRawIrFrmae)
{
  if( (pRawIrFrmae == nullptr) && (m_Status == 0) ) return false;
  
  pRawIrFrmae->HeaderMark   = m_Buf[HDR_MARK_POS]; 
  pRawIrFrmae->HeaderSpace  = m_Buf[HDR_SPACE_POS]; 
  pRawIrFrmae->BitMark      = m_Buf[2]; 
  
  for(uint8_t i = 3; i < m_PulseBuf.Idx; i += 2)  
  {
    //if()
  }
  return true;      
}



// Another way if we want to share the Timer with other modules. in this case we need not to create the timer object inside IrDecoder class
// instead we can pass the Timer type and address to be used inside IrDecoder module, but it generates 86 more bytes of code 
//#if 0

template<typename T, T* Addreess, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE>
class IrDecoder : public Callback 
{
public:       
  
  IrDecoder();
  
  ~IrDecoder(){};
  
  void HwInit();
  
  bool Decode(uint32_t* pResult);
  
  bool DecodeRaw(RawIrFrmae_t* pRawIrFrmae);
  
  bool Start(){ return PulseTimer->Start(&m_PulseBuf);}
  
  void Stop(){PulseTimer->Stop();}
  
  virtual void CallbackFunction();
  
  void GetRawFrame(uint16_t* Buf, uint16_t* Len){ Buf = m_Buf; *Len = m_PulseBuf.Idx;  }
  
  void Sync(uint32_t delay){ HAL::usDelay(delay); }
  
private:
  uint8_t           m_Status;
  uint16_t          m_Buf[size];
  HAL::PulseBuf_t   m_PulseBuf;
  T*                PulseTimer;      
};

template< typename T, uint32_t Addreess, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
IrDecoder<T,Addreess,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
IrDecoder(): PulseTimer((T*)Addreess)
{
  m_PulseBuf.Buf = m_Buf;
  m_PulseBuf.Len = size;
  m_PulseBuf.Idx = 0;
  m_PulseBuf.pCallback = this;
  m_Status = 0;
}

template< typename T,  uint32_t Addreess,uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
void IrDecoder<T,Addreess,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
HwInit()
{
  PulseTimer->HwInit(__LL_TIM_CALC_PSC(72000000, 1000000),0xffff,true);      
}

template< typename T, uint32_t Addreess, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
bool IrDecoder<T,Addreess,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
Decode(uint32_t* pResult)
{
  uint32_t Result = 0;
  uint32_t Mask = ( 1UL << (IR_BITS-1) );
  uint8_t i;
  
  if(m_Status == 1)  // IR packet received
  {
    if( IN_RANGE(m_Buf[HDR_MARK_POS],HDR_MARK,IR_TOERANCE) && IN_RANGE(m_Buf[HDR_SPACE_POS],HDR_SPACE,IR_TOERANCE) ) // Header atched
    {
      // Decode Bits
      for(i = 3; i < IR_BITS*2+2 ; i += 2)  
      {
        if( IN_RANGE(m_Buf[i],BIT_MARK,IR_TOERANCE) )
        {
          
          if(IN_RANGE(m_Buf[i+1],ONE_SPACE,IR_TOERANCE))
          {
            Result |= Mask;  
          }
        }
        else
        {
          //Sync( ONE_SPACE*(IR_BITS - i) );
          //Sync(ONE_SPACE*IR_BITS);
          m_Status = 0;
          *pResult = Result;
          m_PulseBuf.Idx = 0;              
          Start();              
          return false;
        }
        Mask = Mask >> 1;
      }
      m_Status = 0;
      *pResult = Result;
      m_PulseBuf.Idx = 0;
      Start();
      return true;
    }
    else
    {
      // Sync(ONE_SPACE*IR_BITS);
      m_Status = 0;
      *pResult = Result;
      m_PulseBuf.Idx = 0;
      Start();
      return false;
    }        
  }
  return false;      
}    

template< typename T, uint32_t Addreess, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
void IrDecoder<T,Addreess,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
CallbackFunction()
{
  m_Status = 1;      
}

template< typename T, uint32_t Addreess, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
bool IrDecoder<T,Addreess,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
DecodeRaw(RawIrFrmae_t* pRawIrFrmae)
{
  if( (pRawIrFrmae == nullptr) && (m_Status == 0) ) return false;
  
  pRawIrFrmae->HeaderMark   = m_Buf[HDR_MARK_POS]; 
  pRawIrFrmae->HeaderSpace  = m_Buf[HDR_SPACE_POS]; 
  pRawIrFrmae->BitMark      = m_Buf[2]; 
  
  for(uint8_t i = 3; i < m_PulseBuf.Idx; i += 2)  
  {
    //if()
  }
  return true;      
}
#endif