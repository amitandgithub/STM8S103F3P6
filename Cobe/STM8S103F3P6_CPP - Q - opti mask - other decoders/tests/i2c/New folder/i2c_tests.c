/******************
** FILE: i2c_tests.c
**
** DESCRIPTION:
**  I2c Tests
**
** CREATED: 7/31/2019, by Amit Chaudhary
******************/
#include "i2c_tests.h"
#include <stdio.h>
#include <string.h>

I2CStatus_t Status;
uint8_t RepeatedStart = 0;
uint8_t SlaveAddress = 0x0010;
uint8_t TxBuf[TX_BUF_SIZE];
uint8_t RxBuf[RX_BUF_SIZE];
uint8_t TxLen,RxLen;
I2c_tests_t I2c_test_id;
Transaction_t Transaction;
void I2c_Poll_Tests()
{
  I2c_HwInit();  
  
  //I2cScanBus(TxBuf, TX_BUF_SIZE);  
  
  SlaveAddress = 0x68<<1;
  I2c_test_id = I2C_POLL_TX_1_RX_1;
  TxBuf[0] = 0x00;
  TxBuf[1] = 0x00;
  I2cXferPoll(SlaveAddress,TxBuf,2,0,0,0);       
  
  TxBuf[0] = 8;
  
  for(uint8_t i = 1; i<TX_BUF_SIZE; i++) TxBuf[i] = i;
  
  while(1)
  {
    switch(I2c_test_id)
    {            
    case I2C_POLL_TX_1_RX_1:
      TxLen = RxLen = 1;
      I2cXferPoll(SlaveAddress,TxBuf,TxLen+1,0,0,0);
      I2cXferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_1 = Pass"), STR("I2C_POLL_TX_1_RX_1 = Fail"));
      I2c_test_id = I2C_POLL_TX_1_RX_2;
      break;    
    case I2C_POLL_TX_1_RX_2:
      TxLen = 1;
      RxLen = 2;
      I2cXferPoll(SlaveAddress,TxBuf,TxLen+1,0,0,0);
      I2cXferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart);
      Test_Condition(!(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_2 = Pass"), STR("I2C_POLL_TX_1_RX_2 = Fail"));
      I2c_test_id = I2C_POLL_TX_1_RX_3;
      break; 
    case I2C_POLL_TX_1_RX_3:
      TxLen = 1;
      RxLen = 3;
      I2cXferPoll(SlaveAddress,TxBuf,TxLen+1,0,0,0);
      I2cXferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart);
      Test_Condition(!(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_3 = Pass"), STR("I2C_POLL_TX_1_RX_3 = Fail"));
      I2c_test_id = I2C_POLL_TX_2_RX_2;
      break; 
    case I2C_POLL_TX_2_RX_2:  
      TxLen = RxLen = 2;
      I2cXferPoll(SlaveAddress,TxBuf,TxLen+1,0,0,0);
      I2cXferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart);
      Test_Condition(!(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_2_RX_2 = Pass"), STR("I2C_POLL_TX_2_RX_2 = Fail"));
      I2c_test_id = I2C_POLL_TX_3_RX_3;
      break;      
    case I2C_POLL_TX_3_RX_3:  
      TxLen = RxLen = 3;
      I2cXferPoll(SlaveAddress,TxBuf,TxLen+1,0,0,0);
      I2cXferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart);  
      Test_Condition( !(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_3_RX_3 = Pass"), STR("I2C_POLL_TX_3_RX_3 = Fail"));
      I2c_test_id = I2C_POLL_TX_9_RX_9;
      break;      
    case I2C_POLL_TX_9_RX_9: 
      TxLen = RxLen = 9;           
      I2cXferPoll(SlaveAddress,TxBuf,TxLen+1,0,0,0);
      I2cXferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart);            
      Test_Condition( !(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_9_RX_9 = Pass"), STR("I2C_POLL_TX_9_RX_9 = Fail"));
      I2c_test_id = I2C_INT_TX_1_RX_1_TXN;
      break;
      
    case I2C_INT_TX_1_RX_1_TXN:
      TxLen = RxLen = 1;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = 0;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.pStatus = &Status;
      Transaction.XferDoneCallback = I2CCallback;            
      
      I2cXferIntr(&Transaction);
      while(I2cGetState() != I2C_READY);
      
      Transaction.TxLen = 1;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2cXferIntr(&Transaction);
      while(I2cGetState() != I2C_READY);
      
      Test_Condition( !(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_1_RX_1_TXN = Pass"), STR("I2C_INT_TX_1_RX_1_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_1_RX_2_TXN;
      break;    
    case I2C_INT_TX_1_RX_2_TXN:
      TxLen = 1;
      RxLen = 2;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = 0;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.pStatus = &Status;
      Transaction.XferDoneCallback = I2CCallback;            
      
      I2cXferIntr(&Transaction);
      while(I2cGetState() != I2C_READY);
      
      Transaction.TxLen = 1;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2cXferIntr(&Transaction);
      while(I2cGetState() != I2C_READY);
      
      I2cXferIntr(&Transaction);
      while(I2cGetState() != I2C_READY); 
      Test_Condition( !(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_1_RX_2_TXN = Pass"), STR("I2C_INT_TX_1_RX_2_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_1_RX_3_TXN;
      break; 
    case I2C_INT_TX_1_RX_3_TXN:
      TxLen = 1;
      RxLen = 3;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = 0;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.pStatus = &Status;
      Transaction.XferDoneCallback = I2CCallback;            
      
      I2cXferIntr(&Transaction);
      while(I2cGetState() != I2C_READY);
      
      Transaction.TxLen = 1;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2cXferIntr(&Transaction);
      while(I2cGetState() != I2C_READY);
      
      I2cXferIntr(&Transaction);
      while(I2cGetState() != I2C_READY); 
      Test_Condition( !(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_1_RX_3_TXN = Pass"), STR("I2C_INT_TX_1_RX_3_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_2_RX_2_TXN;
      break;
    case I2C_INT_TX_2_RX_2_TXN:  
      TxLen = RxLen = 2;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = 0;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.pStatus = &Status;
      Transaction.XferDoneCallback = I2CCallback;            
      
      I2cXferIntr(&Transaction);
      while(I2cGetState() != I2C_READY);
      
      Transaction.TxLen = 1;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2cXferIntr(&Transaction);
      while(I2cGetState() != I2C_READY);
      
      I2cXferIntr(&Transaction);
      while(I2cGetState() != I2C_READY); 
      Test_Condition( !(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_2_RX_2_TXN = Pass"), STR("I2C_INT_TX_2_RX_2_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_3_RX_3_TXN;
      break;            
    case I2C_INT_TX_3_RX_3_TXN:  
      TxLen = RxLen = 3;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = 0;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.pStatus = &Status;
      Transaction.XferDoneCallback = I2CCallback;            
      
      I2cXferIntr(&Transaction);
      while(I2cGetState() != I2C_READY);
      
      Transaction.TxLen = 1;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2cXferIntr(&Transaction);
      while(I2cGetState() != I2C_READY);
      
      I2cXferIntr(&Transaction);
      while(I2cGetState() != I2C_READY); 
      Test_Condition( !(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_3_RX_3_TXN = Pass"), STR("I2C_INT_TX_3_RX_3_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_9_RX_9_TXN;
      break;    
    case I2C_INT_TX_9_RX_9_TXN:
      // This test case tests the Transaction and the Callback functionality
      TxLen = RxLen = 9;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = 0;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.pStatus = &Status;
      Transaction.XferDoneCallback = I2CCallback;            
      
      I2cXferIntr(&Transaction);
      while(I2cGetState() != I2C_READY);
      
      Transaction.TxLen = 1;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2cXferIntr(&Transaction);
      while(I2cGetState() != I2C_READY);
      
      I2cXferIntr(&Transaction);           
      
      Test_Condition( !(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_9_RX_9_TXN = Pass"), STR("I2C_INT_TX_9_RX_9_TXN = Fail"));
      
      
      
      if(RepeatedStart == 0)
      {
        I2c_test_id = I2C_POLL_TX_1_RX_1;
        RepeatedStart = 1;	
        //printf("\n\nRe-Testing test cases with repeated start \n\n");
      }
      else
      {
        //return;
      }
      break;
      
    default: break;
    }
    
    MemSet(RxBuf,0,RX_BUF_SIZE);
    delay_ms(2000);
  }
}

void I2CCallback(void)
{
  static uint8_t count;
  count++;
}
uint8_t pass,fail;
uint8_t Test_Condition(uint8_t condition, char* PassStr, char* FailStr)
{
  if(condition)
  {
    //printf(PassStr);
    // printf("\n");
    pass++;
    return 0;
  }
  else
  {
    // printf(FailStr);
    // printf("\n");
    fail++;
    return 1;
  }   
  
}

void MemSet(uint8_t* mem, uint8_t data, uint8_t size)
{
  while(size--) *mem++ = data;
}