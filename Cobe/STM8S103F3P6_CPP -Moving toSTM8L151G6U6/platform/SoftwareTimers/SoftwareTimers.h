

/*
* SoftwareTimers.h
*
*  Created on: 10 Apr 2021
*      Author: Amit Chaudhary
*/

#ifndef SOFTWARE_TIMERS_H
#define SOFTWARE_TIMERS_H

// SoftwareTimers_Tests.h
#include "system.h"
#include "SoftwareTimers_Tests.h"
#include "App.h"

namespace platform
{
    typedef void (*SoftwareTimersCallback_t)(void);

    template <typename timer_t, uint8_t timers>
    class SoftwareTimers
    {

    private:
    public:
        typedef enum
        {
            SOFTWARE_TIMER_1,
            SOFTWARE_TIMER_2,
            SOFTWARE_TIMER_MAX
        } Timer_ID_t;        

        typedef enum
        {
            INACTIVE    = (uint8_t)0x00,
            ACTIVE      = (uint8_t)0x01,
            EXPIRED     = (uint8_t)0x02,
            PAUSED      = (uint8_t)0x04
        } State_t;

        typedef struct
        {
            uint16_t    LastRun;
            uint16_t    Delay;
            State_t     State;
            uint8_t     Periodic;
            SoftwareTimersCallback_t  Callback;
        } Timer_t;

        static Timer_t Timers[SOFTWARE_TIMER_MAX];

        static uint16_t Ticks(void)
        {
          return timer_t::GetTicks();
        }
        
        static void Init(void)
        {          
          timer_t::HwInit();
          timer_t::Start();
          timer_t::InterruptEnable(timer_t::UIE);
        }

        static void Run(void)
        {
         // TestGPIO.High();
          
          uint16_t Delta;
          uint8_t timer_id;
          uint16_t ticks = Ticks();
          
          for (timer_id = 0; timer_id < SOFTWARE_TIMER_MAX; timer_id++)
          {
            if (Timers[timer_id].State == ACTIVE)
            {
              if(ticks >= Timers[timer_id].LastRun)
              {
                Delta = ticks - Timers[timer_id].LastRun;
              }
              else
              {
                Delta = ticks - Timers[timer_id].LastRun + 0xFFFF;
              }
                
              if (Delta >= Timers[timer_id].Delay)
              {
                if (Timers[timer_id].Periodic == 0)
                {
                  Timers[timer_id].State = INACTIVE;
                }
                else
                {
                  Timers[timer_id].LastRun = ticks;
                }
                
                Timers[timer_id].Callback();
              }
            }
          }
          
          //TestGPIO.Low();
        }        
        
        static void Start(Timer_ID_t timer_id)
        {
          Timers[timer_id].LastRun = Ticks();
          Timers[timer_id].State = ACTIVE;
        }
        
        static void Start(Timer_ID_t timer_id, SoftwareTimersCallback_t Callback )
        {
          Timers[timer_id].Callback = Callback;
          Timers[timer_id].LastRun = Ticks();
          Timers[timer_id].State = ACTIVE;
        }
                         
        static void Stop(Timer_ID_t timer_id)
        {
          Timers[timer_id].State = INACTIVE;
        }
    };
    
    template <typename timer_t, uint8_t timers>
      SoftwareTimers<timer_t,timers>::Timer_t SoftwareTimers<timer_t,timers>::Timers[SoftwareTimers<timer_t,timers>::SOFTWARE_TIMER_MAX] = 
      { 
        {0,125,  ACTIVE,1,App::PowerMonitorApp_t::CaptureSamples},
        {0,1000-55, ACTIVE,1,App::PowerMonitorApp_t::SecondTicks} // +-55ms for error compensation
      };
}

#endif // SOFTWARE_TIMERS_H
