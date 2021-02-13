
#if 0
#include"system.h"
#include"gpt4.h"
#include"gpt.h" 
#include"pulseIn.h"
#include"IrDecoder.h" 
#include"pulseOut.h"
#include"IrEncoder.h" 

//#define SAMSUNG_BITS          32
//#define SAMSUNG_HDR_MARK    4500
//#define SAMSUNG_HDR_SPACE   4500
//#define SAMSUNG_BIT_MARK     560
//#define SAMSUNG_ONE_SPACE   1600
//#define SAMSUNG_ZERO_SPACE   560
//#define SAMSUNG_RPT_SPACE   2250

extern DigitalOut<C4> PC4;
extern DigitalOut<B1> PB1;
void gpt_test(void);
void gpt4_test(void);
void pulseOut_test(void);
void pulseIn_test(void);
void IrDecoder_test(void);
void IR_Transmitter(void);
void IR_Extender(void);
void IR_Loopback(void);

gpt4<uint32_t> timer4;
gpt<TIMER2,Tick_4us,249,0> timer2;
gpt<TIMER3,Tick_1us,0xffff,0> timer3;
static uint32_t Results;

volatile uint16_t counter;
void timer_tests(void)
{
 
  //gpt4_test();
  //gpt_test();
  //pulseIn_test();
  // IrDecoder_test();
 // pulseOut_test();
  IR_Transmitter();
  //IR_Extender();
  //IR_Loopback();

}

void gpt4_test(void)
{
  timer4.HwInit(0x04,255);
  //timer4.Start();

  while(1)
  {    
    PB1.High();
    timer4.Delay_US(1000);
    PB1.Low();
  }  
}


void gpt_test(void)
{
  //timer2.HwInit();
  timer3.HwInit();
  
  //timer2.InterruptEnable(TIM_IER_UIE);
  //timer3.InterruptEnable(TIM_IER_UIE);
  
  //timer2.Start();
  //timer3.Start();

  while(1)
  {
    PB1.High();
    timer3.Delay_us(4);
    //timer3.Delay_ms(650);
    PB1.Low();
  }  
}

#if 0
pulseIn<TIMER2,Timer_Channel_1,Tick_1us,0xffff> PulseIn3;
void pulseIn_test(void)
{
  PulseIn3.HwInit();
  PulseIn3.Start();
  while(1)
  {
    //counter = PulseIn3.GetFrequency();
  }  
}
#endif

IrDecoder<TIMER2,Timer_Channel_1,
SAMSUNG_BITS,
SAMSUNG_HDR_MARK,
SAMSUNG_HDR_SPACE,
SAMSUNG_BIT_MARK,
SAMSUNG_ONE_SPACE,
SAMSUNG_ZERO_SPACE,
SAMSUNG_RPT_SPACE> Ir_Decoder;

void IrDecoder_test(void)
{ 
  
  Ir_Decoder.HwInit();
  Ir_Decoder.Start();
  while(1)
  {
    Ir_Decoder.Run();
    
    if(Ir_Decoder.GetStatus())
    {
      Ir_Decoder.Decode(&Results);
      if( (Results == 0xE0E0E01f) || (Results == 0x14EBE01F) )
      {
        PC4.Toggle();
      }
      Ir_Decoder.Start();     
    }
  }  
}

#if 0
pulseOut<TIMER2,Timer_Channel_1,38000> IrOut;
void pulseOut_test(void)
{  
  uint8_t duty = 50;
  IrOut.HwInit();
  IrOut.Start();
  //IrOut.SetFrequency(38000);
  while(1)
  {
   IrOut.SetDutyCycle(duty++);
   
   delay_ms(20);
    
  }  
}
#endif

IrEncoder<TIMER3,Timer_Channel_2,
SAMSUNG_BITS,
SAMSUNG_HDR_MARK,
SAMSUNG_HDR_SPACE,
SAMSUNG_BIT_MARK,
SAMSUNG_ONE_SPACE,
SAMSUNG_ZERO_SPACE,
SAMSUNG_RPT_SPACE> Ir_Encoder;

IrEncoder<TIMER3,Timer_Channel_1,
32,
1200,
1200,
200,
600,
200,
800> Ir_Encoder_Fast;

void IR_Transmitter(void)
{
  timer4.HwInit(0x04,255);  
  Ir_Encoder.HwInit();
  while(1)
  {
    Ir_Encoder.TxProtocol(0xE0E0E01f); // Samsung IR code for Vol up
    delay_ms(100);
  }  
}

void IR_Extender(void)
{
  timer4.HwInit(0x04,255);  
  Ir_Encoder.HwInit();
  Ir_Decoder.HwInit();
  Ir_Decoder.Start();
  while(1)
  {
   Ir_Decoder.Run();
    
    if(Ir_Decoder.GetStatus())
    {
      Ir_Decoder.Decode(&Results);

      if( (Results == 0x14EBE01F) )
      {
        Ir_Encoder.TxProtocol(0xE0E0E01f);
        //delay_ms(10);
      }
      Ir_Decoder.Start();     
    }
  }  
}



IrDecoder<TIMER2,Timer_Channel_1,
32,
1200,
1200,
200,
600,
200,
800> Ir_Decoder_Fast;

void IR_Loopback(void)
{
  timer4.HwInit(0x04,255);  
  
  Ir_Decoder_Fast.HwInit();
  Ir_Decoder_Fast.Start();
  
  Ir_Encoder_Fast.HwInit();
  
  while(1)
  {
    Ir_Encoder_Fast.TxProtocol(0xE0E0E01f);
    
    Ir_Decoder_Fast.Run();
    
    if(Ir_Decoder_Fast.GetStatus())
    {
      Ir_Decoder_Fast.Decode(&Results);
    }
    
    Ir_Decoder_Fast.Start();
    
    delay_ms(5000);
  }  
}


#endif



