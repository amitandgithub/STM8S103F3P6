
/*
* HAL.h
*
*  Created on: 22 Apr 2021
*      Author: Amit Chaudhary
*/

#ifndef HAL_H
#define HAL_H

#include"i2c.h"

#include"gpt1.h"
#include"gpt2.h"
#include"gpt4.h"
#include"pulseIn1.h"
#include"pulseIn2.h"
#include"pulseOut1.h"
#include"pulseOut2.h"


using namespace HAL;

namespace Devices
{
  
/********************* I2C *********************************/

typedef HAL::i2c::Master< HAL::i2c::MasterIntr<i2c::i2c1, i2c::CLK_400KHz> > i2c_t;

/********************* I2C *********************************/

/********************* Timers *********************************/
typedef HAL::gpt1<15999,0xffff,0> gpt1_t;

typedef HAL::gpt2<4,0xffff,0> gpt2_t;

typedef HAL::gpt4<HAL::gpt4_tick_4us,249> gpt4_t;

typedef pulseOut2<TIM2_CH3_A3,38000> PulseOut_t;

typedef pulseIn1<TIM1_CH1_C6_RMP,15,0xffff,TimerPol_Rising,34,0> PulseIn_t;

extern gpt1_t timer1;

extern gpt2_t timer2;

extern gpt4_t timer4;

extern PulseIn_t PulseIn;

extern PulseOut_t PulseOut;
/********************* Timers *********************************/

} //namespace Devices
#endif //HAL_H