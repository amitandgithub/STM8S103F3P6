


#include"gpio_tests.h"

GpioOut<LED> Led;
GpioOut<LED2> Led2;
Button_t Button;
Button2_t Button2;
static uint16_t Lows;

void gpio_tests()
{
   //gpio_poll_test();
   RotaryEncoder_test();
  //delay_ms(300);
  
}

void gpio_poll_test()
{
  Led.HwInit();
  Led2.HwInit();
  Button.HwInit();
  Button2.HwInit();
  
  while(1)
  {
    Led = Button;
    Led2 = Button2;

//    Led.Toggle();
//    Led2.Toggle();
//    delay_ms(300);
  }
}

void Gpio_Callback(void)
{
  Lows++;
}

RotaryEncoder_t RotaryEncoder_Obj;
typedef typename RotaryEncoder_t::Event_t RotEvent_t;
RotEvent_t RotEvent;

void RotaryEncoder_test(void)
{
  RotaryEncoder_Obj.HwInit();
  
  while(1)
  {
    RotaryEncoder_Obj.Run();
    
  } 
}