
#include "eeprom.h"
#include <string.h>
//#include"uart.h"


uint8_t eeprom_IsReady(uint8_t timeout)
{
    while( (timeout-- != 0) && (FLASH->IAPSR & (uint8_t)( (FLASH_IAPSR_HVOFF | FLASH_IAPSR_WR_PG_DIS | FLASH_IAPSR_DUL))) != (FLASH_IAPSR_HVOFF | FLASH_IAPSR_DUL) )
    {
        __asm("NOP");
    }
    // EEPROM_DEBUG_LOG("<");
    // EEPROM_DEBUG_LOG_HEX8(timeout);
    // EEPROM_DEBUG_LOG(">");
    return timeout;
}


void eeprom_Test(void)
{
    static uint8_t Src[] = "Harman International";
    uint8_t* peeprom = EEPROM_MEM_MAP->Manufacturer;
    uint8_t Get[20];

    eeprom_Unlock();

    while(eeprom_IsReady(255) == 0);

    EEPROM_SET_MANUFACTURER(Src);
    EEPROM_GET_MANUFACTURER(Get);
    //EEPROM_DEBUG_LOG_L("");
    //EEPROM_MEMDUMP(Get,sizeof(Src)); 
    //EEPROM_DEBUG_LOG_L("");

    Src[sizeof(Src)-2] = (uint8_t)(Src[sizeof(Src)-2] + 1);
}





























#if 0

uint8_t eeprom_Write(uint8_t *Dst, uint8_t *Src, uint8_t Len)
{
    if ((Dst == 0) || (Src == 0) || (Len == 0))
    {
        return 1; // Invalid Params
    }

    if (((uint16_t)Dst + Len) > (FL_EEPROM_START_ADDRESS + FL_EEPROM_SIZE) || ((uint16_t)Dst < FL_EEPROM_START_ADDRESS))
    {
        return 2; // Mem Out of Range
    }

    // Unlock EEPROM
    FLASH->DUKR = FLASH_PUKR_RESET_VALUE;
    FLASH->DUKR = FLASH_DUKR_RESET_VALUE;

    if(eeprom_Ready() == 0)
    {
        memcpy(Dst, Src, Len);

        // Lock EEPROM
        FLASH->IAPSR &= (uint8_t)~FLASH_IAPSR_DUL;

        return 0;
    }

    return 3; // EEPROM_UNLOCK ERROR
}

uint8_t eeprom_Ready(void)
{
    uint16_t timeout = OPERATION_TIMEOUT;
    uint8_t flagstatus = 0x00;

    while((flagstatus == 0x00) && (timeout != 0x00))
    {
      flagstatus = (uint8_t)(FLASH->IAPSR & (uint8_t)(FLASH_IAPSR_HVOFF |
                                                      FLASH_IAPSR_WR_PG_DIS));
      timeout--;
    }

    if(timeout == 0)
    {
        return 4; // EEPROM not ready
    }

    return 0;
}

#endif