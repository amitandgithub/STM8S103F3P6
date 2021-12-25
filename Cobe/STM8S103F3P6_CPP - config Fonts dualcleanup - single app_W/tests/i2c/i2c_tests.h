

#ifndef i2c_tests_h
#define i2c_tests_h

#include <stdio.h>
#include <cstring>
#include"HAL_Objects.h"
#include"AT24Cxx.h"

using namespace HAL;
using namespace Devices;

#define I2C_PRINT printf

void I2c_Master_Tests(void);
void I2C_Slave_Tests(void);
bool Test_Condition(uint8_t condition, char *PassStr, char *FailStr);

#define STR(x) ((char *)(x))

template <uint16_t size>
struct eeprom_packet
{
  eeprom_packet(uint8_t da) : device_address(da)
  {
  }
  uint8_t device_address;
  uint16_t mem_address;
  uint8_t data[size];
};

template <uint16_t size>
struct I2C_Test
{

  typedef enum
  {
    /* Tests with Polling*/
    I2C_POLL_TX_1_RX_1,
    I2C_POLL_TX_1_RX_2,
    I2C_POLL_TX_1_RX_3,
    I2C_POLL_TX_2_RX_2,
    I2C_POLL_TX_3_RX_3,
    I2C_POLL_40_RX_40,
    I2C_POLL_64_RX_64,
    I2C_POLL_200_RX_200,
    I2C_POLL_FULL,

    /* Tests with Interrupts*/
    I2C_INT_TX_1_RX_1,
    I2C_INT_TX_1_RX_2,
    I2C_INT_TX_1_RX_3,
    I2C_INT_TX_2_RX_2,
    I2C_INT_TX_3_RX_3,
    I2C_INT_TX_40_RX_40,
    I2C_INT_TX_64_RX_64,

    /* Tests with Transaction*/
    I2C_INT_TX_1_RX_1_TXN,
    I2C_INT_TX_1_RX_2_TXN,
    I2C_INT_TX_1_RX_3_TXN,
    I2C_INT_TX_2_RX_2_TXN,
    I2C_INT_TX_3_RX_3_TXN,

    /* Tests with Queue*/
    I2C_INT_TX_QUEUE,

    I2C_INT_DS1307_RTC_TIME,
    I2C_INT_DS1307_RTC_RAM,
    I2C_INT_DS1307_RTC_RAM_TXN,
    I2C_ALL_TESTS_DONE

  } I2c_tests_t;

  AT24Cxx<i2c_t, 0xA0, 16 * 1024, 64> AT24C128;
  i2c_t I2CDevIntr;

  uint16_t mem_address;

  uint8_t TxLen, RxLen;
  uint8_t TxBuf[size + 2];
  uint8_t RxBuf[size];
  I2c_tests_t I2c_test_id;

  uint8_t pass;

  void Init(void)
  {

    i2c_t::HwInit();
    i2c_t::ScanBus(RxBuf, 10);
    
    TxBuf[0] = 0;
    TxBuf[1] = 250;

    mem_address = (uint16_t)TxBuf[0] << 8U | TxBuf[1];

    for (uint8_t i = 2; i < sizeof(TxBuf); i++)
    {
      TxBuf[i] = 0 + i - 2;
    }

    I2c_test_id = I2C_INT_TX_1_RX_1;//I2C_INT_TX_1_RX_1;
  }

  void Run(void)
  {
    Init();

    while (1)
    {
      //Master_Poll();
      Master_Intr();
    }
  }

  void Master_Poll(void)
  {
    switch (I2c_test_id)
    {
    case I2C_POLL_TX_1_RX_1:
      TxLen = 1;
      RxLen = 1;
      AT24C128.page_Write(TxBuf, TxLen + 2); 
        
      AT24C128.Read(mem_address, &RxBuf[0], RxLen);
      Test_Condition(!(memcmp((const void *)&TxBuf[2], (const void *)RxBuf, TxLen)), "P", "F");
      I2c_test_id = I2C_POLL_TX_1_RX_2;
      break;

    case I2C_POLL_TX_1_RX_2:
      TxLen = 1;
      RxLen = 2;
      AT24C128.page_Write(TxBuf, TxLen + 2);
      AT24C128.Read(mem_address, &RxBuf[0], RxLen);
      Test_Condition(!(memcmp((const void *)&TxBuf[2], (const void *)RxBuf, TxLen)), "P", "F");
      I2c_test_id = I2C_POLL_TX_1_RX_3;
      break;

    case I2C_POLL_TX_1_RX_3:
      TxLen = 1;
      RxLen = 3;
      AT24C128.page_Write(TxBuf, TxLen + 2);
      AT24C128.Read(mem_address, &RxBuf[0], RxLen);
      Test_Condition(!(memcmp((const void *)&TxBuf[2], (const void *)RxBuf, TxLen)), "P", "F");
      I2c_test_id = I2C_POLL_TX_2_RX_2;
      break;

    case I2C_POLL_TX_2_RX_2:
      TxLen = 2;
      RxLen = 2;
      AT24C128.page_Write(TxBuf, TxLen + 2);
      AT24C128.Read(mem_address, &RxBuf[0], RxLen);
      Test_Condition(!(memcmp((const void *)&TxBuf[2], (const void *)RxBuf, TxLen)), "P", "F");
      I2c_test_id = I2C_POLL_TX_3_RX_3;
      break;

    case I2C_POLL_TX_3_RX_3:
      TxLen = 3;
      RxLen = 3;
      AT24C128.page_Write(TxBuf, TxLen + 2);
      AT24C128.Read(mem_address, &RxBuf[0], RxLen);
      Test_Condition(!(memcmp((const void *)&TxBuf[2], (const void *)RxBuf, TxLen)), "P", "F");
      I2c_test_id = I2C_POLL_40_RX_40;
      break;

    case I2C_POLL_40_RX_40:
      TxLen = 40;
      RxLen = 40;
      AT24C128.Write(mem_address, &TxBuf[2], TxLen);
      AT24C128.Read(mem_address, &RxBuf[0], RxLen);
      AT24C128.Write(mem_address, &TxBuf[2], TxLen);
      AT24C128.Read(mem_address, &RxBuf[0], RxLen);
      Test_Condition(!(memcmp((const void *)&TxBuf[2], (const void *)RxBuf, TxLen)), "P", "F");
      I2c_test_id = I2C_POLL_64_RX_64;
      break;

    case I2C_POLL_64_RX_64:
      TxLen = 64;
      RxLen = 64;
      AT24C128.Write(mem_address, &TxBuf[2], TxLen);
      AT24C128.Read(mem_address, &RxBuf[0], RxLen);
      Test_Condition(!(memcmp((const void *)&TxBuf[2], (const void *)RxBuf, TxLen)), "P", "F");
      I2c_test_id = I2C_POLL_200_RX_200;
      break;

    case I2C_POLL_200_RX_200:
      TxLen = 200;
      RxLen = 200;
      AT24C128.Write(mem_address, &TxBuf[2], TxLen);
      AT24C128.Read(mem_address, &RxBuf[0], RxLen);
      Test_Condition(!(memcmp((const void *)&TxBuf[2], (const void *)RxBuf, TxLen)), "P", "F");
      I2c_test_id = I2C_POLL_FULL;
      break;

      case I2C_POLL_FULL:
      for(uint16_t pages = 0; pages < AT24C128.Pages-1; pages++)
      {
        for (uint8_t i = 0; i < 64; i++)
        {
          TxBuf[i] = pages;
        }
        
        AT24C128.Write(pages*64, &TxBuf[2], TxLen);
        AT24C128.Read(pages*64, &RxBuf[0], RxLen);
        Test_Condition(!(memcmp((const void *)&TxBuf[2], (const void *)RxBuf, TxLen)), "P", "F");       
      }
      I2c_test_id = I2C_ALL_TESTS_DONE;
      break;

     case I2C_ALL_TESTS_DONE:
     break;
     
    default: break;
      
    }

    //MemSet(RxBuf, 0, sizeof(RxBuf));
    memset(RxBuf, 0, sizeof(RxBuf));

    delay_ms(100);
  }

void Master_Intr(void)
  {
    switch (I2c_test_id)
    {
    case I2C_INT_TX_1_RX_1:
      TxLen = 1;
      RxLen = 1;
      AT24C128.page_Write_Intr(TxBuf, TxLen + 2);
      AT24C128.page_Read_Intr(mem_address, &RxBuf[0], RxLen);
      Test_Condition(!(memcmp((const void *)&TxBuf[2], (const void *)RxBuf, TxLen)), "P", "F");
      I2c_test_id = I2C_INT_TX_1_RX_2;
      break;

    case I2C_INT_TX_1_RX_2:
      TxLen = 1;
      RxLen = 2;
      AT24C128.page_Write_Intr(TxBuf, TxLen + 2);
      AT24C128.page_Read_Intr(mem_address, &RxBuf[0], RxLen);
      Test_Condition(!(memcmp((const void *)&TxBuf[2], (const void *)RxBuf, TxLen)), "P", "F");
      I2c_test_id = I2C_INT_TX_1_RX_3;
      break;

    case I2C_INT_TX_1_RX_3:
      TxLen = 1;
      RxLen = 3;
      AT24C128.page_Write_Intr(TxBuf, TxLen + 2);
      AT24C128.page_Read_Intr(mem_address, &RxBuf[0], RxLen);
      Test_Condition(!(memcmp((const void *)&TxBuf[2], (const void *)RxBuf, TxLen)), "P", "F");
      I2c_test_id = I2C_INT_TX_2_RX_2_TXN;
      break;

    case I2C_INT_TX_2_RX_2_TXN:
      TxLen = 2;
      RxLen = 2;
      AT24C128.page_Write_Intr(TxBuf, TxLen + 2);
      AT24C128.page_Read_Intr(mem_address, &RxBuf[0], RxLen);
      Test_Condition(!(memcmp((const void *)&TxBuf[2], (const void *)RxBuf, TxLen)), "P", "F");
      I2c_test_id = I2C_INT_TX_3_RX_3;
      break;

    case I2C_INT_TX_3_RX_3:
      TxLen = 3;
      RxLen = 3;
      AT24C128.page_Write_Intr(TxBuf, TxLen + 2);
      AT24C128.page_Read_Intr(mem_address, &RxBuf[0], RxLen);
      Test_Condition(!(memcmp((const void *)&TxBuf[2], (const void *)RxBuf, TxLen)), "P", "F");
      I2c_test_id = I2C_INT_TX_40_RX_40;
      break;

    case I2C_INT_TX_40_RX_40:
      TxLen = 40;
      RxLen = 40;
      AT24C128.page_Write_Intr(TxBuf, TxLen + 2);
      AT24C128.page_Read_Intr(mem_address, &RxBuf[0], RxLen);
      Test_Condition(!(memcmp((const void *)&TxBuf[2], (const void *)RxBuf, TxLen)), "P", "F");
      I2c_test_id = I2C_ALL_TESTS_DONE;
      break;
      
    case I2C_ALL_TESTS_DONE:
     break;

    default: break;

    }

    memset(RxBuf, 0, sizeof(RxBuf));
    //MemSet(RxBuf, 0, sizeof(RxBuf));

    delay_ms(100);
  }

  void Test_Condition(uint8_t condition, char *PassStr, char *FailStr)
  {
    if (condition)
    {
      //printf(PassStr);
      // printf("\n");
      pass++;
    }
    else
    {
      // printf(FailStr);
      // printf("\n");
      //fail++;
      while (1)
        ;
    }
  }


};

#endif //i2c_tests_h // i2c_tests_h