
/*
* App.h
*
*  Created on: 22 Apr 2021
*      Author: Amit Chaudhary
*/

#ifndef APP_H
#define APP_H

#include"HAL.h"
#include"Devices.h"

#include"PowerMonitor.h"

namespace App
{



typedef App::PowerMonitor<ssd1306_t,ina219_t,App::Font_11x18> PowerMonitorApp_t; //Font_16x26  Font_11x18




}



#endif //APP_H