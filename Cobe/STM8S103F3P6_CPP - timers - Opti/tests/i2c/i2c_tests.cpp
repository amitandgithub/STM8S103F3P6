

#include"i2c_tests.h"

#include <stdio.h>
#include <string.h>

i2c I2CDevIntr;
i2c::I2CStatus_t Status;
uint8_t RepeatedStart = 0;
uint8_t SlaveAddress = 0x10;
uint8_t TxBuf[16];
uint8_t RxBuf[15];
uint8_t TxLen,RxLen;
i2c::MasterTxn_t Transaction;

void I2CCallback(i2c::I2CStatus_t Status);
void I2C_Slave_Callback(i2c::I2CStatus_t status);

void I2c_Master_Tests()
{
  I2c_tests_t I2c_test_id = I2C_POLL_TX_1_RX_1;
  
  //sprintf(text[0], "Sum of %d and %d is %d", 1, 2, 10);
  
  I2CDevIntr.HwInit();
  
  I2CDevIntr.ScanBus(RxBuf,10);
  SlaveAddress = 0x68<<1;
  I2c_test_id = I2C_POLL_TX_1_RX_1;//I2C_POLL_TX_1_RX_1;//I2C_INT_TX_1_RX_1_TXN
  TxBuf[0] = 0x00;
  TxBuf[1] = 0x00;
  I2CDevIntr.XferPoll(SlaveAddress,TxBuf,2);       
  
  TxBuf[0] = 8;
  
  for(uint8_t i = 1; i<10; i++) TxBuf[i] = i;
  
  while(1)
  {
    switch(I2c_test_id)
    {            
    case I2C_POLL_TX_1_RX_1:
      TxLen = RxLen = 1;
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_1 = Pass"), STR("I2C_POLL_TX_1_RX_1 = Fail"));
      I2c_test_id = I2C_POLL_TX_1_RX_2;
      break;    
    case I2C_POLL_TX_1_RX_2:
      TxLen = 1;
      RxLen = 2;
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_2 = Pass"), STR("I2C_POLL_TX_1_RX_2 = Fail"));
      I2c_test_id = I2C_POLL_TX_1_RX_3;
      break; 
    case I2C_POLL_TX_1_RX_3:
      TxLen = 1;
      RxLen = 3;
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_3 = Pass"), STR("I2C_POLL_TX_1_RX_3 = Fail"));
      I2c_test_id = I2C_POLL_TX_2_RX_2;
      break; 
    case I2C_POLL_TX_2_RX_2:  
      TxLen = RxLen = 2;
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_2_RX_2 = Pass"), STR("I2C_POLL_TX_2_RX_2 = Fail"));
      I2c_test_id = I2C_POLL_TX_3_RX_3;
      break;  
      
    case I2C_POLL_TX_3_RX_3:  
      TxLen = RxLen = 3;
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart);   
      Test_Condition( !(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_3_RX_3 = Pass"), STR("I2C_POLL_TX_3_RX_3 = Fail"));
      I2c_test_id = I2C_POLL_TX_9_RX_9;
      break;
      
    case I2C_POLL_TX_9_RX_9: 
      TxLen = RxLen = 9;           
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart);            
      Test_Condition( !(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_9_RX_9 = Pass"), STR("I2C_POLL_TX_9_RX_9 = Fail"));
      I2c_test_id = I2C_INT_TX_1_RX_1_TXN;
      break;
      
#if I2C_INTERRUPT_MODE_TESTS
      
    case I2C_INT_TX_1_RX_1_TXN:
      TxLen = RxLen = 1;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = 0;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      //Transaction.pStatus = &Status;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != i2c::I2C_READY);
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() !=i2c::I2C_READY); 
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
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != i2c::I2C_READY);
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() !=i2c::I2C_READY); 
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
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != i2c::I2C_READY);
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() !=i2c::I2C_READY); 
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
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != i2c::I2C_READY);
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != i2c::I2C_READY); 
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
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != i2c::I2C_READY);
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() !=i2c::I2C_READY); 
      Test_Condition( !(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_3_RX_3_TXN = Pass"), STR("I2C_INT_TX_3_RX_3_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_9_RX_9_TXN;
      break;    
    case I2C_INT_TX_9_RX_9_TXN:
      TxLen = RxLen = 9;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = 0;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != i2c::I2C_READY);
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() !=i2c::I2C_READY);            
      
      Test_Condition( !(memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_40_RX_40_TXN = Pass"), STR("I2C_INT_TX_40_RX_40_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_QUEUE;			
      break;
#endif // I2C_INTERRUPT_MODE
      
    default:         
      if(RepeatedStart == 0)
      {
        I2c_test_id = I2C_POLL_TX_1_RX_1;
        RepeatedStart = 1;	
        //printf("\n\nRe-Testing test cases with repeated start \n\n");
      }
      else
      {
        RepeatedStart = 0;
        //return;
      }
      break;
    }
    
    MemSet(RxBuf,0,10);
    
    delay_ms(2000);
  }
}

void I2CCallback(i2c::I2CStatus_t Status)
{
  static uint8_t count;
  count++;
}
static uint8_t pass,fail;
bool Test_Condition(uint8_t condition, char* PassStr, char* FailStr)
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
  if(mem)
  {
    while(size--) 
      *mem++ = data;
  }
}


#if I2C_INTERRUPT_MODE_TESTS


#else // #endif // I2C_INTERRUPT_MODE_TESTS

void I2C_Slave_Tests(){}

#endif // I2C_INTERRUPT_MODE_TESTS



//#endif //I2C_POLL