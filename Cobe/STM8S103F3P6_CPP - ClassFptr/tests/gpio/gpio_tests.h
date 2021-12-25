

#include"system.h"
#include"DigitalOut.h"
#include"DigitalIn.h"

#define LED B5
#define BUTTON D4

class myClass
{
public:
  void Fx(void)
  {
    volatile int i,a = 20;
    i = 20*a;    
  }
};

typedef  void (myClass::*Fx_t)(void);

extern class myClass AmitClass;
void gpio_tests();

void gpio_poll_test();

void gpio_intr_test();

void Gpio_1_Callback();

void Gpio_Callback();

void Gpio1_Callback();

extern DigitalIn<myClass,D4,INPUT_FLOATING_INTERRUPT,INTERRUPT_ON_RISING> Button;
//extern DigitalIn<GpioFptr_t,D5,INPUT_FLOATING_INTERRUPT,INTERRUPT_ON_RISING> Button1;