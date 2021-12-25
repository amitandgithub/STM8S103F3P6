

#include"system.h"
#include"Gpio.h"
#include"GpioOut.h"
#include"GpioIn.h"
#include"RotaryEncoder.h"

using namespace HAL;
using namespace Devices;

#define LED B5
#define LED2 C4
#define BUTTON D3

void gpio_tests();

void gpio_poll_test();

void gpio_intr_test();

void RotaryEncoder_test(void);

void Gpio_1_Callback();

void Gpio_Callback(void);

typedef void (*GpioFptr_t)(void);

template<typename ISR_t, ISR_t& ISR >
void Interrupt(void)
{
  ISR.IRQ_Handler();
}


template<typename T>
void VendorJob(T fp)
{
  fp();//callback
}

volatile static uint8_t byte;
struct Keypad
{
  static void Handler(void)
  {    
    byte++;
  }
};


volatile static uint8_t Handler2;
struct RotaryEncoder2
{
  static void Handler(void)
  {
    Handler2++;
  }
};

typedef GpioIn<D6,Gpio::FL_INT,Gpio::RISING,Keypad> Button2_t;
typedef GpioIn<BUTTON,Gpio::FL_INT,Gpio::RISING> Button_t;

extern Button_t Button;
extern Button2_t Button2;

template<typename ISR_t , ISR_t* Isr, uint8_t ISR_no>
void Interrupt_Fx(uint8_t)
{
  ISR_t* pISR = Isr;
  pISR->IRQ_Handler();
}

struct RotApp; // Forward decl
typedef RotaryEncoder<RotApp,D4,D5,D6> RotaryEncoder_t;
extern RotaryEncoder_t RotaryEncoder_Obj;

volatile static uint8_t CW,CCW,Btn;
struct RotApp
{  
  static void Fx(uint8_t rotEv)
  {   
    if(rotEv == RotaryEncoder_t::EV_CW)
    {
      CW++;
    }
    else  if(rotEv == RotaryEncoder_t::EV_CCW)
    {
      CCW++;
    }
    else  if(rotEv == RotaryEncoder_t::EV_BTN)
    {
      Btn++;
    }
  }
};






//struct ButtonMonitor
//{
//  void Handler(void)
//  {
//    volatile static int Handler;
//    Handler++;
//  }
//};
//
//ButtonMonitor ButtonMonitor_Obj;
//
//void (ButtonMonitor::*handler)(void);
//template<>
//void Interrupt_Fx<HAL::Gpio,15>(void)
//{
//  HAL::Gpio::IRQ_Handler();
//}
//
//template<>
//void Interrupt_Fx<HAL::Gpio,16>(void)
//{
//  HAL::Gpio::IRQ_Handler();
//}