
#include"timers_test.h"

gpt1<15,0xffff,0> timer1;
gpt2<4,0xffff,0> timer2;
gpt4<0x04,255> timer4;
static uint32_t Results;

PulseIn_t       PulseIn;
PulseOut_t      PulseOut;
Ir_Encoder_t    Ir_Encoder;
IR_Decoder_t    Ir_Decoder;

void timer_tests(void)
{
 
  //gpt4_test();
  //gpt_test();
  //pulseIn_test();
  // IrDecoder_test();
  //pulseOut_test();
  //IR_Transmitter();
  //IR_Receiver();
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

void pulseIn_test(void)
{
  PulseIn.HwInit();
  PulseIn.Start();
  while(1)
  {
    //counter = PulseIn3.GetFrequency();
  }  
}

void IR_Transmitter(void)
{
  timer4.HwInit();  
  Ir_Encoder.HwInit();
  while(1)
  {
    Ir_Encoder.TxProtocol(SAMSUNG_32_TV_VOL_UP); // Samsung IR code for Vol up
    delay_ms(10);
    TestGPIO.Toggle();
  }  
}

void IR_Receiver(void)
{  
  Ir_Decoder.HwInit();
  Ir_Decoder.Start();
  while(1)
  {    
    Ir_Decoder.Run();
    if(Ir_Decoder.Available())
    {
      Results = Ir_Decoder.Read();
      
      if( (Results == SAMSUNG_32_TV_VOL_UP) )
      {
        TestLED.Toggle();
      }      
    }
  }
}

void pulseOut_test(void)
{  
  //uint8_t duty = 50;
  PulseOut.HwInit();
  PulseOut.Start();
  //PulseOut.SetFrequency(38000);
  while(1)
  {
   //PulseOut.SetDutyCycle(duty++);
   
   delay_ms(20);
    
  }  
}

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
     TestGPIO.High();
     WT_to_Samsung(Ir_Decoder.Read());
     TestGPIO.Low();
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

