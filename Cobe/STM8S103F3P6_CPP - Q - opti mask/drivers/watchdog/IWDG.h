#ifndef IWDG_H
#define IWDG_H

#include "system.h"


//=========================Macro Decalartion======================//
#define INDEPEND_WATCHDOG_10MS_TIMEOUT          1
#define INDEPEND_WATCHDOG_20MS_TIMEOUT          3
#define INDEPEND_WATCHDOG_50MS_TIMEOUT          7
#define INDEPEND_WATCHDOG_100MS_TIMEOUT         15
#define INDEPEND_WATCHDOG_200MS_TIMEOUT         30
#define INDEPEND_WATCHDOG_300MS_TIMEOUT         45
#define INDEPEND_WATCHDOG_400MS_TIMEOUT         59
#define INDEPEND_WATCHDOG_500MS_TIMEOUT         74
#define INDEPEND_WATCHDOG_600MS_TIMEOUT         89
#define INDEPEND_WATCHDOG_700MS_TIMEOUT         104
#define INDEPEND_WATCHDOG_800MS_TIMEOUT         119
#define INDEPEND_WATCHDOG_900MS_TIMEOUT         134
#define INDEPEND_WATCHDOG_1000MS_TIMEOUT        148
#define INDEPEND_WATCHDOG_1100MS_TIMEOUT        163
#define INDEPEND_WATCHDOG_1200MS_TIMEOUT        178
#define INDEPEND_WATCHDOG_1300MS_TIMEOUT        193
#define INDEPEND_WATCHDOG_1400MS_TIMEOUT        208
#define INDEPEND_WATCHDOG_1500MS_TIMEOUT        223
#define INDEPEND_WATCHDOG_1600MS_TIMEOUT        238
#define INDEPEND_WATCHDOG_1700MS_TIMEOUT        252


#define roundNo(num)   ((num) < 0 ? (num) - 0.5 : (num) + 0.5)

////=========================Enumeration Decalartion======================//
///** IWDG write  access enumeration */
//typedef enum
//{
//  IWDG_WriteAccess_Enable  = (uint8_t)0x55, /*!< Code 0x55 in Key register, allow write access to Prescaler and Reload registers */
//  IWDG_WriteAccess_Disable = (uint8_t)0x00  /*!< Code 0x00 in Key register, not allow write access to Prescaler and Reload registers */
//} IWDG_WriteAccess_TypeDef;
//
///** IWDG prescaler enumaration */
//typedef enum
//{
//  IWDG_Prescaler_4   = (uint8_t)0x00, /*!< Used to set prescaler register to 4 */
//  IWDG_Prescaler_8   = (uint8_t)0x01, /*!< Used to set prescaler register to 8 */
//  IWDG_Prescaler_16  = (uint8_t)0x02, /*!< Used to set prescaler register to 16 */
//  IWDG_Prescaler_32  = (uint8_t)0x03, /*!< Used to set prescaler register to 32 */
//  IWDG_Prescaler_64  = (uint8_t)0x04, /*!< Used to set prescaler register to 64 */
//  IWDG_Prescaler_128 = (uint8_t)0x05, /*!< Used to set prescaler register to 128 */
//  IWDG_Prescaler_256 = (uint8_t)0x06  /*!< Used to set prescaler register to 256 */
//} IWDG_Prescaler_TypeDef;

//Byte Consumed :  without macro implementation = 4 Bytes
INLINE void IWDG_Feed(void)        { IWDG->KR = IWDG_KEY_REFRESH; } //Writes the Refresh Key in IWDG_KR register to prevent from reset


//===========================Function Prototype========================//
void IWDG_WriteAccessCmd(IWDG_WriteAccess_TypeDef IWDG_WriteAccess);
void IWDG_SetPrescaler(IWDG_Prescaler_TypeDef IWDG_Prescaler);
void IWDG_SetReload(uint8_t IWDG_Reload);
void IWDG_ReloadCounter(void);
void IWDG_Enable(void);
void IWDG_Init(void);
void IWDG_Test(void);


void IWDG_Start(uint8_t Time_in_ms); 
#endif