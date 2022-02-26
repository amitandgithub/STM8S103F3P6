#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include "system.h"
#include "GpioIn.h"

using namespace HAL;

namespace Devices
{
  
  template< typename Callback, 
  Gpio::Port_t CWPort, Gpio::Pin_t CWPin,
  Gpio::Port_t CCWPort, Gpio::Pin_t CCWPin,
  Gpio::Port_t BtnPort, Gpio::Pin_t BtnPin >
    
    class RotaryEncoder
    {      
    public:
      typedef enum 
      {
        EV_IDLE = (uint8_t)0x00,
        EV_CW   = (uint8_t)0x01,
        EV_CCW  = (uint8_t)0x02,
        EV_BTN  = (uint8_t)0x04
      }Event_t;
      
      private:
      static  uint8_t Event;
      
      /* Handler to be executed in ISR, when 
      *  the button is pressed
      */
      struct Btn_Hanler
      {
        static void Handler(void)
        {
          Event |= EV_BTN;
        }
      };
      
      /* Handler to be executed in ISR, when 
      *  the Encoder rotates
      */
      struct CW_Hanler
      {
        static void Handler(void)
        {
          if(CCW)
          {
            Event |= EV_CCW;
          }
          else
          {
            Event |= EV_CW;
          }        
        }
      };
      
    public:
      static GpioIn<CWPort,CWPin,Gpio::FL_INT,Gpio::FALLING,typename RotaryEncoder::CW_Hanler>  CW;
      static GpioIn<CCWPort,CCWPin> CCW;
      static GpioIn<BtnPort,BtnPin,Gpio::FL_INT,Gpio::FALLING,typename RotaryEncoder::Btn_Hanler> Btn;

      static void HwInit(void)
      {
        CW.HwInit();
        CCW.HwInit();
        Btn.HwInit();
      }
      
      static void Run(void)
      {
        if(Event != EV_IDLE)
        {          
          if(Event & EV_CW)
          {
            Callback::Fx(EV_CW);
          }
          else  if(Event & EV_CCW)
          {
            Callback::Fx(EV_CCW);
          }
          else if(Event & EV_BTN)
          {
            Callback::Fx(EV_BTN);
          }
          
          Event = EV_IDLE;
        }
      }
      
    };
  
  
  template< typename Callback,Gpio::Port_t CWPort, Gpio::Pin_t CWPin,
  Gpio::Port_t CCWPort, Gpio::Pin_t CCWPin,
  Gpio::Port_t BtnPort, Gpio::Pin_t BtnPin>
    uint8_t RotaryEncoder<Callback,CWPort,CWPin,CCWPort,CCWPin,BtnPort,BtnPin>::Event = EV_IDLE;
  
  template< typename Callback,Gpio::Port_t CWPort, Gpio::Pin_t CWPin,
  Gpio::Port_t CCWPort, Gpio::Pin_t CCWPin,
  Gpio::Port_t BtnPort, Gpio::Pin_t BtnPin>
    GpioIn<CWPort,CWPin,Gpio::FL_INT,Gpio::FALLING,typename RotaryEncoder<Callback,CWPort,CWPin,CCWPort,CCWPin,BtnPort,BtnPin>::CW_Hanler> 
      RotaryEncoder<Callback,CWPort,CWPin,CCWPort,CCWPin,BtnPort,BtnPin>::CW;
  
  template< typename Callback,Gpio::Port_t CWPort, Gpio::Pin_t CWPin,
  Gpio::Port_t CCWPort, Gpio::Pin_t CCWPin,
  Gpio::Port_t BtnPort, Gpio::Pin_t BtnPin>
    GpioIn<CCWPort,CCWPin> 
      RotaryEncoder<Callback,CWPort,CWPin,CCWPort,CCWPin,BtnPort,BtnPin>::CCW;
  
  template< typename Callback,Gpio::Port_t CWPort, Gpio::Pin_t CWPin,
  Gpio::Port_t CCWPort, Gpio::Pin_t CCWPin,
  Gpio::Port_t BtnPort, Gpio::Pin_t BtnPin>
    GpioIn<BtnPort,BtnPin,Gpio::FL_INT,Gpio::FALLING,typename RotaryEncoder<Callback,CWPort,CWPin,CCWPort,CCWPin,BtnPort,BtnPin>::Btn_Hanler> 
      RotaryEncoder<Callback,CWPort,CWPin,CCWPort,CCWPin,BtnPort,BtnPin>::Btn;
}

#endif // ROTARY_ENCODER_H