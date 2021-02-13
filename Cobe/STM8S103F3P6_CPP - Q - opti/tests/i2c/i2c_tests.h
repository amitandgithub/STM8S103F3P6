


#ifndef i2c_tests_h
#define i2c_tests_h 

#include<stdio.h> 
#include<cstring>
#include "i2c.h"

#if I2C_INTERRUPT_MODE
#define I2C_INTERRUPT_MODE_TESTS 1
#else
#define I2C_INTERRUPT_MODE_TESTS 0
#endif

#define I2C_PRINT printf

void I2c_Master_Tests();
void I2C_Slave_Tests();
bool Test_Condition(uint8_t condition, char* PassStr, char* FailStr);
void MemSet(uint8_t* mem, uint8_t data, uint8_t size);

#define STR(x) ((char*)(x))


typedef enum
{
	/* Tests with Polling*/
    I2C_POLL_TX_1_RX_1,
    I2C_POLL_TX_1_RX_2,
    I2C_POLL_TX_1_RX_3,
    I2C_POLL_TX_2_RX_2,
    I2C_POLL_TX_3_RX_3,
    I2C_POLL_TX_9_RX_9,

	/* Tests with Interrupts*/
    I2C_INT_TX_1_RX_1,
    I2C_INT_TX_1_RX_2,
    I2C_INT_TX_1_RX_3,
    I2C_INT_TX_2_RX_2,
    I2C_INT_TX_3_RX_3,
    I2C_INT_TX_9_RX_9,

	/* Tests with Transaction*/
    I2C_INT_TX_1_RX_1_TXN,
    I2C_INT_TX_1_RX_2_TXN,
    I2C_INT_TX_1_RX_3_TXN,
    I2C_INT_TX_2_RX_2_TXN,
    I2C_INT_TX_3_RX_3_TXN,
    I2C_INT_TX_9_RX_9_TXN,
    
    /* Tests with Queue*/
    I2C_INT_TX_QUEUE,
	
    I2C_INT_DS1307_RTC_TIME,
    I2C_INT_DS1307_RTC_RAM,
    I2C_INT_DS1307_RTC_RAM_TXN,

}I2c_tests_t;


#endif //i2c_tests_h // i2c_tests_h