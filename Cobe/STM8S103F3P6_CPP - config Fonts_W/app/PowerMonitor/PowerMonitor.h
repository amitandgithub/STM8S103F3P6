
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
  
  template<typename CurrentSensor_t, font_t font>
    class PowerMonitor
    {
      public:
      enum
      {
        LINE_1 = 0,    
        FONT_W = 11,
        FONT_H = 18,
        LINE_2 = 1*FONT_H + 64/FONT_H  + 10,
        LINE_3 = 2*FONT_H + 64/FONT_H  + 10,       
        COLS = 128/FONT_W,
      };
      
      typedef Devices::SSD1306< i2c_t ,0x78,FONT_W,FONT_H,true> ssd1306_t;
       
      static char DispBuf[11];
      static uint8_t RunLoop;      
      typedef typename CurrentSensor_t::Power_t Power_t;
      static Power_t Power;
      
      static void Init(void)
      {
        ssd1306_t::HwInit();
        CurrentSensor_t::HwInit();
        
        if( font == Font_11x18  )
        {
          ssd1306_t::Puts("00:00 26.30v",11,0,LINE_1);
          ssd1306_t::Puts("I ",2,0,LINE_2);
          ssd1306_t::Puts("C ",2,0,LINE_3);
        }
        else if( font == Font_16x26 )
        {
          ssd1306_t::Puts("V ",2,0,LINE_1);
          ssd1306_t::Puts("I ",2,0,LINE_2);
        }
        else
        {
          
        }        
      }
      
      static void UpdateClock(void)
      {   
       if( font == Font_11x18  )
        {
          char TimeStr[10];
          static uint32_t Seconds;
          
          Seconds++;       
          
          if(! (Seconds % 60) )
          {
            CountertoTime(Seconds,TimeStr);
            
            ssd1306_t::Puts(TimeStr,5,0*FONT_W,LINE_1);
          }
        }
      }     
      
      static void CaptureSamples(void)
      {        
        if( font == Font_11x18  )
        {
          static float CurrentSamples;
          static uint8_t SampleCount; 
          
          CurrentSamples += CurrentSensor_t::GetCurrent_mA();
          SampleCount++;
          
          if(SampleCount >= 8)
          {           
            Power.mAH   += CurrentSamples/28800.0; // 8*3600 = 28800
            SampleCount = 0;
            CurrentSamples = 0;
          }  
        }
        else
        {
          RunLoop = 1;
        }
      }
      
      static void Run()
      {
        // Run every 125 ms
          if(RunLoop) 
          {
            RunLoop = 0;
          }
          else
          {            
            return;
          }
          
          TestGPIO.High();
          Power.Voltage = CurrentSensor_t::GetBusVoltage_V(); // 26.0 V
          Power.Current = CurrentSensor_t::GetCurrent_mA();   // 3200.0 mA
          
          if(Power.Current < 0) 
          {
            Power.Current = 0;
          }
          
          if(Power.Voltage < 1) 
          {
            Power.Voltage = 0;
          }
          
         if( font == Font_11x18  )
          {
            memset((uint8_t*)&DispBuf[6],' ',COLS-6);
            Utils::ftoa(Power.Voltage,&DispBuf[6],2,' ');
            ssd1306_t::Puts(&DispBuf[6],COLS-5 ,6*FONT_W,LINE_1);
            
            memset((uint8_t*)DispBuf,' ',COLS);
            Utils::ftoa(Power.Current,DispBuf,2,' ');                 
            ssd1306_t::Puts(DispBuf,COLS-2,2*FONT_W,LINE_2);
            
            memset((uint8_t*)DispBuf,' ',COLS);
            Utils::ftoa(Power.mAH,DispBuf,2,' ');                 
            ssd1306_t::Puts(DispBuf,COLS-2,2*FONT_W,LINE_3);
          }
          else if( font == Font_16x26  )
          {
            memset((uint8_t*)&DispBuf[0],' ',COLS);
            Utils::ftoa(Power.Voltage,&DispBuf[0],2,' ');
            ssd1306_t::Puts(&DispBuf[0],COLS-2 ,2*FONT_W,LINE_1);
            
            memset((uint8_t*)DispBuf,' ',COLS);
            Utils::ftoa(Power.Current,DispBuf,2,' ');                 
            ssd1306_t::Puts(DispBuf,COLS-2,2*FONT_W,LINE_2);
          }
          else
          {
            
          }
          
          TestGPIO.Low();
      }
    };
  
  template<typename CurrentSensor_t, font_t font>
    char PowerMonitor<CurrentSensor_t,font>::DispBuf[11];
  
  template<typename CurrentSensor_t, font_t font>
   volatile uint8_t PowerMonitor<CurrentSensor_t,font>::RunLoop = 1;
  
  
    template<typename CurrentSensor_t, font_t font>     
      PowerMonitor<CurrentSensor_t,font>::Power_t PowerMonitor<CurrentSensor_t,font>::Power;



    template<typename CurrentSensor_t>
    class PowerMonitor<CurrentSensor_t,Font_11x18>
    {
      public:
      enum
      {
        LINE_1 = 0,    
        FONT_W = 11,
        FONT_H = 18,
        LINE_2 = 1*FONT_H + 64/FONT_H  + 10,
        LINE_3 = 2*FONT_H + 64/FONT_H  + 10,       
        COLS = 128/FONT_W,
      };
      
      typedef Devices::SSD1306< i2c_t ,0x78,FONT_W,FONT_H,true> ssd1306_t;
      
      static char DispBuf[11];
      volatile static uint8_t RunLoop;      
      static uint16_t Seconds;
      static float CurrentSamples;
      static uint8_t SampleCount; 
      
      typedef typename CurrentSensor_t::Power_t Power_t;
      static Power_t Power;
      
      static void Init(void)
      {
        ssd1306_t::HwInit();
        CurrentSensor_t::HwInit();
        
          ssd1306_t::Puts("00:00 26.30v",11,0,LINE_1);
          ssd1306_t::Puts("I ",2,0,LINE_2);
          ssd1306_t::Puts("C ",2,0,LINE_3);       
      }
      
      static void UpdateClock(void)
      {   
          char TimeStr[10];          
          
          Seconds++;       
          
          if(! (Seconds % 60) )
          {
            CountertoTime(Seconds,TimeStr);
            
            ssd1306_t::Puts(TimeStr,5,0*FONT_W,LINE_1);
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
          
            memset((uint8_t*)&DispBuf[6],' ',COLS-6);
            Utils::ftoa(Power.Voltage,&DispBuf[6],2,' ');
            ssd1306_t::Puts(&DispBuf[6],COLS-6 ,6*FONT_W,LINE_1);
            
            memset((uint8_t*)DispBuf,' ',COLS);
            Utils::ftoa(Power.Current,DispBuf,2,' ');                 
            ssd1306_t::Puts(DispBuf,COLS-2,2*FONT_W,LINE_2);
            
            memset((uint8_t*)DispBuf,' ',COLS);
            Utils::ftoa(Power.mAH,DispBuf,2,' ');                 
            ssd1306_t::Puts(DispBuf,COLS-2,2*FONT_W,LINE_3);
          
          TestGPIO.Low();
      }
    };
    template<typename CurrentSensor_t>
    char PowerMonitor<CurrentSensor_t,Font_11x18>::DispBuf[11];
    
    template<typename CurrentSensor_t>
    volatile uint8_t PowerMonitor<CurrentSensor_t,Font_11x18>::RunLoop = 1;
    
    template<typename CurrentSensor_t>
    PowerMonitor<CurrentSensor_t,Font_11x18>::Power_t PowerMonitor<CurrentSensor_t,Font_11x18>::Power;
    
    template<typename CurrentSensor_t>
    uint16_t PowerMonitor<CurrentSensor_t,Font_11x18>::Seconds;
    
    template<typename CurrentSensor_t>
    float PowerMonitor<CurrentSensor_t,Font_11x18>::CurrentSamples;
    
    template<typename CurrentSensor_t>
    uint8_t PowerMonitor<CurrentSensor_t,Font_11x18>::SampleCount; 
    
  template<typename CurrentSensor_t>
    class PowerMonitor<CurrentSensor_t,Font_16x26>
    {
      public:
      enum
      {
        LINE_1 = 0,    
        FONT_W = 16,
        FONT_H = 26,
        LINE_2 = 1*FONT_H + 64/FONT_H  + 10,
        LINE_3 = 2*FONT_H + 64/FONT_H  + 10,       
        COLS = 128/FONT_W,
      };
      
      typedef Devices::SSD1306< i2c_t ,0x78,FONT_W,FONT_H,true> ssd1306_t;
      
      static char DispBuf[11];
      volatile static uint8_t RunLoop;      
      static uint32_t Seconds;
      
      typedef typename CurrentSensor_t::Power_t Power_t;
      static Power_t Power;
      
      static void Init(void)
      {
        ssd1306_t::HwInit();
        CurrentSensor_t::HwInit();
        
        ssd1306_t::Puts("V ",2,0,LINE_1);
        ssd1306_t::Puts("I ",2,0,LINE_2);
       
      }
      
      static void UpdateClock(void)
      {   

      }
      
      
      static void CaptureSamples(void)
      {          
          RunLoop = 1;          
      }
      
      static void Run()
      {
        // Run every 125 ms
          if(RunLoop) 
          {
            RunLoop = 0;
          }
          else
          {            
            return;
          }
          
          TestGPIO.High();
          Power.Voltage = CurrentSensor_t::GetBusVoltage_V(); // 26.0 V
          Power.Current = CurrentSensor_t::GetCurrent_mA();   // 3200.0 mA
          
          if(Power.Current < 0) 
          {
            Power.Current = 0;
          }
          
          if(Power.Voltage < 1) 
          {
            Power.Voltage = 0;
          }
          
          if( (Power.Current >= 400.0) || (Power.Voltage >= 16.0) )
          {
            CurrentSensor_t::SetCalibration_32V_2A();
          }
          else
          {
            CurrentSensor_t::SetCalibration_16V_400mA();
          }         
          
            memset((uint8_t*)&DispBuf[0],' ',COLS);
            Utils::ftoa(Power.Voltage,&DispBuf[0],2,' ');
            ssd1306_t::Puts(&DispBuf[0],COLS-2 ,2*FONT_W,LINE_1);
            
            memset((uint8_t*)DispBuf,' ',COLS);
            Utils::ftoa(Power.Current,DispBuf,2,' ');                 
            ssd1306_t::Puts(DispBuf,COLS-2,2*FONT_W,LINE_2);
          
          TestGPIO.Low();
      }
    };
    template<typename CurrentSensor_t>
    char PowerMonitor<CurrentSensor_t,Font_16x26>::DispBuf[11];
    
    template<typename CurrentSensor_t>
    volatile uint8_t PowerMonitor<CurrentSensor_t,Font_16x26>::RunLoop = 1;
    
    template<typename CurrentSensor_t>
    PowerMonitor<CurrentSensor_t,Font_16x26>::Power_t PowerMonitor<CurrentSensor_t,Font_16x26>::Power;
    
    template<typename CurrentSensor_t>
    uint32_t PowerMonitor<CurrentSensor_t,Font_16x26>::Seconds;
    
} // namespace App

#endif //POWER_MONITOR_H