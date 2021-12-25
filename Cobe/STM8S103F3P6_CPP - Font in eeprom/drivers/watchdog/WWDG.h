#ifndef WWDG_H
#define WWDG_H

#include "system.h" // STM8S_Std_Periph_Driver


//==================== Macro Declaration========================//
#define COUNTER_INIT            0x7F
#define WINDOW_VALUE            0x50
#define __IO                    volatile


#define WINDOW_10MS_TIMEOUT   76
#define WINDOW_20MS_TIMEOUT   89
#define WINDOW_30MS_TIMEOUT   102
#define WINDOW_40MS_TIMEOUT   115
#define WINDOW_49MS_TIMEOUT   127

//Byte Consumed :  without macro implementation = 26 Bytes
INLINE void WWDG_Feed(void){ if(((WWDG->CR & 0x7F) > 0x3F) && ((WWDG->CR & 0x7F) < WINDOW_VALUE)) {WWDG->CR |= (uint8_t)(0x7F);}}

//==================== Function Prototype======================//
void WWDG_Init(uint8_t Counter, uint8_t WindowValue);
uint8_t WWDG_GetCounter(void);
void WWDG_SetCounter(uint8_t Counter);
void WWDG_SetWindowValue(uint8_t WindowValue);
void WWDG_Test(void);

void WWDG_Start(uint8_t WindowTime);
#endif