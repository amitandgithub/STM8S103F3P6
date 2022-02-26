

#include"i2c_tests.h"

#include <stdio.h>
#include <string.h>

#define AT24C128_Address 0xA0
#define AT24C128_DELAY 3

I2C_t I2CDevIntr;
i2c::Status_t Status;
uint8_t RepeatedStart = 0;
uint8_t SlaveAddress = 0x10;

uint8_t TxLen,RxLen;
i2c::MasterTxn_t Transaction;

void I2CCallback(i2c::Status_t Status);
void I2C_Slave_Callback(i2c::Status_t status);

eeprom_packet<200> AT24C_packet(AT24C128_Address);

AT24Cxx <I2C_t,AT24C128_Address,16*1024,64> AT24C128;

void I2c_Master_Tests(void)
{
  uint16_t mem_address;
  
  static uint8_t TxBuf[200+2];
  static uint8_t RxBuf[200];

  I2c_tests_t I2c_test_id = I2C_POLL_TX_1_RX_1;
  
  I2CDevIntr.HwInit();
  
  I2CDevIntr.ScanBus(RxBuf,10);
  SlaveAddress = AT24C128_Address;
  I2c_test_id = I2C_POLL_TX_1_RX_1;//I2C_POLL_TX_1_RX_1;//I2C_INT_TX_1_RX_1_TXN,I2C_POLL_64_RX_64
  TxBuf[0] = 0;
  TxBuf[1] = 100;
  
  mem_address = (uint16_t)TxBuf[0]<<8U | TxBuf[1];

  for(uint8_t i = 2; i<sizeof(TxBuf); i++) TxBuf[i] = 'A' + i - 2;
  
  while(1)
  {
    switch(I2c_test_id)
    {            
    case I2C_POLL_TX_1_RX_1:
      TxLen = 1 ;
      RxLen = 1;
      
      AT24C128.byte_Write(0,TxBuf[2]);
      RxBuf[0] = AT24C128.byte_Read(0);
      Test_Condition(!(memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_2 = Pass"), STR("I2C_POLL_TX_1_RX_2 = Fail"));
      
      I2c_test_id = I2C_POLL_TX_1_RX_2;      
      break;    
    case I2C_POLL_TX_1_RX_2:
      TxLen = 1;
      RxLen = 2;
      
      AT24C128.Write(mem_address,&TxBuf[2],TxLen);
      //delay_ms(AT24C128_DELAY);      
      AT24C128.Read(mem_address,&RxBuf[0],RxLen);      
      Test_Condition( !(memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_9_RX_9 = Pass"), STR("I2C_POLL_TX_9_RX_9 = Fail"));
      
      I2c_test_id = I2C_POLL_TX_1_RX_3;
      break; 
    case I2C_POLL_TX_1_RX_3:
      TxLen = 1;
      RxLen = 3;
      
      AT24C128.Write(mem_address,&TxBuf[2],TxLen);
      //delay_ms(AT24C128_DELAY);      
      AT24C128.Read(mem_address,&RxBuf[0],RxLen);      
      Test_Condition( !(memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_9_RX_9 = Pass"), STR("I2C_POLL_TX_9_RX_9 = Fail"));
      
      I2c_test_id = I2C_POLL_TX_2_RX_2;
      break; 
    case I2C_POLL_TX_2_RX_2:  
      TxLen = 2;
      RxLen = 2;
      
      AT24C128.Write(mem_address,&TxBuf[2],TxLen);
      //delay_ms(AT24C128_DELAY);      
      AT24C128.Read(mem_address,&RxBuf[0],RxLen);      
      Test_Condition( !(memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_9_RX_9 = Pass"), STR("I2C_POLL_TX_9_RX_9 = Fail"));
      
      I2c_test_id = I2C_POLL_TX_3_RX_3;
      break;  
      
    case I2C_POLL_TX_3_RX_3:  
      TxLen = 3;
      RxLen = 3;
     
      AT24C128.Write(mem_address,&TxBuf[2],TxLen);
      //delay_ms(AT24C128_DELAY);      
      AT24C128.Read(mem_address,&RxBuf[0],RxLen);      
      Test_Condition( !(memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_9_RX_9 = Pass"), STR("I2C_POLL_TX_9_RX_9 = Fail"));
      
      I2c_test_id = I2C_POLL_40_RX_40;
      break;
      
    case I2C_POLL_40_RX_40: 
  
      TxLen = 40;
      RxLen = 40;     
      
      AT24C128.Write(mem_address,&TxBuf[2],TxLen);
      //delay_ms(AT24C128_DELAY);      
      AT24C128.Read(mem_address,&RxBuf[0],RxLen);      
      Test_Condition( !(memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_9_RX_9 = Pass"), STR("I2C_POLL_TX_9_RX_9 = Fail"));
      
      I2c_test_id = I2C_POLL_64_RX_64;
      break;
      
    case I2C_POLL_64_RX_64:     
      TxLen = 64;
      RxLen = 64;  
      
      AT24C128.Write(mem_address,&TxBuf[2],TxLen);
      //delay_ms(AT24C128_DELAY);      
      AT24C128.Read(mem_address,&RxBuf[0],RxLen);      
      Test_Condition( !(memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_9_RX_9 = Pass"), STR("I2C_POLL_TX_9_RX_9 = Fail"));
      
      I2c_test_id = I2C_POLL_200_RX_200;
      break;
      
      case I2C_POLL_200_RX_200:     
      TxLen = 200;
      RxLen = 200;  
      
      AT24C128.Write(mem_address,&TxBuf[2],TxLen);
      //delay_ms(AT24C128_DELAY);      
      AT24C128.Read(mem_address,&RxBuf[0],RxLen);      
      Test_Condition( !(memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_9_RX_9 = Pass"), STR("I2C_POLL_TX_9_RX_9 = Fail"));
      
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
    
    MemSet(RxBuf,0,sizeof(RxBuf));
    
    delay_ms(1000);
  }

}

void I2CCallback(i2c::Status_t Status)
{
  static uint8_t count;
  count++;
}



#if I2C_INTERRUPT_MODE_TESTS


#else // #endif // I2C_INTERRUPT_MODE_TESTS

void I2C_Slave_Tests(void){}

#endif // I2C_INTERRUPT_MODE_TESTS



//#endif //I2C_POLL