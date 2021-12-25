
/*
* INA219.h
*
*  Created on: 4 Apr 2021
*      Author: Amit Chaudhary
*/

#ifndef HAL_OBJECTS_H
#define HAL_OBJECTS_H

#include"i2cPoll.h"
#include"i2cMasterIntr.h"

#include"gpt1.h"
#include"gpt2.h"
#include"gpt4.h"
#include"pulseIn1.h"
#include"pulseIn2.h"
#include"pulseOut1.h"
#include"pulseOut2.h"

#include"AT24Cxx.h"
//#include"SSD1306.h"


using namespace HAL;

/********************* I2C *********************************/
//typedef HAL::i2c::MasterIntr<i2c::i2c1, i2c::CLK_400KHz> I2C_Obj;

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



typedef Devices::AT24Cxx<i2c_t, 0xA0, 16 * 1024, 64> AT24C128_t;
//typedef Devices::SSD1306< i2c_t ,0x78,11,18,true> ssd1306_t;



#endif //HAL_OBJECTS_H