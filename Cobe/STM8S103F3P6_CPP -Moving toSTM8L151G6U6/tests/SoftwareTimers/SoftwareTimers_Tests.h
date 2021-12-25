

/*
* SoftwareTimers_Tests.h
*
*  Created on: 10 Apr 2021
*      Author: Amit Chaudhary
*/

#ifndef SOFTWARE_TIMERS_TESTS_H
#define SOFTWARE_TIMERS_TESTS_H


#include "system.h"

    template<typename SoftwareTimer_t>
    class SoftwareTimers_Tests
    {
      public:
        
      static uint16_t ticks[3];
        
      static void Timer1_Callback(void)
      {
        TestGPIO.High();
        ticks[0]++;
        TestGPIO.Low();
      }
      
       static void Timer2_Callback(void)
      {
        //TestGPIO.High();
        ticks[1]++;
        //TestGPIO.Low();
      }
      
       static void Timer3_Callback(void)
      {
        //TestGPIO.High();
        ticks[2]++;
        //TestGPIO.Low();
      }

      static void Init(void)
      {
        SoftwareTimer_t::Init();
        SoftwareTimer_t::Start(SoftwareTimer_t::SOFTWARE_TIMER_1);
        SoftwareTimer_t::Start(SoftwareTimer_t::SOFTWARE_TIMER_2);
        SoftwareTimer_t::Start(SoftwareTimer_t::SOFTWARE_TIMER_3);
      }
      
      static void Run(void)
      {
        Init();
        
        while(1)
        {        
          SoftwareTimer_t::Run();
        }        
      }
    };
    template<typename SoftwareTimer_t>
      uint16_t SoftwareTimers_Tests<SoftwareTimer_t>::ticks[3];


#endif // SOFTWARE_TIMERS_TESTS_H
