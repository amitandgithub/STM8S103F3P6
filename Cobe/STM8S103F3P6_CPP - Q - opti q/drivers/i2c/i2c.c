/******************
** FILE: i2c.c
**
** DESCRIPTION:
**  I2c peripheral implementation
**
** CREATED: 7/29/2019, by Amit Chaudhary
******************/
#include"i2c.h"

static I2C_Slave_Context_t I2C_Slave_Context;

#if I2C_DEBUG 
I2CLogs_t I2CStates[I2C_LOG_STATES_SIZE];
#endif

DigitalOut<C0,OPEN_DRAIN_FAST> Sda;
DigitalOut<C1,OPEN_DRAIN_FAST> Scl;

void I2c_HwInit()
{   
  I2cClockEnable();
  //Clear_I2C_BUSY_Condition_NXP();
  I2C_Init(100000,0x08,I2C_DutyCycle_2,I2C_Ack_Disable,I2C_AcknowledgedAddress_7bit);  
  I2cPinsInit();
  
  I2C_Cmd(ENABLE);
}
void Busy_Check()
{
  if(I2C->SR3 & I2C_SR3_BUSY )//now we have to stop it
  {
    GPIOC->DDR |= 0x01;
    GPIOC->CR1 &= ~0x01; 
    GPIOC->DDR |= 0x02;
    GPIOC->CR1 &= ~0x02;
    
    uint8_t count = 0;
    while((I2C->SR3 & I2C_SR3_BUSY )&&(count < 100) )//first recovery level - clock the i2c
    {
      GPIOC->ODR   &= ~0x01;
      delay_ms(1);
      GPIOC->ODR   = 0x01;
      delay_ms(1);
      count++;
    }
    GPIOC->DDR |= 0x01;
    GPIOC->CR1 &= ~0x01; 
    GPIOC->DDR |= 0x02;
    GPIOC->CR1 &= ~0x02;
    
    GPIOC->DDR &= ~0x01;
    GPIOC->CR1 &= ~0x01; 
    GPIOC->DDR &= ~0x02;
    GPIOC->CR1 &= ~0x02;
  }
}

void I2cClockEnable()
{
  /* Enable the I2c Clock */
  CLK->PCKENR |= ((uint8_t)CLK_Peripheral_I2C);
}

void I2cClockDisable()
{
  /* Disable the I2c Clock */
  CLK->PCKENR &= (uint8_t)(~(uint8_t)CLK_Peripheral_I2C);
}
/*  
SCL = C1 - 19
SDA = C0 - 18
*/
void I2cPinsInit()
{
  //GpioSetOutputOpenDrainFast(C0);
    Scl.HwInit();
  //GpioSetOutputOpenDrainFast(C1); 
    Sda.HwInit();
}

void I2cScanBus(uint8_t* pFoundDevices, uint8_t size)
{
  uint8_t slave, i=0;
  
  for(slave = 0; slave < 255; slave++)
  {
    if(SendSlaveAddress(slave,0) == I2C_OK)
    {
      pFoundDevices[i++] = slave;
      // printf("Slave Dectected at Address = 0x%x \n",slave);
    }
    else
    {
      ClearAF();
    }
    GenerateStop();
    if(i == size) i=0;
  }
}

I2CStatus_t SendSlaveAddress(uint8_t SlaveAddress, uint8_t Repeatedstart)
{
  /* During repeated start don't check the busy flag, as it will always be busy*/
  if(!Repeatedstart)
  {
    /* Wait while BUSY flag is set */
    if (WaitOnFlag(&I2C->SR3, I2C_SR3_BUSY, I2C_SR3_BUSY, I2C_TIMEOUT))
    {
      I2C_LOG_STATES(I2C_LOG_BUSY_TIMEOUT);
      return I2C_BUSY_TIMEOUT; 
    }    
  }    
  /* Disable Pos */
  DisablePOS();
  
  /* Enable Acknowledge, Generate Start */
  I2C->CR2 |= I2C_CR2_START | I2C_CR2_ACK;
  
  /* Wait while SB flag is 0 */
  if(WaitOnFlag(&I2C->SR1, I2C_SR1_SB, 0, I2C_TIMEOUT))
  {         
    I2C_LOG_STATES(I2C_LOG_START_TIMEOUT); 
    return I2C_START_TIMEOUT;                 
  }
  
  I2C_LOG_EVENTS(I2C_LOG_START);
  
  I2C_DATA_REG = SlaveAddress;
  
  /* Wait while ADDR flag is 0 */
  if(WaitOnFlag(&I2C->SR1, I2C_SR1_ADDR, 0, I2C_TIMEOUT))
  {
    /* if there is ack fail, clear the AF flag */
    if(I2C->SR2 & I2C_SR2_AF)
    {
      ClearAF();
      
      /* Generate Stop */
      GenerateStop();
      
      return I2C_ACK_FAIL;
    }
    
    I2C_LOG_STATES(I2C_LOG_ADDR_TIMEOUT);
    return I2C_ADDR_TIMEOUT;             
  }
  I2C_LOG_EVENTS(I2C_LOG_ADDR);
  return I2C_OK;
}
I2CStatus_t I2cTxPoll(uint8_t SlaveAddress,uint8_t* TxBuf, uint8_t TxLen) // 110 bytes
{
  if(!TxBuf) return I2C_INVALID_PARAMS;
  
  /* Send Slave address */
  if(SendSlaveAddress(SlaveAddress & I2C_DIR_WRITE,0) != I2C_OK)
    return I2C_ADDR_TIMEOUT;
  
  /* Clear ADDR flag */
  ClearADDR();
  
  while(TxLen)
  {    
    /* Write data to DR */
    I2C_DATA_REG = *TxBuf++;                
    TxLen--; 
    
    /* Wait until TXE flag is set */
    if (WaitOnFlag(&I2C->SR1, I2C_SR1_TXE, 0, I2C_TIMEOUT))
    {          
      I2C_LOG_STATES(I2C_LOG_TXE_TIMEOUT);
      return I2C_TXE_TIMEOUT;                
    }  
    
    I2C_LOG_EVENTS(I2C_LOG_TXE);    
    
    if(I2C->SR1 & I2C_SR1_BTF)
    {
      I2C_LOG_EVENTS(I2C_LOG_BTF);                    
    }                               
  }             
  /* Generate Stop */
  GenerateStop();  
  
  I2C_LOG_EVENTS(I2C_LOG_STOP);
  
  return I2C_OK;  
}

I2CStatus_t I2cRxPoll(uint8_t SlaveAddress,uint8_t* RxBuf, uint8_t RxLen) // 445 bytes
{
  if(!RxBuf) return I2C_INVALID_PARAMS;
  
  /* Send Slave address */
  if(SendSlaveAddress(SlaveAddress | I2C_DIR_READ,0) != I2C_OK)
    return I2C_ADDR_TIMEOUT;
  
  if(RxLen == 1)
  {
    /* Disable Acknowledge */
    DisableACK();
    
    /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
    software sequence must complete before the current byte end of transfer */
    __disable_interrupt();
    
    /* Clear ADDR flag */
    ClearADDR();
    
    /* Generate Stop */
    GenerateStop();
    
    /* Re-enable IRQs */
    __enable_interrupt();   
    
    I2C_LOG_STATES(I2C_LOG_RX_1);
  }
  else if(RxLen == 2)
  {
    /* Enable Pos */
    EnablePOS();
    
    /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
    software sequence must complete before the current byte end of transfer */
    __disable_interrupt();
    
    /* Clear ADDR flag */
    ClearADDR();
    
    /* Disable Acknowledge */
    DisableACK();
    
    /* Re-enable IRQs */
    __enable_interrupt(); 
    
    I2C_LOG_STATES(I2C_LOG_RX_2);
  }
  else
  {
    /* Enable Acknowledge */
    EnableACK();
    
    /* Clear ADDR flag */
    ClearADDR();
    
    I2C_LOG_STATES(I2C_LOG_RX_GT_2);
  }
  
  while(RxLen > 0U)
  {
    if(RxLen <= 3U)
    {
      /* One byte */
      if(RxLen == 1U)
      {
        /* Wait until RXNE flag is set */
        if(WaitOnFlag(&I2C->SR1, I2C_SR1_RXNE, 0, I2C_TIMEOUT))
        {          
          I2C_LOG_STATES(I2C_LOG_RXNE_TIMEOUT);
          return I2C_RXNE_TIMEOUT;                
        } 
        
        I2C_LOG_EVENTS(I2C_LOG_RXNE); 
        
        /* Read data from DR */
        (*RxBuf) = I2C_DATA_REG;
        RxLen--;
        I2C_LOG_STATES(I2C_LOG_RX_1_DONE);
      }
      /* Two bytes */
      else if(RxLen == 2U)
      {
        /* Wait until BTF flag is set */
        if(WaitOnFlag(&I2C->SR1, I2C_SR1_BTF, 0, I2C_TIMEOUT))
        {         
          I2C_LOG_STATES(I2C_LOG_BTF_TIMEOUT);
          return I2C_BTF_TIMEOUT;                
        }
        
        /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
        software sequence must complete before the current byte end of transfer */
        __disable_interrupt();
        
        /* Generate Stop */
        GenerateStop();
        
        /* Read data from DR */
        (*RxBuf++)= I2C_DATA_REG;
        RxLen--;
        
        /* Re-enable IRQs */
        __enable_interrupt();
        
        /* Read data from DR */
        (*RxBuf++) = I2C_DATA_REG;
        RxLen--;
        
        I2C_LOG_STATES(I2C_LOG_RX_2_DONE);
      }
      /* 3 Last bytes */
      else
      {
        /* Wait until BTF flag is set */
        if(WaitOnFlag(&I2C->SR1, I2C_SR1_BTF, 0, I2C_TIMEOUT))
        {          
          I2C_LOG_STATES(I2C_LOG_BTF_TIMEOUT);
          return I2C_BTF_TIMEOUT;               
        } 
        
        /* Disable Acknowledge */
        DisableACK();
        
        /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
        software sequence must complete before the current byte end of transfer */
        __disable_interrupt();
        
        /* Read data from DR */
        (*RxBuf++) = I2C_DATA_REG;
        RxLen--;
        
        /* Wait until BTF flag is set */
        if(WaitOnFlag(&I2C->SR1, I2C_SR1_BTF, 0, I2C_TIMEOUT))
        {         
          I2C_LOG_STATES(I2C_LOG_BTF_TIMEOUT);
          return I2C_BTF_TIMEOUT;                
        }
        
        /* Generate Stop */
        GenerateStop();
        
        /* Read data from DR */
        (*RxBuf++) = I2C_DATA_REG;
        RxLen--;
        
        /* Re-enable IRQs */
        __enable_interrupt(); 
        
        /* Read data from DR */
        (*RxBuf++) = I2C_DATA_REG;
        RxLen--;
        
        I2C_LOG_STATES(I2C_LOG_RX_3_DONE);
      }
    }
    else
    {
      /* Wait until RXNE flag is set */
      if(WaitOnFlag(&I2C->SR1, I2C_SR1_RXNE, 0, I2C_TIMEOUT))
      {          
        I2C_LOG_STATES(I2C_LOG_RXNE_TIMEOUT);
        return I2C_RXNE_TIMEOUT;                
      } 
      
      I2C_LOG_EVENTS(I2C_LOG_RXNE);
      
      /* Read data from DR */
      (*RxBuf++) = I2C_DATA_REG;
      RxLen--;
      
      if(WaitOnFlag(&I2C->SR1, I2C_SR1_BTF, 0, I2C_TIMEOUT))
      {
        /* Read data from DR */
        (*RxBuf++) = I2C_DATA_REG;
        RxLen--;
        I2C_LOG_EVENTS(I2C_LOG_BTF);
      }
    }
  }       
  
  I2C_LOG_STATES(I2C_LOG_RX_DONE);
  return I2C_OK;
}

#if 0
I2CStatus_t I2cXferPoll(Transaction_t* pTransaction) // xxx bytes
{
  if(!TxBuf && !RxBuf)
    return I2C_INVALID_PARAMS;
  
  if((TxLen == 0) || (RxLen == 0))
  {
    /* RepeatedStart is only valid for TXRx type transfer */
    RepeatedStart = 0;
  }  
  
  if(TxLen != 0)
  {
    /* Send Slave address */
    if(SendSlaveAddress(SlaveAddress & I2C_DIR_WRITE,0) != I2C_OK)
      return I2C_ADDR_TIMEOUT;
    
    /* Clear ADDR flag */
    ClearADDR();
    
    while(TxLen)
    {    
      /* Write data to DR */
      I2C_DATA_REG = *TxBuf++;                
      TxLen--; 
      
      /* Wait until TXE flag is set */
      if (WaitOnFlag(&I2C->SR1, I2C_SR1_TXE, 0, I2C_TIMEOUT))
      {          
        I2C_LOG_STATES(I2C_LOG_TXE_TIMEOUT);
        return I2C_TXE_TIMEOUT;                
      }  
      
      I2C_LOG_EVENTS(I2C_LOG_TXE);    
      
      if(I2C->SR1 & I2C_SR1_BTF)
      {
        I2C_LOG_EVENTS(I2C_LOG_BTF);                    
      }                               
    }             
    /* Generate Stop */
    if(!RepeatedStart)
    {
      GenerateStop();
      
      I2C_LOG_EVENTS(I2C_LOG_STOP); 
      
      if (WaitOnFlag(&I2C->CR2, I2C_CR2_STOP, I2C_CR2_STOP, I2C_TIMEOUT))
      {          
        I2C_LOG_STATES(I2C_LOG_STOP_TIMEOUT);
        return I2C_STOP_TIMEOUT;                
      } 
    }
  }
  
  if(RxLen != 0)
  {
    /* Send Slave address */
    if(SendSlaveAddress(SlaveAddress | I2C_DIR_READ,RepeatedStart) != I2C_OK)
      return I2C_ADDR_TIMEOUT;
    
    if(RxLen == 1)
    {
      /* Disable Acknowledge */
      DisableACK();
      
      /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
      software sequence must complete before the current byte end of transfer */
      __disable_interrupt();
      
      /* Clear ADDR flag */
      ClearADDR();
      
      /* Generate Stop */
      GenerateStop();
      
      /* Re-enable IRQs */
      __enable_interrupt();   
      
      I2C_LOG_STATES(I2C_LOG_RX_1);
    }
    else if(RxLen == 2)
    {
      /* Enable Pos */
      EnablePOS();
      
      /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
      software sequence must complete before the current byte end of transfer */
      __disable_interrupt();
      
      /* Clear ADDR flag */
      ClearADDR();
      
      /* Disable Acknowledge */
      DisableACK();
      
      /* Re-enable IRQs */
      __enable_interrupt(); 
      
      I2C_LOG_STATES(I2C_LOG_RX_2);
    }
    else
    {
      /* Enable Acknowledge */
      EnableACK();
      
      /* Clear ADDR flag */
      ClearADDR();
      
      I2C_LOG_STATES(I2C_LOG_RX_GT_2);
    }
    
    while(RxLen > 0U)
    {
      if(RxLen <= 3U)
      {
        /* One byte */
        if(RxLen == 1U)
        {
          /* Wait until RXNE flag is set */
          if(WaitOnFlag(&I2C->SR1, I2C_SR1_RXNE, 0, I2C_TIMEOUT))
          {          
            I2C_LOG_STATES(I2C_LOG_RXNE_TIMEOUT);
            return I2C_RXNE_TIMEOUT;                
          } 
          
          I2C_LOG_EVENTS(I2C_LOG_RXNE); 
          
          /* Read data from DR */
          (*RxBuf) = I2C_DATA_REG;
          RxLen--;
          I2C_LOG_STATES(I2C_LOG_RX_1_DONE);
        }
        /* Two bytes */
        else if(RxLen == 2U)
        {
          /* Wait until BTF flag is set */
          if(WaitOnFlag(&I2C->SR1, I2C_SR1_BTF, 0, I2C_TIMEOUT))
          {         
            I2C_LOG_STATES(I2C_LOG_BTF_TIMEOUT);
            return I2C_BTF_TIMEOUT;                
          }
          
          /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
          software sequence must complete before the current byte end of transfer */
          __disable_interrupt();
          
          /* Generate Stop */
          GenerateStop();
          
          /* Read data from DR */
          (*RxBuf++)= I2C_DATA_REG;
          RxLen--;
          
          /* Re-enable IRQs */
          __enable_interrupt();
          
          /* Read data from DR */
          (*RxBuf++) = I2C_DATA_REG;
          RxLen--;
          
          I2C_LOG_STATES(I2C_LOG_RX_2_DONE);
        }
        /* 3 Last bytes */
        else
        {
          /* Wait until BTF flag is set */
          if(WaitOnFlag(&I2C->SR1, I2C_SR1_BTF, 0, I2C_TIMEOUT))
          {          
            I2C_LOG_STATES(I2C_LOG_BTF_TIMEOUT);
            return I2C_BTF_TIMEOUT;               
          } 
          
          /* Disable Acknowledge */
          DisableACK();
          
          /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
          software sequence must complete before the current byte end of transfer */
          __disable_interrupt();
          
          /* Read data from DR */
          (*RxBuf++) = I2C_DATA_REG;
          RxLen--;
          
          /* Wait until BTF flag is set */
          if(WaitOnFlag(&I2C->SR1, I2C_SR1_BTF, 0, I2C_TIMEOUT))
          {         
            I2C_LOG_STATES(I2C_LOG_BTF_TIMEOUT);
            return I2C_BTF_TIMEOUT;                
          }
          
          /* Generate Stop */
          GenerateStop();
          
          /* Read data from DR */
          (*RxBuf++) = I2C_DATA_REG;
          RxLen--;
          
          /* Re-enable IRQs */
          __enable_interrupt(); 
          
          /* Read data from DR */
          (*RxBuf++) = I2C_DATA_REG;
          RxLen--;
          
          I2C_LOG_STATES(I2C_LOG_RX_3_DONE);
        }
      }
      else
      {
        /* Wait until RXNE flag is set */
        if(WaitOnFlag(&I2C->SR1, I2C_SR1_RXNE, 0, I2C_TIMEOUT))
        {          
          I2C_LOG_STATES(I2C_LOG_RXNE_TIMEOUT);
          return I2C_RXNE_TIMEOUT;                
        } 
        
        I2C_LOG_EVENTS(I2C_LOG_RXNE);
        
        /* Read data from DR */
        (*RxBuf++) = I2C_DATA_REG;
        RxLen--;
        
        if(WaitOnFlag(&I2C->SR1, I2C_SR1_BTF, 0, I2C_TIMEOUT))
        {
          /* Read data from DR */
          (*RxBuf++) = I2C_DATA_REG;
          RxLen--;
          I2C_LOG_EVENTS(I2C_LOG_BTF);
        }
      }
    }       
    
    I2C_LOG_STATES(I2C_LOG_RX_DONE);
  }
  return I2C_OK;    
}
#endif
I2CStatus_t I2cXferPoll(uint8_t SlaveAddress,uint8_t* TxBuf, uint8_t TxLen, uint8_t* RxBuf, uint8_t RxLen, uint8_t RepeatedStart) // 462 bytes
{
  if(!TxBuf && !RxBuf)
    return I2C_INVALID_PARAMS;
  
  if((TxLen == 0) || (RxLen == 0))
  {
    /* RepeatedStart is only valid for TXRx type transfer */
    RepeatedStart = 0;
  }  
  
  if(TxLen != 0)
  {
    /* Send Slave address */
    if(SendSlaveAddress(SlaveAddress & I2C_DIR_WRITE,0) != I2C_OK)
      return I2C_ADDR_TIMEOUT;
    
    /* Clear ADDR flag */
    ClearADDR();
    
    while(TxLen)
    {    
      /* Write data to DR */
      I2C_DATA_REG = *TxBuf++;                
      TxLen--; 
      
      /* Wait until TXE flag is set */
      if (WaitOnFlag(&I2C->SR1, I2C_SR1_TXE, 0, I2C_TIMEOUT))
      {          
        I2C_LOG_STATES(I2C_LOG_TXE_TIMEOUT);
        return I2C_TXE_TIMEOUT;                
      }  
      
      I2C_LOG_EVENTS(I2C_LOG_TXE);    
      
      if(I2C->SR1 & I2C_SR1_BTF)
      {
        I2C_LOG_EVENTS(I2C_LOG_BTF);                    
      }                               
    }             
    /* Generate Stop */
    if(!RepeatedStart)
    {
      GenerateStop();
      
      I2C_LOG_EVENTS(I2C_LOG_STOP); 
      
      if (WaitOnFlag(&I2C->CR2, I2C_CR2_STOP, I2C_CR2_STOP, I2C_TIMEOUT))
      {          
        I2C_LOG_STATES(I2C_LOG_STOP_TIMEOUT);
        return I2C_STOP_TIMEOUT;                
      } 
    }
  }
  
  if(RxLen != 0)
  {
    /* Send Slave address */
    if(SendSlaveAddress(SlaveAddress | I2C_DIR_READ,RepeatedStart) != I2C_OK)
      return I2C_ADDR_TIMEOUT;
    
    if(RxLen == 1)
    {
      /* Disable Acknowledge */
      DisableACK();
      
      /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
      software sequence must complete before the current byte end of transfer */
      __disable_interrupt();
      
      /* Clear ADDR flag */
      ClearADDR();
      
      /* Generate Stop */
      GenerateStop();
      
      /* Re-enable IRQs */
      __enable_interrupt();   
      
      I2C_LOG_STATES(I2C_LOG_RX_1);
    }
    else if(RxLen == 2)
    {
      /* Enable Pos */
      EnablePOS();
      
      /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
      software sequence must complete before the current byte end of transfer */
      __disable_interrupt();
      
      /* Clear ADDR flag */
      ClearADDR();
      
      /* Disable Acknowledge */
      DisableACK();
      
      /* Re-enable IRQs */
      __enable_interrupt(); 
      
      I2C_LOG_STATES(I2C_LOG_RX_2);
    }
    else
    {
      /* Enable Acknowledge */
      EnableACK();
      
      /* Clear ADDR flag */
      ClearADDR();
      
      I2C_LOG_STATES(I2C_LOG_RX_GT_2);
    }
    
    while(RxLen > 0U)
    {
      if(RxLen <= 3U)
      {
        /* One byte */
        if(RxLen == 1U)
        {
          /* Wait until RXNE flag is set */
          if(WaitOnFlag(&I2C->SR1, I2C_SR1_RXNE, 0, I2C_TIMEOUT))
          {          
            I2C_LOG_STATES(I2C_LOG_RXNE_TIMEOUT);
            return I2C_RXNE_TIMEOUT;                
          } 
          
          I2C_LOG_EVENTS(I2C_LOG_RXNE); 
          
          /* Read data from DR */
          (*RxBuf) = I2C_DATA_REG;
          RxLen--;
          I2C_LOG_STATES(I2C_LOG_RX_1_DONE);
        }
        /* Two bytes */
        else if(RxLen == 2U)
        {
          /* Wait until BTF flag is set */
          if(WaitOnFlag(&I2C->SR1, I2C_SR1_BTF, 0, I2C_TIMEOUT))
          {         
            I2C_LOG_STATES(I2C_LOG_BTF_TIMEOUT);
            return I2C_BTF_TIMEOUT;                
          }
          
          /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
          software sequence must complete before the current byte end of transfer */
          __disable_interrupt();
          
          /* Generate Stop */
          GenerateStop();
          
          /* Read data from DR */
          (*RxBuf++)= I2C_DATA_REG;
          RxLen--;
          
          /* Re-enable IRQs */
          __enable_interrupt();
          
          /* Read data from DR */
          (*RxBuf++) = I2C_DATA_REG;
          RxLen--;
          
          I2C_LOG_STATES(I2C_LOG_RX_2_DONE);
        }
        /* 3 Last bytes */
        else
        {
          /* Wait until BTF flag is set */
          if(WaitOnFlag(&I2C->SR1, I2C_SR1_BTF, 0, I2C_TIMEOUT))
          {          
            I2C_LOG_STATES(I2C_LOG_BTF_TIMEOUT);
            return I2C_BTF_TIMEOUT;               
          } 
          
          /* Disable Acknowledge */
          DisableACK();
          
          /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
          software sequence must complete before the current byte end of transfer */
          __disable_interrupt();
          
          /* Read data from DR */
          (*RxBuf++) = I2C_DATA_REG;
          RxLen--;
          
          /* Wait until BTF flag is set */
          if(WaitOnFlag(&I2C->SR1, I2C_SR1_BTF, 0, I2C_TIMEOUT))
          {         
            I2C_LOG_STATES(I2C_LOG_BTF_TIMEOUT);
            return I2C_BTF_TIMEOUT;                
          }
          
          /* Generate Stop */
          GenerateStop();
          
          /* Read data from DR */
          (*RxBuf++) = I2C_DATA_REG;
          RxLen--;
          
          /* Re-enable IRQs */
          __enable_interrupt(); 
          
          /* Read data from DR */
          (*RxBuf++) = I2C_DATA_REG;
          RxLen--;
          
          I2C_LOG_STATES(I2C_LOG_RX_3_DONE);
        }
      }
      else
      {
        /* Wait until RXNE flag is set */
        if(WaitOnFlag(&I2C->SR1, I2C_SR1_RXNE, 0, I2C_TIMEOUT))
        {          
          I2C_LOG_STATES(I2C_LOG_RXNE_TIMEOUT);
          return I2C_RXNE_TIMEOUT;                
        } 
        
        I2C_LOG_EVENTS(I2C_LOG_RXNE);
        
        /* Read data from DR */
        (*RxBuf++) = I2C_DATA_REG;
        RxLen--;
        
        if(WaitOnFlag(&I2C->SR1, I2C_SR1_BTF, 0, I2C_TIMEOUT))
        {
          /* Read data from DR */
          (*RxBuf++) = I2C_DATA_REG;
          RxLen--;
          I2C_LOG_EVENTS(I2C_LOG_BTF);
        }
      }
    }       
    
    I2C_LOG_STATES(I2C_LOG_RX_DONE);
  }
  return I2C_OK;    
}

uint8_t WaitOnFlag(volatile uint8_t* reg, uint8_t bitmask, uint8_t status, uint16_t timeout) // 35(0x25)bytes
{
  while( ((*reg & bitmask) == status) && timeout-- );    
  return ((*reg & bitmask) == status);
}

static Transaction_t m_Transaction;

/* It must be volatile becoz it is shared between ISR and main loop */
volatile I2CState_t     m_I2CState; 

/* It must be volatile becoz it is shared between ISR and main loop */
volatile I2CStatus_t    m_I2CStatus;

I2CStatus_t I2cXferIntr(Transaction_t* pTransaction) 
{
  if(pTransaction == 0)
  {
    return I2C_INVALID_PARAMS;
  }
  
  m_Transaction.SlaveAddress       = pTransaction->SlaveAddress;
  m_Transaction.TxBuf              = pTransaction->TxBuf;
  m_Transaction.TxLen              = pTransaction->TxLen;
  m_Transaction.RxBuf              = pTransaction->RxBuf;
  m_Transaction.RxLen              = pTransaction->RxLen;  
  m_Transaction.RepeatedStart      = pTransaction->RepeatedStart;  
  m_Transaction.pStatus            = pTransaction->pStatus;// ? pStatus : &m_I2CStatus; 
  m_Transaction.XferDoneCallback   = pTransaction->XferDoneCallback;
  
  if(!m_Transaction.pStatus)
  {
    m_Transaction.pStatus = &m_I2CStatus;
  }
  
  if( (!m_Transaction.TxBuf) && (!m_Transaction.RxBuf) )
    return I2C_INVALID_PARAMS;
  
  if((m_Transaction.TxLen == 0) || (m_Transaction.RxLen == 0))
  {
    /* RepeatedStart is only valid for TX and Rx type transfer */
    m_Transaction.RepeatedStart = 0;
  }
  
  /* Wait while BUSY flag is set */
  if (WaitOnFlag(&I2C->SR3, I2C_SR3_BUSY, I2C_SR3_BUSY, I2C_TIMEOUT))
  {
    I2C_LOG_STATES(I2C_LOG_BUSY_TIMEOUT);
    return I2C_BUSY_TIMEOUT; 
  }
  
  /* Disable Pos */
  DisablePOS();
  
  if(m_Transaction.TxLen)
  {
    m_I2CState = I2C_MASTER_TX;
  }
  else
  {
    m_I2CState = I2C_MASTER_RX;
  }
  
  //Enable_EVT_BUF_ERR_Interrupt();
  Enable_EVT_BUF_ERR_Interrupt();
  
  /* Enable Acknowledge, Generate Start */
  I2C->CR2 |= I2C_CR2_START | I2C_CR2_ACK;  
  
  return I2C_OK;
}

uint16_t Err;
void I2C_ISR(void)
{
  uint8_t SR1 = I2C->SR1;
  
  if(SR1 & I2C_SR1_SB)
  {
    SB_Handler();
  }
  else if(SR1 & I2C_SR1_ADDR)
  {
    ADDR_Handler();
  }
  else if(SR1 & I2C_SR1_RXNE)
  {
    RXNE_Handler();
  }
  else if(SR1 & I2C_SR1_TXE)
  {
    TXE_Handler();
  }
  else if(SR1 & I2C_SR1_BTF) 
  {
    BTF_Handler();
  }
  else if(SR1 & I2C_SR1_STOPF)
  {
    STOPF_Handler();
  }   
  else if(SR1 & I2C_SR1_ADD10)
  {
    ADD10_Handler();
  }
  else
  {
    //while(1);
    Err++;
  }
}

void I2cTxnDoneHandler(uint8_t StopFlag)
{            
  if(m_Transaction.RxLen != 0)
  {              
    // From here we don't expect buffer interrupts till SB,ADDR is handled
    Disable_BUF_Interrupt();
    
    // Rx is pending, generate start or repeated start
    if(m_Transaction.RepeatedStart)
    {
      /* Generate Start */
      GenerateStart();
      
      m_I2CState = I2C_MASTER_RX_REPEATED_START;
      
      I2C_LOG_STATES(I2C_LOG_REPEATED_START);                     
    }
    else
    {
      /* Generate Stop */
      I2C->CR2 |= StopFlag;//I2C_CR1_STOP;
      
      if (WaitOnFlag(&I2C->CR2, I2C_CR2_STOP, I2C_CR2_STOP, I2C_TIMEOUT))
      {          
        m_I2CStatus = I2C_STOP_TIMEOUT;
        I2C_LOG_STATES(I2C_LOG_STOP_TIMEOUT);              
      } 
      
      /* Generate Start */
      GenerateStart();
      
      m_I2CState = I2C_MASTER_RX;
    }				 
    return;			
  }
  else 
  {                
    // TxLen and RxLen is 0, Txn finished, Load next Txn if available                
    /* Generate Stop */
    I2C->CR2 |= StopFlag;
    
    (*m_Transaction.pStatus) = I2C_XFER_DONE;
    
    // Transaction ended here, call the completion callback
    if(m_Transaction.XferDoneCallback)
      m_Transaction.XferDoneCallback();  
    
    if (WaitOnFlag(&I2C->CR2, I2C_CR2_STOP, I2C_CR2_STOP, I2C_TIMEOUT))
    {          
      I2C_LOG_STATES(I2C_LOG_STOP_TIMEOUT);
      m_I2CStatus = I2C_STOP_TIMEOUT;               
    } 
    else
    {
      Disable_EVT_BUF_ERR_Interrupt();
      
      m_I2CState = I2C_READY;
      
      /* Disable Acknowledge */
      DisableACK();              
      
      I2C_LOG_STATES(I2C_LOG_TXN_DONE_ALL);                    
    }
  }                         
}

#pragma inline = forced
void RXNE_Handler()
{
  if(m_I2CState == I2C_MASTER_RX)
  {
#ifdef I2C_RX_METHOD_1
    if(m_Transaction.RxLen == 2U)
    {
      /* Read data from DR */
      (*m_Transaction.RxBuf++) = I2C_DATA_REG;                
      m_Transaction.RxLen--;
      
      /* Disable Acknowledge */
      DisableACK();
      
      /* Generate Stop */
      GenerateStop();
      
      I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_2);
    }
    else if(m_Transaction.RxLen == 1U)
    {
      /* Read data from DR */
      (*m_Transaction.RxBuf++) = I2C_DATA_REG;                
      m_Transaction.RxLen--;
      
      /* Disable EVT, BUF and ERR interrupt */
      Disable_EVT_BUF_ERR_Interrupt();
      
      I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_1);
      
      I2cTxnDoneHandler(0);                
    }
    else
    {
      /* Read data from DR */
      (*m_Transaction.RxBuf++) = I2C_DATA_REG;                 
      m_Transaction.RxLen--;
      
      I2C_LOG_STATES(I2C_LOG_RXNE_MASTER);
    }
#else
    if(m_Transaction.RxLen > 3U)
    {
      /* Read data from DR */
      (*m_Transaction.RxBuf++) = I2C_DATA_REG;                
      m_Transaction.RxLen--;                
    }
    else if( (m_Transaction.RxLen == 2U) || (m_Transaction.RxLen == 3U) )
    {
      // Do nothing here, data 3 is here in data register.
      // Let the data 2 also accumulate in shift register in next BTF.
      // After that we read data 3 and data2 in the Master Rx BTF handler
      if(I2C->SR1 & I2C_SR1_BTF) 
      {
        BTF_Handler();
      }
      I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_2_OR_3);
    }
    else
    {      
      // This is for the case when Last byte/data1 is to be read
      /* Read data from DR */
      (*m_Transaction.RxBuf++) = I2C_DATA_REG;                
      m_Transaction.RxLen--;
      
      /* Disable EVT, BUF and ERR interrupt */
      Disable_EVT_BUF_ERR_Interrupt();
      
      I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_LAST);
      I2cTxnDoneHandler(0);                
    }        
#endif
  }
  else if(m_I2CState == I2C_SLAVE_RX)
  {
    /* Read the data from I2C data register and save it into the Rx Queue */
    I2C_Slave_Context.RxQueue.Write(I2C_DATA_REG);
  }
  else
  {
    while(1);/* Fatal Error*/ 
  }
}

void TXE_Handler()
{
  I2C_LOG_EVENTS(I2C_LOG_TXE);
  if(m_I2CState == I2C_MASTER_TX)   
  {
    if( m_I2CState == I2C_MASTER_RX_REPEATED_START)
      return;
    
    if(m_Transaction.TxLen > 0)
    {
      I2C_DATA_REG = (*m_Transaction.TxBuf++);
      m_Transaction.TxLen--;
      I2C_LOG_STATES(I2C_LOG_TXE_GT_0);
    } 
    else
    {
      I2C_LOG_STATES(I2C_LOG_TXE_DONE);
      I2cTxnDoneHandler(I2C_CR2_STOP);
    }
  }
  else if(m_I2CState == I2C_SLAVE_TX )  
  {
    /* Read the data from TxQueue and write in to the I2c data register for slave transmision */
    I2C_Slave_Context.TxQueue.Read((uint8_t*)&I2C_DATA_REG);
  }
  else if(m_I2CState == I2C_MASTER_RX_REPEATED_START)
  {
    return;
  }
  else
  {
    while(1);/* Fatal Error*/  
  }
  
}

void SB_Handler()
{
  I2C_LOG_EVENTS(I2C_LOG_SB); 
  if( m_I2CState == I2C_MASTER_TX )
  {
    I2C_DATA_REG = m_Transaction.SlaveAddress & I2C_DIR_WRITE; 
    I2C_LOG_STATES(I2C_LOG_SB_MASTER_TX);
  }
  else if((m_I2CState == I2C_MASTER_RX) || (m_I2CState == I2C_MASTER_RX_REPEATED_START))
  {
    /* start listening RxNE and TxE interrupts */  
   // if((m_I2CState == I2C_MASTER_RX))
      Enable_BUF_Interrupt();
    
#ifndef I2C_RX_METHOD_1                
    if(m_Transaction.RxLen == 2U) 
    {
      /* Enable Pos */
      EnablePOS();
    }
#endif 
    I2C_DATA_REG = m_Transaction.SlaveAddress | I2C_DIR_READ;    
    
    /* Repeated start is handled here, clear the flag*/
    m_Transaction.RepeatedStart = 0;
    
     m_I2CState = I2C_MASTER_RX;
    
    I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX);
  }
//  else if(m_I2CState == I2C_MASTER_RX_REPEATED_START)
//  {
//    /* Repeated start is handled here, clear the flag*/
//    m_Transaction.RepeatedStart = 0;         
//    
//#ifndef I2C_RX_METHOD_1                
//    if(m_Transaction.RxLen == 2U) 
//    {
//      /* Enable Pos */
//      EnablePOS();
//    }
//#endif          
//    I2C_DATA_REG = m_Transaction.SlaveAddress | I2C_DIR_READ;  
//    
//    /* start listening RxNE and TxE interrupts */                
//    Enable_BUF_Interrupt();
//    
//    m_I2CState = I2C_MASTER_RX;
//    
//    I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX_REPEATED_START);
//  }
  else
  {
    while(1);
  }						
}

void ADDR_Handler()
{
  I2C_LOG_EVENTS(I2C_LOG_ADDR); 
  if(m_I2CState == I2C_MASTER_RX)
  {
    if(m_Transaction.RxLen == 1U)   
    {
      /* Clear ADDR flag */
      ClearADDR();
      
      /* Disable Acknowledge */
      DisableACK();
      
      /* Generate Stop */
      GenerateStop();
      
      I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_1);                                       
    }   
#ifndef I2C_RX_METHOD_1 
    else if(m_Transaction.RxLen == 2U)   
    {
      /* Clear ADDR flag */
      ClearADDR();
      
      /* Disable Acknowledge */
      DisableACK();
      
      I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_2);                       
    }
#endif
    else
    {                    
      /* Clear ADDR flag */
      ClearADDR();
      
      I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_OTHER);
    }  
  }
  else if(m_I2CState == I2C_MASTER_TX)
  {                
    /* Clear ADDR flag */
    ClearADDR();
    
    if(m_Transaction.TxLen > 0)
    {                    
      I2C_DATA_REG = (*m_Transaction.TxBuf++);
      m_Transaction.TxLen--;
      I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_TX_SIZE_GT_0);
    }
    else
    {
      //while(1);
    }
  }
  else if((m_I2CState == I2C_READY) || (m_I2CState == I2C_SLAVE_RX) || (m_I2CState == I2C_SLAVE_TX) )
  {
    // changing state to Slave Tx here
    /* Check the addressing mode*/
    if( (I2C->SR3 & I2C_SR3_DUALF ) == RESET)
    {
      m_Transaction.SlaveAddress = (I2C->OARL & 0xFE); // Bit 1-7 are address
    }
    else
    {
      m_Transaction.SlaveAddress = (I2C->OARL & 0xFE); // Bit 1-7 are address
    }            
    /* Transfer Direction requested by Master */
    if( (I2C->SR2 & I2C_SR3_TRA) == 0)
    {
      m_I2CState = I2C_SLAVE_RX;
    }
    else
    {
      m_I2CState = I2C_SLAVE_TX;
    }
  }
  else
  {
    while(1);/* Fatal Error*/      
  }
}

void STOPF_Handler()
{
  I2C_LOG_EVENTS(I2C_LOG_STOPF);
  ClearSTOPF();  
  if(m_I2CState == I2C_SLAVE_RX)
  {
    /* Execute the RxDone Callback */
    //if(m_SlaveRxDoneCallback)
    // m_SlaveRxDoneCallback();  
    
    m_I2CState = I2C_READY;
    return;
  }               
  I2C_LOG_STATES(I2C_LOG_STOPF_FLAG);
}

void BTF_Handler()
{
  I2C_LOG_EVENTS(I2C_LOG_BTF);
  
  if(m_I2CState == I2C_MASTER_RX_REPEATED_START ) 
  {
    return;
  }
  else if(m_I2CState == I2C_MASTER_RX)
  {
    if( m_I2CState == I2C_MASTER_RX_REPEATED_START)
      return;            
    
    if(m_Transaction.RxLen == 3U)
    {
      /* Disable Acknowledge */
      DisableACK();
      
      // Read data3
      (*m_Transaction.RxBuf++) = I2C_DATA_REG;
      m_Transaction.RxLen--;
      
      /* Generate Stop */
      GenerateStop(); 
      
      // Read data2
      (*m_Transaction.RxBuf++) = I2C_DATA_REG;
      m_Transaction.RxLen--;                
      
      I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_3);
    }
    else if(m_Transaction.RxLen == 2U)
    {                 
      /* Generate Stop */
      GenerateStop();         
      
      // Read data2
      (*m_Transaction.RxBuf++) = I2C_DATA_REG;
      m_Transaction.RxLen--;
      
      // Read data1
      (*m_Transaction.RxBuf++) = I2C_DATA_REG;
      m_Transaction.RxLen--; 
      
      I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_2_STOPED);
      
      if (WaitOnFlag(&I2C->CR2, I2C_CR2_STOP, I2C_CR2_STOP, I2C_TIMEOUT))
      {          
        I2C_LOG_STATES(I2C_LOG_STOP_TIMEOUT);
        (*m_Transaction.pStatus) = I2C_STOP_TIMEOUT;                
      } 
      I2cTxnDoneHandler(0); 
    }
    else 
    {
      /* Read data from DR */
      (*m_Transaction.RxBuf++) = I2C_DATA_REG;
      m_Transaction.RxLen--;
      I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_GT_3);
    }
  }
  else if(m_I2CState == I2C_MASTER_TX)
  {
    if(m_Transaction.TxLen > 0)
    {
      I2C_DATA_REG = (*m_Transaction.TxBuf++);
      m_Transaction.TxLen--;
      I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_GT_0);
    }
    else
    {
      I2cTxnDoneHandler(I2C_CR2_STOP);
    }
    
  }
  else if(m_I2CState == I2C_SLAVE_RX ) 
  {
    
  }
  else if(m_I2CState == I2C_SLAVE_TX )    
  {
    
  }
  else
  {
    while(1);/* Fatal Error*/ 
  }
}

void ADD10_Handler()
{
  
}
void Clear_I2C_BUSY_Condition()
{
  uint8_t i;
  //  Set SCL as GPIO output. 
  //GpioSetOutputOpenDrainPushPull(C0);
  //  Set SDA as GPIO input. 
  //GpioSetOutputOpenDrainPushPull(C1);
  // Generate 12 clcok cycles at the same freq as is normally used  
  //  (toggle line high and low)  
  for(i=0; i<12; i++)  
  {
    //Gpio_On(C1);
    
    delay_100us();
    delay_100us();
    delay_50us();
    
    //Gpio_Off(C1);
    
    delay_100us();
    delay_100us();
    delay_50us();
  }
  
  //Gpio_On(C1);
  
}

void Clear_I2C_BUSY_Condition_NXP()
{
  uint8_t i;
  //  Set SCL as GPIO output. 
  //GpioSetInputPullup(C1);
  //  Set SDA as GPIO input. 
  //GpioSetInputPullup(C0);
  // Generate 12 clcok cycles at the same freq as is normally used  
  //  (toggle line high and low)  
  while((I2C->SR3 & I2C_SR3_BUSY ))
  {
    for(i=0; i<12; i++)  
    {
     // Gpio_On(C1);
      
      delay_100us();
      delay_100us();
      delay_50us();
      
     // Gpio_Off(C1);
      
      delay_100us();
      delay_100us();
      delay_50us();
    }
  } 
  
}

#if 0

// Experiencing I2C communications lockup when during the I2C communications,  
//  the processor experiences brownout  or is put into a low power state because of loss of power.  
//  Toggling the clock after initializing i2c to fix the problem.  

//  Set PTE24 (I2C0_SCL) as GPIO output.  
PORT_HAL_SetMuxMode(PORTE,24u,kPortMuxAsGpio);   
GPIO_HAL_SetPinDir(GPIOE, 24, kGpioDigitalOutput);  

// Set PTE25 (I2C0_SDA) as GPIO Input  
PORT_HAL_SetMuxMode(PORTE,25u,kPortMuxAsGpio);   
GPIO_HAL_SetPinDir(GPIOE, 25, kGpioDigitalInput);  

// Generate 12 clcok cycles at the same freq as is normally used  
//  (toggle line high and low)  
for(i=0; i<12; i++)  
{  
  GPIO_HAL_SetPinOutput(GPIOE, 24);  
  delay_usec(125);  // I2C is running at 4 kbit/s. 4 KHz freq ==> 250 usec.  
  GPIO_HAL_ClearPinOutput(GPIOE, 24);  
  delay_usec(125);   
}  

// Set the I2C0_SCL line high  
GPIO_HAL_SetPinOutput(GPIOE, 24); 


The solution is as follows:
  
  - rather than configure the I2C pins when the I2C interface is initialised they are left as inputs with pull-ups enabled
    
    - the first time that the I2C bus is to be used for transmission (transmission takes place as first operation of any read or write) the bus state is checked.
      
      - if it is detected in the busy state, the SCL pin is set to an output and clocks generated until the busy state no longer exists (the clocks remove the slave from its present ACK state - several clocks may be required)
        
        - the pins are then set to their final I2C peripheral usage
          
          - once the pins are set no further checking is required for subsequent I2C use and normal interrupt or DMA driven modes are possible
            
#endif
            