
#if 1
#include"system.h"
#include"gpt1.h"
#include"gpt2.h"
#include"gpt4.h"
#include"pulseIn1.h"
#include"pulseIn2.h"
#include"IrDecoder1.h" 
#include"IrDecoder2.h"
#include"pulseOut1.h"
#include"pulseOut2.h"
//#include"IrEncoder.h" 

//#define SAMSUNG_BITS          32
//#define SAMSUNG_HDR_MARK    4500
//#define SAMSUNG_HDR_SPACE   4500
//#define SAMSUNG_BIT_MARK     560
//#define SAMSUNG_ONE_SPACE   1600
//#define SAMSUNG_ZERO_SPACE   560
//#define SAMSUNG_RPT_SPACE   2250


void gpt_test(void);
void gpt4_test(void);
void pulseOut_test(void);
void pulseIn_test(void);
void IrDecoder_test(void);
void IR_Transmitter(void);
void IR_Extender(void);
void IR_Loopback(void);

gpt1<15,0xffff,0> timer1;
gpt2<4,0xffff,0> timer2;
gpt4<0x04,255> timer4;
static uint32_t Results;

volatile uint16_t counter;
void timer_tests(void)
{
 
  //gpt4_test();
  //gpt_test();
  //pulseIn_test();
   IrDecoder_test();
  //pulseOut_test();
  //IR_Transmitter();
  //IR_Extender();
  //IR_Loopback();

}

void gpt4_test(void)
{
  timer4.HwInit();
  timer4.Start();

  while(1)
  {    
    TestGPIO.High();
    timer4.Delay_US(1000);
    TestGPIO.Low();
  }  
}


void gpt_test(void)
{
  //timer1.HwInit();
  timer2.HwInit();
  
  //timer1.InterruptEnable(TIM1_IER_UIE);
  //timer2.InterruptEnable(TIM2_IER_UIE);
  
  //timer1.Start();
  timer2.Start();

  while(1)
  {
    TestGPIO.High();
    timer2.Delay_us(10);
    //timer2.Delay_ms(1000);
    TestGPIO.Low();
  }  
}


pulseIn1<TIM1_CH2_C7_RMP,15,0xffff> PulseIn1;
pulseIn2<TIM2_CH1_D4,4,0xffff> PulseIn2;
void pulseIn_test(void)
{
  PulseIn2.HwInit();
  PulseIn2.Start();
  while(1)
  {
    //counter = PulseIn3.GetFrequency();
  }  
}




//IrDecoder2<Timer2_Channel_2,
//32,
//1200,
//1200,
//200,
//600,
//200,
//800> Ir_Decoder_Fast;
IrDecoder2<TIM2_CH1_C5_RMP,
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
        TestLED.Toggle();
      }
      Ir_Decoder.Start();     
    }
  }  
}


pulseOut1<TIM1_CH2_C7_RMP ,1000> IrOut;
//pulseOut2<TIM2_CH3_D2_RMP ,1000> IrOut;
void pulseOut_test(void)
{  
  //uint8_t duty = 50;
  IrOut.HwInit();
  IrOut.Start();
  //IrOut.SetFrequency(38000);
  while(1)
  {
   //IrOut.SetDutyCycle(duty++);
   
   delay_ms(20);
    
  }  
}

#if 0
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

#endif



