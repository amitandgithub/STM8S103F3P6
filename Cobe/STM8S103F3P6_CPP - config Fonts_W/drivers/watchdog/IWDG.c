#include "IWDG.h"


/********************************************************************************
  * @brief  Enables or disables write access to Prescaler and Reload registers.
  * @param  IWDG_WriteAccess : New state of write access to Prescaler and Reload
  *         registers.  This parameter can be a value of @ref IWDG_WriteAccess_TypeDef.
  * @retval None
  ******************************************************************************
*/
void IWDG_WriteAccessCmd(IWDG_WriteAccess_TypeDef IWDG_WriteAccess)
{   
    //Write 0x55 to disable the write protection so that RLR and PR register can access
    IWDG->KR = (uint8_t)IWDG_WriteAccess;
}


/********************************************************************************
  * @brief  Sets IWDG Prescaler value.
  * @note   Write access should be enabled
  * @param  IWDG_Prescaler : Specifies the IWDG Prescaler value.
  *         This parameter can be a value of @ref IWDG_Prescaler_TypeDef.
  * @retval None
  ******************************************************************************
*/
void IWDG_SetPrescaler(IWDG_Prescaler_TypeDef IWDG_Prescaler)
{
    //Writes the prescalar value in IWDG_PR register
    IWDG->PR = (uint8_t)IWDG_Prescaler;
}

/********************************************************************************
  * @brief  Sets IWDG Reload value.
  * @note   Write access should be enabled
  * @param  IWDG_Reload : Reload register value.
  *         This parameter must be a number between 0 and 0xFF.
  * @retval None
  ******************************************************************************
*/
void IWDG_SetReload(uint8_t IWDG_Reload)
{
    //Writes the Reload value in IWDG_RLR register
    IWDG->RLR = IWDG_Reload;
}

/********************************************************************************
  * @brief  Reloads IWDG counter
  * @note   Write access should be enabled
  * @param  None
  * @retval None
  ******************************************************************************
*/
void IWDG_ReloadCounter(void)
{
    //Writes the Refresh Key in IWDG_KR register to prevent from reset
    IWDG->KR = IWDG_KEY_REFRESH;
}

/********************************************************************************
  * @brief  Enables IWDG.
  * @param  None
  * @retval None
  * @retval None
  ******************************************************************************
*/
void IWDG_Enable(void)
{
    //Enable the Window watchdog timer
    IWDG->KR = IWDG_KEY_ENABLE;
}

/********************************************************************************
  * @brief  Initialize the IWDG so that it generate a Reset if it is not refreshed at the
  *         correct time. 
  * @param  None
  * @retval None
  * Byte Consumed :  without macro implementation = 18 Bytes
  *                  with macro implementation = 834 Bytes
  ******************************************************************************
*/
void IWDG_Init(void)
{
    //Enable IWDG (the LSI oscillator will be enabled by hardware)
    IWDG_Enable();
  
    // Enable write access to IWDG_PR and IWDG_RLR registers
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* IWDG counter clock: LSI Clock/128 */
    IWDG_SetPrescaler(IWDG_Prescaler_128);

    /* Set counter reload value to obtain 54.0 ms IWDG Timeout.
    Counter Reload Value = LSI Clock / Prescaler 
                            = 38000 / 128
                            = 296.875 Hz 
                            = 0.054 / (1 / 296.875)
                            = 16.03125
                            = 16(Approx)
    */
    IWDG_SetReload((uint8_t)16);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();
}


   


#define INDEPENTWDG_CALC_MS(x)     roundNo(((float)38000 * (x)) / ((float)1000 * 256))  

/********************************************************************************
  * @brief  Initialize the IWDG so that it generate a Reset if it is not refreshed at the
  *         correct time. 
  * @param  Time in millisecond
  * @retval None
  * note :    Prescalar value         Max Timeout value(ms)
  *           IWDG_Prescaler_4   ---->  26.95
  *           IWDG_Prescaler_8   ---->  53.90
  *           IWDG_Prescaler_16  ---->  107.79
  *           IWDG_Prescaler_32  ---->  215.58
  *           IWDG_Prescaler_64  ---->  431.68
  *           IWDG_Prescaler_128 ---->  862.32      
  *           IWDG_Prescaler_256 ---->  1724.64     (Configured)
  * Take care : Need to change at IWDG->PR
  ******************************************************************************
*/
void IWDG_Start(uint8_t Time_in_ms)
{
    //const uint8_t CntReloadValue = INDEPENTWDG_CALC_MS(Time_in_ms);

    //Enable the Window watchdog timer
    IWDG->KR = IWDG_KEY_ENABLE;

    //Write 0x55 to disable the write protection so that RLR and PR register can access
    IWDG->KR = (uint8_t)IWDG_WriteAccess_Enable;

    //Writes the prescalar value in IWDG_PR register
    IWDG->PR = (uint8_t)IWDG_Prescaler_256;

    //Writes the Reload value in IWDG_RLR register
    IWDG->RLR = Time_in_ms;//(uint8_t)CntReloadValue; //Time_in_ms

    //Writes the Refresh Key in IWDG_KR register to prevent from reset
    IWDG->KR = IWDG_KEY_REFRESH;
}


void IWDG_Test(void)
{   
    //Initialize the Window watchdog Timer
     IWDG_Start(INDEPEND_WATCHDOG_200MS_TIMEOUT);//Independent_WatchDogStart(200); //or

    while(1)
    {
        /* Reload IWDG counter */
        //Independent_WatchDogFeed();

        //Provide a delay for 50 ms
        //delay_ms(50);
    }
}