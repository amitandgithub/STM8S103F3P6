
#ifndef __EEPROM_h
#define __EEPROM_h

#include "system.h"
#include <string.h>
//#include "uart.h"
#define EEPROM_DEBUG_ENABLE 1

#if EEPROM_DEBUG_ENABLE
#define EEPROM_DEBUG_LOG(__STR)				Uart_TxStr(DEBUG_UART, __STR)
#define EEPROM_DEBUG_LOG_L(__STR)			Uart_TxStr(DEBUG_UART, __STR"\n\r")
#define EEPROM_DEBUG_LOG_HEX8(VAL)	        Uart_TxHex8(DEBUG_UART, VAL);
#define EEPROM_MEMDUMP(__BUF,__LEN)         Uart_TxBufPoll(DEBUG_UART,(uint8_t*) __BUF, (uint8_t)__LEN)
#else
#define PM_DEBUG_LOG(__STR)				
#define PM_DEBUG_LOG_HEX8(UARTX_,VAL)
#endif

#define OPERATION_TIMEOUT  ((uint16_t)0xFFFF)


typedef struct 
{
    uint8_t TCAM_Boot_Config;
    uint8_t Version;
    uint8_t Manufacturer[sizeof("Harman International") - 1];
    uint16_t Crash_events;
    uint32_t SystemUpTime;
}TCAM_eeprom_map_t;

#define EEPROM_MEM_START  (uint16_t)(0x1000 + 1024)

#define EEPROM_MEM_MAP  ((TCAM_eeprom_map_t*)EEPROM_MEM_START)

#define EEPROM_SET_MANUFACTURER(Pdata) eeprom_Write_Opti(EEPROM_MEM_MAP->Manufacturer,Pdata, sizeof(EEPROM_MEM_MAP->Manufacturer))

#define EEPROM_GET_MANUFACTURER(Pdata) eeprom_Read_Opti(Pdata, EEPROM_MEM_MAP->Manufacturer, sizeof(EEPROM_MEM_MAP->Manufacturer))

uint8_t eeprom_Write(uint8_t* Dst, uint8_t* Src, uint8_t Len);

void eeprom_Write_Opti(uint8_t *Dst, uint8_t *Src, uint8_t Len);

uint8_t eeprom_Read(uint8_t* Dst, uint8_t* Src, uint8_t Len);

uint8_t eeprom_Ready(void);

void eeprom_Test(void);

uint8_t eeprom_IsReady(uint8_t timeout);

INLINE void eeprom_Unlock(void)
{
    // Unlock EEPROM
    FLASH->DUKR = FLASH_RASS_KEY2;
    FLASH->DUKR = FLASH_RASS_KEY1;
}

INLINE void eeprom_Lock(void)
{
   FLASH->IAPSR &= (uint8_t)~FLASH_IAPSR_DUL;
}

INLINE void eeprom_Write_Opti(uint8_t *Dst, uint8_t *Src, uint8_t Len)
{
    memcpy(Dst, Src, Len);    
}

INLINE void eeprom_Read_Opti(uint8_t *Dst, uint8_t *Src, uint8_t Len)
{
    memcpy(Dst, Src, Len);    
}



























#endif // __EEPROM_h