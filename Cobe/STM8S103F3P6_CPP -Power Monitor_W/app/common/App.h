
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



typedef App::PowerMonitor<ina219_t> PowerMonitorApp_t; //Font_16x26  Font_11x18




}



#endif //APP_H