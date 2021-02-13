
/******************
** CLASS: IrDecoder1
**
** DESCRIPTION:
**  Implements the IrDecoder driver
**
** CREATED: 7 Feb 2021, by Amit Chaudhary
**
** FILE: IrDecoder1.h
**
******************/

#ifndef IrDecoder1_h
#define IrDecoder1_h

#include"TvCodes.h"
#include"pulseIn1.h"

template<typename PulseIn_t, PulseIn_t* pPulseIn , uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE>
class IrDecoder1 //: public pulseIn1<Channel,15,0xffff,TimerPol_Rising,34,0> 
{
  //typedef pulseIn1<Channel,15,0xffff,TimerPol_Rising,34,0> PulseIn;
 
  
  static const uint16_t ONE_MARK_TOLERANCE = 300;
  static const uint8_t HDR_MARK_POS = 1;
  
  uint8_t data_available;
  uint8_t Mask;
  uint32_t Result;
  
public:       
  typedef typename PulseIn_t::queue_t::DataSize_t data_t;
  
  typedef enum
  {
    IDLE,
    REPEATED_CODE,
    HEADER_FOUND
  }State_t;
  
  State_t State;
  
  void HwInit() { pPulseIn->HwInit(); }
  
  State_t GetState() const { return State; }
  
  void Stop() { pPulseIn->Stop(); }
  
  uint8_t Available(void) const{ return data_available; }
  
  uint32_t Read(void) { data_available = 0; return Result; }
  
  bool RepetedCode(data_t data) const   {return (data > RPT_SPACE - ONE_MARK_TOLERANCE) && (data < RPT_SPACE + ONE_MARK_TOLERANCE); }
  
  bool Header(data_t data) const        {return (data > HDR_SPACE - ONE_MARK_TOLERANCE) &&  (data < HDR_SPACE + ONE_MARK_TOLERANCE); }
  
  bool BitOne(data_t data) const        {return (data > ONE_SPACE - ONE_MARK_TOLERANCE) && (data < ONE_SPACE + ONE_MARK_TOLERANCE); }
  
  void Start()
  {
    pPulseIn->Start();
    State = IDLE;
    Result = 0;
    data_available = 0;
  }
  
  /*
  IDLE 1. Wait for data available in receive Queue
  2. If data available, decode Header or Repeated code
  HEADER_FOUND
  1. Decode the data bits
  2. when all data bits decoded, Go to Idle state
  */
  
  void Run(void)
  {    
    data_t data;
    
    if(pPulseIn->Q.Read(&data) == PulseIn_t::queue_t::QUEUE_OK)
    {      
      if(State == IDLE)
      {       
        if(RepetedCode(data)) // Repeated frame found
        {  
          data_available = 1;
        }
        else if(Header(data)) // Header found
        {          
          Result = 0;
          Mask = IR_BITS-1U;
          State = HEADER_FOUND;
        }
      }
      else if(State == HEADER_FOUND) // if Header found, decode data bits
      {
        data_available = 0;
        
        if(BitOne(data))
        {
          Result |= ((uint32_t)1U<<Mask);  
        }            
        
        if(--Mask == 255)
        {
          State = IDLE;
          data_available = 1;
        }
      }
    }  
  }  
  
  /*
  if we don't define this IRQ_Handler function in this class then,
  the IRQ_Handler of the pulseIn will get called automatically with this class object.
  So, If we want to add some functionality to the IRQ_Handler of this calss then
  better to define one here and call the pulseIn in IRQ_Handler that with extra functionality here like below
  Finally, IRQ_Handler of this class = IRQ_Handler of pulseIn + extra Functionality of this class 
  */
  void IRQ_Handler(void)
  {
    pPulseIn->GetPulseWidth();    
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