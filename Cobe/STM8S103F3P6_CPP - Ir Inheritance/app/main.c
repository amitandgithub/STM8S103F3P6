
#include"system.h"
#include"tests.h"
#include"interrupt.h"
#include"eeprom.h"

extern Cmd_t myI2C_CMD;
typedef reg_t<rw_t,0x50B3,0,8> Rst_SR;
typedef reg_t<rw_t,0x50C6,3,2> CLK_HSIDIV;

void main(void)
{
    CLK->CKDIVR = (uint8_t)0;
    //CLK_HSIDIV::write(0);
    __enable_interrupt(); 
    //I2C_Slave_Tests(); 
    //Cmdsvr_Init();
    TestGPIO.HwInit();
    TestLED.HwInit();
    //RTF_Init();
   // RST->SR  = 0xff;
   // Rst_SR::write(0xff);
   //*(uint8_t*)0x50B3 |= 1;
    while (1)
    {
      //CmdSvr_Run();     
       //gpio_tests();
      //I2c_Master_Tests();
       //Spi_Tests();
      // RTF_Run();
      //Uart_Test();
      //Templates_Test();
      //template_test();
      timer_tests();
      // delay_ms(1000);
        
    }
}









/*


*/







//void Gpio_1_Callback();



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