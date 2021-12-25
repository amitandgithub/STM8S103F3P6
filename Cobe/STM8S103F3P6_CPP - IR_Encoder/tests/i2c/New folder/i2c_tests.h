/******************
** FILE: i2c_tests.h
**
** DESCRIPTION:
**  I2c Tests
**
** CREATED: 7/31/2019, by Amit Chaudhary
******************/
#ifndef i2c_tests_h
#define i2c_tests_h


#include"system.h"

#define STR(x) ((char*)(x))
#define RX_BUF_SIZE 10
#define TX_BUF_SIZE 10

void I2c_Poll_Tests();
void I2c_Intr_Tests();
uint8_t Test_Condition(uint8_t condition, char* PassStr, char* FailStr);
void MemSet(uint8_t* mem, uint8_t data, uint8_t size);
void I2CCallback(void);
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
    I2C_INT_TX_1_RX_1_TXN,
    I2C_INT_TX_1_RX_2_TXN,
    I2C_INT_TX_1_RX_3_TXN,
    I2C_INT_TX_2_RX_2_TXN,
    I2C_INT_TX_3_RX_3_TXN,
    I2C_INT_TX_9_RX_9_TXN

}I2c_tests_t;
























#endif //i2c_tests_h
