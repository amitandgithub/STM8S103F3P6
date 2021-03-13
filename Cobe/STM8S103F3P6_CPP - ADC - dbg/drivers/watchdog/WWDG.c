#include "WWDG.h"
#include "IWDG.h"

/* WWDG configuration: WWDG is clocked by SYSCLK = 2MHz */
/* WWDG timeout is equal to 251,9 ms */
/* Watchdog Window = (COUNTER_INIT - 63) * 1 step
                    = 63 * (12288 / 16Mhz)
                    = 49.152 ms
*/
/* Non Allowed Window = (COUNTER_INIT - WINDOW_VALUE) * 1 step
                    = (127 - 80) * 1 step
                    =  47 * 1 step 
                    =  47 * (12288 / 16Mhz) 
                    =  36.096 ms
*/
/* Window size is   = (Watchdog Window - Non Allowed Window)
                    = (49.152 - 36.096)ms
                    = 13.096 ms
*/                    
/* So the non allowed window starts from 0.0 ms to 36.096 ms
and the allowed window starts from 36.096 ms to 49.152 ms
If refresh is done during non allowed window, a reset will occur.
If refresh is done during allowed window, no reset will occur.
If the WWDG down counter reaches 63, a reset will occur. */


/* Private define ------------------------------------------------------------*/
#define BIT_MASK          ((uint8_t)0x7F)

/********************************************************************************
  * @brief Window watchdog Initilization.
  *         This function set Window Register = WindowValue, Counter Register
  *         according to Counter
  * @param  Counter : WWDG counter value
  * @param  WindowValue : specifies the WWDG Window Register, range is 0x00 to 0x7F.
  * @retval void None.
  ******************************************************************************
*/
void WWDG_Init(uint8_t Counter, uint8_t WindowValue)
{
    //Write 0x7F to the window register
    WWDG->WR = WWDG_WR_RESET_VALUE;

    /*Enable the Window watchdog timer, set the T6 bit (To avoid accidental reset generation)
    and counter value  */
    WWDG->CR = (uint8_t)((uint8_t)(WWDG_CR_WDGA | WWDG_CR_T6) | (uint8_t)Counter);

    /*Write the window register value for allowed window with T6 bit set because the 
    WR value should be from 0x7F to 0xC0*/
    WWDG->WR = (uint8_t)((uint8_t)(~WWDG_CR_WDGA) & (uint8_t)(WWDG_CR_T6 | WindowValue));
}



/********************************************************************************
  * @brief Gets the WWDG Counter Value.
  *        This value could be used to check if WWDG is in the window, where
  *        refresh is allowed.
  * @param  None
  * @retval WWDG Counter Value
  ******************************************************************************
*/
uint8_t WWDG_GetCounter(void)
{
    // Return the counter value
    return(WWDG->CR);
}


/********************************************************************************
  * @brief  Refreshes the WWDG peripheral.
  * @param  Counter :  WWDG Counter Value
  *         This parameter must be a number between 0x40 and 0x7F.
  * @retval None
  ******************************************************************************
*/
void WWDG_SetCounter(uint8_t Counter)
{
    /* Write to T[6:0] bits to configure the counter value, no need to do
    a read-modify-write; writing a 0 to WDGA bit does nothing */
    WWDG->CR = (uint8_t)(Counter & (uint8_t)BIT_MASK);
}


/********************************************************************************
  * @brief  Sets the WWDG window value.
  * @param  WindowValue: specifies the window value to be compared to the
  *         downcounter.
  *         This parameter value must be lower than 0x80.
  * @retval None
  ******************************************************************************
*/
void WWDG_SetWindowValue(uint8_t WindowValue)
{
    //Set the new window value and T6 bit set
    WWDG->WR = (uint8_t)((uint8_t)(~WWDG_CR_WDGA) & (uint8_t)(WWDG_CR_T6 | WindowValue));
}



#define WINDOW_CALC_MS(x)     (roundNo((((float)16000000 * (x)) / ((float)12233 * 1000))) + 63)

/********************************************************************************
  * @brief Window watchdog Initilization.
  *         This function set Window Register = WindowValue, Counter Register
  * @param  Time in millisecond
  * @retval None
  * Note :    SYS Clock            Max Window size(ms)
  *            16000000 ------->    49.152
  *            8000000  ------->    98.304
  *            4000000  ------->    196.608
  * Byte Consumed :  without macro implementation = 20 Bytes
  *                  with macro implementation = 846 Bytes
  ******************************************************************************
*/

void WWDG_Start(uint8_t WindowTime)
{
    //const uint8_t WindowValue = WINDOW_CALC_MS(WindowTime);//(roundOff;

    //Write 0x7F to the window register
    WWDG->WR = WWDG_WR_RESET_VALUE;

    //Enable the Window watchdog timer, set the T6 bit (To avoid accidental reset generation)
    //and counter value  /
    WWDG->CR = (uint8_t)((uint8_t)(WWDG_CR_WDGA | WWDG_CR_T6) | (uint8_t)0x7F);

    //Write the window register value for allowed window with T6 bit set because the 
    //WR value should be from 0x7F to 0xC0/
    WWDG->WR = (uint8_t)((uint8_t)(~WWDG_CR_WDGA) & (uint8_t)(WWDG_CR_T6 | WindowTime));//WindowValue
}


void WWDG_Test(void)
{
    //Initialze the window watchdog timer 
    WWDG_Start(WINDOW_30MS_TIMEOUT);

    //Dead Loop
    while(1)
    {
        WWDG_Feed();
    }
}