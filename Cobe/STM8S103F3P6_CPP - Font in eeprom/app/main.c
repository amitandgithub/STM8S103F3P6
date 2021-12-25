
#include"system.h"
#include"tests.h"
#include"App.h" 
#include"Devices.h"


void main(void)
{
    CLK->CKDIVR = (uint8_t)0;
    
    TestLED.HwInit();
    TestGPIO.HwInit();
    __enable_interrupt(); 

    SoftwareTimer_t::Init();
    App::PowerMonitorApp_t::Init();
    while (1)
    {
      //gpio_tests();
      //I2C_Tests.Run();
      //Devices::ssd1306_t::Tests();
      //ina219.Tests();
      App::PowerMonitorApp_t::Run();
      SoftwareTimer_t::Run();
      //timer_tests();
      //SoftwareTimers_TestsObj.Run();
      
      //queue_test_obj.Run();
    }
}








































//template<uint8_t slaveAddress,uint8_t txLen, uint8_t rxLen, uint8_t repeatedStart>
//class TxnPoll
//{
//public:
//    uint8_t TxBuf[10]; 
//    uint8_t RxBuf[10];
//    
//    uint8_t send(){return (uint8_t)I2cXferPoll(slaveAddress,TxBuf, txLen, RxBuf, rxLen, repeatedStart);} 
//
//};






#ifdef  USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*   where the assert_param error has occurred.
* @param file: pointer to the source file name
* @param line: assert_param error line source number
* @retval : None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    
    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/