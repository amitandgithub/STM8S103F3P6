


#include"gpio_tests.h"

DigitalOut<LED> Led;

DigitalIn<D4,INPUT_FLOATING_INTERRUPT,INTERRUPT_ON_RISING,Gpio_Callback> Button;
DigitalIn<D5,INPUT_FLOATING_INTERRUPT,INTERRUPT_ON_RISING,Gpio1_Callback> Button1;
static uint16_t Lows;
static uint16_t Lows1;

void gpio_tests()
{
    Led.HwInit();
    Button.HwInit();
    Button1.HwInit();
    
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
  
void Gpio1_Callback()
{
  Lows1++;
}