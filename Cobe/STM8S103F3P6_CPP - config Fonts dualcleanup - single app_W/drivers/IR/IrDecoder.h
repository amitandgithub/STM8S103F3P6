
/******************
** CLASS: IrDecoder
**
** DESCRIPTION:
**  Implements the IrDecoder driver
**
** CREATED: 7 Feb 2021, by Amit Chaudhary
**
** FILE: IrDecoder.h
**
******************/

#ifndef IrDecoder_h
#define IrDecoder_h


template<typename PulseIn_t, PulseIn_t& PulseIn, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE>
class IrDecoder
{
  typedef typename PulseIn_t::queue_t::DataSize_t data_t;
  
  static const uint16_t ONE_MARK_TOLERANCE = 300;
  static const uint8_t HDR_MARK_POS = 1;
  
  uint8_t data_available;
  uint8_t Mask;
  uint32_t Result;
  
public:       
  
  typedef enum
  {
    IDLE,
    REPEATED_CODE,
    HEADER_FOUND
  }State_t;
  
  State_t State;
  
  void HwInit() { PulseIn.HwInit(); }
  
  State_t GetState() const { return State; }
  
  void Stop() { PulseIn.Stop(); }
  
  uint8_t Available(void) const{ return data_available; }
  
  uint32_t Read(void) { data_available = 0; return Result; }
  
  bool RepetedCode(data_t data) const   {return (data > RPT_SPACE - ONE_MARK_TOLERANCE) && (data < RPT_SPACE + ONE_MARK_TOLERANCE); }
  
  bool Header(data_t data) const        {return (data > HDR_SPACE - ONE_MARK_TOLERANCE) &&  (data < HDR_SPACE + ONE_MARK_TOLERANCE); }
  
  bool BitOne(data_t data) const        {return (data > ONE_SPACE - ONE_MARK_TOLERANCE) && (data < ONE_SPACE + ONE_MARK_TOLERANCE); }
  
  void Start()
  {
    PulseIn.Start();
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
    
    if(PulseIn.Q.Read(&data) == PulseIn_t::queue_t::QUEUE_OK)
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
    PulseIn.GetPulseWidth();    
  }
  
};
#endif //IrDecoder_h