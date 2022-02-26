
/*
* Devices.h
*
*  Created on: 22 Apr 2021
*      Author: Amit Chaudhary
*/

#ifndef DEVICES_H
#define DEVICES_H

#include"HAL.h"

#include"AT24Cxx.h"
#include"SD1306.h"
#include"INA219.h"

namespace Devices
{

  
typedef Devices::AT24Cxx<i2c_t, 0xA0, 16 * 1024, 64> AT24C128_t;
typedef Devices::SSD1306< i2c_t ,0x78,true> ssd1306_t;
typedef Devices::INA219<i2c_t,0x80> ina219_t;

} // namespace Devices

#endif //DEVICES_H