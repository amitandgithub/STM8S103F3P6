


#include"gpio_tests.h"

DigitalOut<LED> Led;

DigitalIn<D4,INPUT_FLOATING_INTERRUPT,INTERRUPT_ON_RISING,Gpio_Callback> Button;
//DigitalIn<PORTB_FULL,PIN_0,INPUT_PULLUP_INTERRUPT,INTERRUPT_ON_FALLING,Gpio_Callback> PortButton;

static uint16_t Lows;

void gpio_tests()
{
    Led.HwInit();
    Button.HwInit();
    
  while(1)
  {
    gpio_poll_test();
    gpio_intr_test();
//    Led.Toggle();
//    delay_ms(300);
  }
  
}

void gpio_poll_test()
{
  if(Button.Read())
  {
    Led.High();  
  }
  else
  {
    Led.Low();
  } 
}


void gpio_intr_test()
{
  
 // Led.High();
  
 // Led.Low();
}

void Gpio_Callback()
{
  Lows++;
}