

#include"system.h"
#include"DigitalOut.h"
#include"DigitalIn.h"

#define LED B5
#define BUTTON D4



void gpio_tests();

void gpio_poll_test();

void gpio_intr_test();

void Gpio_1_Callback();

void Gpio_Callback();

extern DigitalIn<D4,INPUT_FLOATING_INTERRUPT,INTERRUPT_ON_RISING,Gpio_Callback> Button;