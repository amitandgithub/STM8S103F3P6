
/*
* PowerMonitor.h
*
*  Created on: 4 Apr 2021
*      Author: Amit Chaudhary
*/

#ifndef POWER_MONITOR_H
#define POWER_MONITOR_H

#include "System.h"

using namespace HAL;
using namespace Devices;
using namespace Utils;

namespace App
{
  typedef enum
  {
    Font_11x18,
    Font_16x26,
  }font_t;

    template<typename CurrentSensor_t>
    class PowerMonitor
    {
      public:
//      enum
//      {
//        LINE_1 = 0,   
//        LINE_2 = 1, 
//        LINE_3 = 2, 
//        FONT_W = 11,
//        FONT_H = 18,
////        LINE_2 = 1*18 + 64/18  + 10,        
////        LINE_3 = 2*18 + 64/18  + 10,  
//        LINE_2_16_26 = 1*26 + 64/26  + 10,
//        //COLS = 128/FONT_W,
//        COLS_11_18 = 11,
//        COLS_16_26 = 8,
//      };
      
      typedef Devices::SSD1306< i2c_t ,0x78,true> ssd1306_t; 
      typedef GpioIn<D3> Button_t;
      
      static Button_t Button; 
      
      static char DispBuf[11];
      volatile static uint8_t RunLoop;   
      static uint8_t Mode; 
      static uint16_t Seconds;
      static float CurrentSamples;
      static uint8_t SampleCount; 
      
      typedef typename CurrentSensor_t::Power_t Power_t;
      static Power_t Power;
      
      static void Init(void)
      {
        ssd1306_t::HwInit();
        CurrentSensor_t::HwInit();
        Button.HwInit();
        UpdateMode();
      }
      
      static void UpdateMode(void)
      {
        ssd1306_t::Clear();
        
        if(Mode == 0)
        {
          ssd1306_t::SetFont(Devices::Font_11x18);
          ssd1306_t::Puts("00:00 26.30v",11,ssd1306_t::c1,ssd1306_t::l1);
          ssd1306_t::Puts("I ",2,ssd1306_t::c1,ssd1306_t::l2);
          ssd1306_t::Puts("C ",2,ssd1306_t::c1,ssd1306_t::l3); 
        }
        else if(Mode == 1)
        {
          ssd1306_t::SetFont(Devices::Font_16x26);
          ssd1306_t::Puts("V ",2,ssd1306_t::c1,ssd1306_t::L1);
          ssd1306_t::Puts("I ",2,ssd1306_t::c1,ssd1306_t::L2);
        }
      }
      static void UpdateClock(void)
      {   
          char TimeStr[10];          
          
          // If there is a button press then change the screen mode
          if(Button)
          {
            Mode++;
            
            if(Mode >= 2) Mode = 0;
            
            UpdateMode();
          }
          
          Seconds++;       
          
          if(! (Seconds % 60) )
          {
            CountertoTime(Seconds,TimeStr);
            
            if(Mode == 0)
            {
              ssd1306_t::Puts(TimeStr,5,ssd1306_t::c1,ssd1306_t::l1);
            }
          }
      }      
      
      static void CaptureSamples(void)
      {          
          RunLoop = 1;          
          CurrentSamples += CurrentSensor_t::GetCurrent_mA();
          SampleCount++;
          
          if(SampleCount >= 8)
          {           
            Power.mAH   += CurrentSamples/28800.0; // 8*3600 = 28800
            SampleCount = 0;
            CurrentSamples = 0;
          } 
      }
      
      static void Run()
      {        
        // Run every 125 ms
          if(RunLoop) RunLoop = 0;
          else return;
          
          TestGPIO.High();
          Power.Voltage = CurrentSensor_t::GetBusVoltage_V(); // 26.0 V
          Power.Current = CurrentSensor_t::GetCurrent_mA();   // 3200.0 mA
          
          if(Power.Current < 0) Power.Current = 0;
          
          if(Power.Voltage < 1) Power.Voltage = 0;
         
          if( (Power.Current >= 400.0) || (Power.Voltage >= 16.0) ) CurrentSensor_t::SetCalibration_32V_2A();
          else CurrentSensor_t::SetCalibration_16V_400mA();
          
          if(Mode == 0)
          {
            memset((uint8_t*)&DispBuf[6],' ',ssd1306_t::cols - 6);
            Utils::ftoa(Power.Voltage,&DispBuf[6],2,' ');
            ssd1306_t::Puts(&DispBuf[6],ssd1306_t::cols-6 ,ssd1306_t::c7,ssd1306_t::l1);
            
            memset((uint8_t*)DispBuf,' ',ssd1306_t::cols);
            Utils::ftoa(Power.Current,DispBuf,2,' ');                 
            ssd1306_t::Puts(DispBuf,ssd1306_t::cols-2,ssd1306_t::c3,ssd1306_t::l2);
            
            memset((uint8_t*)DispBuf,' ',ssd1306_t::cols);
            Utils::ftoa(Power.mAH,DispBuf,2,' ');                 
            ssd1306_t::Puts(DispBuf,ssd1306_t::cols-2,ssd1306_t::c3,ssd1306_t::l3);
          }
          else if(Mode == 1)
          {
            memset((uint8_t*)&DispBuf[0],' ',ssd1306_t::COLS);
            Utils::ftoa(Power.Voltage,&DispBuf[0],2,' ');
            ssd1306_t::Puts(&DispBuf[0],ssd1306_t::COLS-2 ,ssd1306_t::K3,ssd1306_t::L1);
            
            memset((uint8_t*)DispBuf,' ',ssd1306_t::COLS);
            Utils::ftoa(Power.Current,DispBuf,2,' ');                 
            ssd1306_t::Puts(DispBuf,ssd1306_t::COLS-2,ssd1306_t::K3,ssd1306_t::L2);
          }
          else
          {
            
          }
          
          TestGPIO.Low();
      }
    };
    template<typename CurrentSensor_t>
      char PowerMonitor<CurrentSensor_t>::DispBuf[11];
    
    template<typename CurrentSensor_t>
      volatile uint8_t PowerMonitor<CurrentSensor_t>::RunLoop = 1;
    
    template<typename CurrentSensor_t>
      PowerMonitor<CurrentSensor_t>::Power_t PowerMonitor<CurrentSensor_t>::Power;
    
    template<typename CurrentSensor_t>
      uint16_t PowerMonitor<CurrentSensor_t>::Seconds;
    
    template<typename CurrentSensor_t>
      float PowerMonitor<CurrentSensor_t>::CurrentSamples;
    
    template<typename CurrentSensor_t>
      uint8_t PowerMonitor<CurrentSensor_t>::SampleCount; 
    
    template<typename CurrentSensor_t>
      uint8_t PowerMonitor<CurrentSensor_t>::Mode = 1;
    
    template<typename CurrentSensor_t>
      PowerMonitor<CurrentSensor_t>::Button_t PowerMonitor<CurrentSensor_t>::Button;
    
} // namespace App

#endif //POWER_MONITOR_H