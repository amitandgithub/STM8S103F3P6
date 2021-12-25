




#include "HAL_Objects.h"

#include "gpio_tests.h"
#include "i2c_tests.h"
#include "Sd1306.h"
#include "INA219.h"
#include "queue_test.h"
//
//#include"I2C_Commands.h"
//#include"Spi_tests.h"
//#include"RTF.h"
/////#include"UartTest.h"
#include"timers_test.h"
#include"SoftwareTimers.h"
#include"SoftwareTimers_Tests.h"

//#include"Templates.h"
//#include"adc_tests.h"

I2C_Test<200> I2C_Tests;
//Devices::SSD1306< i2c_t ,0x78,11,18,true> ssd1306;
// Devices::INA219< i2c_t , 0x80> ina219;

using namespace platform;

typedef platform::SoftwareTimers<gpt4_t,3> SoftwareTimer_t;

//queue_test queue_test_obj;

//SoftwareTimers_Tests<SoftwareTimer_t> SoftwareTimers_TestsObj;