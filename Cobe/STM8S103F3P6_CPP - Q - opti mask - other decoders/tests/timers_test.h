
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
#include"IrEncoder1.h" 
#include"IrEncoder2.h" 
#include"TvCodes.h"


void gpt_test(void);
void gpt4_test(void);
void pulseOut_test(void);
void pulseIn_test(void);
void IrDecoder_test(void);
void IR_Transmitter(void);
void IR_Extender(void);
void IR_Loopback(void);
void WT_to_DDC(uint32_t code);
void WT_to_Samsung(uint32_t code);

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
  // IrDecoder_test();
  //pulseOut_test();
  //IR_Transmitter();
  IR_Extender();
  //IR_Loopback();

}

void gpt4_test(void)
{
  timer4.HwInit();
  //timer4.Start();

  while(1)
  {    
    TestGPIO.High();
    timer4.Delay_US(10);
    //timer4.Delay_us(10);
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
IrDecoder1<TIM1_CH1_C6_RMP,
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
    if(Ir_Decoder.Available())
    {
      Results = Ir_Decoder.Read();
      
      if( (Results == 0xE0E0E01f) || (Results == 0x14EBE01F) )
      {
        TestLED.Toggle();
      }      
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


IrEncoder2<TIM2_CH3_A3,
SAMSUNG_BITS,
SAMSUNG_HDR_MARK,
SAMSUNG_HDR_SPACE,
SAMSUNG_BIT_MARK,
SAMSUNG_ONE_SPACE,
SAMSUNG_ZERO_SPACE,
SAMSUNG_RPT_SPACE> Ir_Encoder;

IrEncoder2<TIM2_CH1_D4,
32,
1200,
1200,
200,
600,
200,
800> Ir_Encoder_Fast;

void IR_Transmitter(void)
{
  timer4.HwInit();  
  Ir_Encoder.HwInit();
  while(1)
  {
    Ir_Encoder.TxProtocol(DDC_MUTE); // Samsung IR code for Vol up
    delay_ms(100);
  }  
}

typedef
IrDecoder1<TIM1_CH1_C6_RMP,
SAMSUNG_BITS,
SAMSUNG_HDR_MARK,
SAMSUNG_HDR_SPACE,
SAMSUNG_BIT_MARK,
SAMSUNG_ONE_SPACE,
SAMSUNG_ZERO_SPACE,
SAMSUNG_RPT_SPACE> IR_Decoder_t;

IR_Decoder_t::State_t State;

void IR_Extender(void)
{
  timer4.HwInit();  
  Ir_Encoder.HwInit();
  Ir_Decoder.HwInit();
  Ir_Decoder.Start();
  while(1)
  {
   Ir_Decoder.Run();
   if(Ir_Decoder.Available())
   {
     WT_to_Samsung(Ir_Decoder.Read());
   }
  }  
}

void WT_to_DDC(uint32_t code)
{
  if(code == WT_CH_UP)
  {
    Ir_Encoder.TxProtocol(DDC_CH_UP);
  }
  else if(code == WT_CH_DOWN)
  {
    Ir_Encoder.TxProtocol(DDC_CH_DOWN);
  }
  else if( (code == WT_VOL_UP) || (code == WT_NAV_RIGHT) )
  {
    Ir_Encoder.TxProtocol(DDC_VOL_UP);
  }
  else if( (code == WT_VOL_DOWN) || (code == WT_NAV_LEFT) )
  {
    Ir_Encoder.TxProtocol(DDC_VOL_DOWN);
  }
  else if(code == WT_MUTE)
  {
    Ir_Encoder.TxProtocol(DDC_MUTE);
  }
  else if(code == WT_POWER)
  {
    Ir_Encoder.TxProtocol(DDC_POWER);
  }  
}

void WT_to_Samsung(uint32_t code)
{
  if(code == WT_CH_UP)
  {
    Ir_Encoder.TxProtocol(SAMSUNG_32_TV_CH_UP);
  }
  else if(code == WT_CH_DOWN)
  {
    Ir_Encoder.TxProtocol(SAMSUNG_32_TV_CH_DOWN);
  }
  else if( (code == WT_VOL_UP) || (code == WT_NAV_RIGHT) )
  {
    Ir_Encoder.TxProtocol(SAMSUNG_32_TV_VOL_UP);
  }
  else if( (code == WT_VOL_DOWN) || (code == WT_NAV_LEFT) )
  {
    Ir_Encoder.TxProtocol(SAMSUNG_32_TV_VOL_DOWN);
  }
  else if(code == WT_MUTE)
  {
    Ir_Encoder.TxProtocol(SAMSUNG_32_TV_MUTE);
  }
  else if(code == WT_POWER)
  {
    Ir_Encoder.TxProtocol(SAMSUNG_32_TV_POWER);
  }  
}

#if 0

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



