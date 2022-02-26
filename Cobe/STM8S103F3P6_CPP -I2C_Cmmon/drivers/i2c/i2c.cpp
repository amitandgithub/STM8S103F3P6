/******************
** FILE: i2c.cpp
**
** DESCRIPTION:
**  I2C class implementation
**
** CREATED: 8/14/2019, by Amit Chaudhary
******************/
#include "i2c.h"

i2c* i2c::m_i2c_this;

i2c::I2CStatus_t i2c::HwInit()
{
  m_i2c_this = this;
  
  ClockEnable();
  
  I2C_Init(100000,0x08<<1,I2C_DutyCycle_2,I2C_Ack_Disable,I2C_AcknowledgedAddress_7bit); 
  
  PinsInit();
  
  //I2C_Cmd(ENABLE);
  Enable();
  
  return I2C_OK;
}


uint8_t i2c::WaitOnFlag(volatile uint8_t* reg, uint8_t bitmask, uint8_t status, uint16_t timeout) // 35(0x25)bytes
{
  while( ((*reg & bitmask) == status) && timeout-- );    
  return ((*reg & bitmask) == status);
}

/*  
SCL = C1 - 19
SDA = C0 - 18
*/
void i2c::PinsInit()
{
  DigitalOut<C0,OUTPUT_OPEN_DRAIN_FAST> Sda;
  DigitalOut<C1,OUTPUT_OPEN_DRAIN_FAST> Scl;
  Scl.HwInit();
  Sda.HwInit();
}

void i2c::ScanBus(uint8_t* pFoundDevices, uint8_t size)
{
  uint8_t slave, i=0;
  
  for(slave = 0; slave < 255; slave++)
  {
    if(SendSlaveAddress(slave & I2C_DIR_WRITE,0) == I2C_OK)
    {
      pFoundDevices[i++] = slave++;
      GenerateStop();
    }
    
    if(i == size) i=0;
  }
}

i2c::I2CStatus_t i2c::SendSlaveAddress(uint8_t SlaveAddress, uint8_t Repeatedstart)
{
  /* To avoid the Busy Timeout here */
  /* During repeated start don't check the busy flag, as it will always be busy */
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
      
      I2C_LOG_STATES(I2C_LOG_ACK_FAIL);
      
      return I2C_ACK_FAIL;
    }
    
    I2C_LOG_STATES(I2C_LOG_ADDR_TIMEOUT);
    return I2C_ADDR_TIMEOUT;             
  }
  I2C_LOG_EVENTS(I2C_LOG_ADDR);
  return I2C_OK;
}

i2c::I2CStatus_t i2c::TxPoll(uint8_t SlaveAddress,uint8_t* TxBuf, uint8_t TxLen) // 110 bytes
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

i2c::I2CStatus_t i2c::RxPoll(uint8_t SlaveAddress,uint8_t* RxBuf, uint8_t RxLen) // 445 bytes
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

i2c::I2CStatus_t  i2c::XferPoll(uint8_t SlaveAddress,uint8_t* TxBuf, uint8_t TxLen, uint8_t* RxBuf, uint8_t RxLen, uint8_t RepeatedStart) // 394 bytes -> Templates 311
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

#if I2C_INTERRUPT_MODE
i2c::I2CStatus_t i2c::XferIntr(MasterTxn_t* pTransaction) 
{
  if(pTransaction == 0)
  {
    return I2C_INVALID_PARAMS;
  }
  
  m_MasterTxn = pTransaction;
  
  if( (!m_MasterTxn->TxBuf) && (!m_MasterTxn->RxBuf) )
    return I2C_INVALID_PARAMS;
  
  if((m_MasterTxn->TxLen == 0) || (m_MasterTxn->RxLen == 0))
  {
    /* RepeatedStart is only valid for TX and Rx type transfer */
    m_MasterTxn->RepeatedStart = 0;
  }
  
  /* Wait while BUSY flag is set */
  if (WaitOnFlag(&I2C->SR3, I2C_SR3_BUSY, I2C_SR3_BUSY, I2C_TIMEOUT))
  {
    I2C_LOG_STATES(I2C_LOG_BUSY_TIMEOUT);
    return I2C_BUSY_TIMEOUT; 
  }
  
  /* Disable Pos */
  DisablePOS();
  
  if(m_MasterTxn->TxLen)
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

void i2c::I2C_ISR(void)
{
  uint8_t SR1 = I2C->SR1;
  uint8_t SR2 = I2C->SR2;
  
  if(SR1 & I2C_SR1_SB)
  {
    m_i2c_this->SB_Handler();
  }
  else if(SR1 & I2C_SR1_ADDR)
  {
    m_i2c_this->ADDR_Handler();
  }
  else if(SR1 & I2C_SR1_RXNE)
  {
    m_i2c_this->RXNE_Handler();
  }
  else if(SR1 & I2C_SR1_BTF) 
  {
    m_i2c_this->BTF_Handler();
  }
  else if(SR1 & I2C_SR1_TXE)
  {
    m_i2c_this->TXE_Handler();
  }
  else if(SR1 & I2C_SR1_STOPF)
  {
    m_i2c_this->STOPF_Handler();
  }   
  else if(SR1 & I2C_SR1_ADD10)
  {
    m_i2c_this->ADD10_Handler();
  }
  else if(SR2 & I2C_SR2_AF)
  {
    m_i2c_this->AF_Handler();
  }
  else
  {
    while(1);
  }
}

void i2c::TxnDoneHandler(uint8_t StopFlag)
{            
  if(m_MasterTxn->RxLen != 0)
  {              
    // From here we don't expect buffer interrupts till SB,ADDR is handled
    Disable_BUF_Interrupt();
    
    // Rx is pending, generate start or repeated start
    if(m_MasterTxn->RepeatedStart)
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
    
    m_I2CStatus = I2C_XFER_DONE;
    
    // Transaction ended here, call the completion callback
    if(m_MasterTxn->XferDoneCallback)
      m_MasterTxn->XferDoneCallback(m_I2CStatus);  
    
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
void i2c::SB_Handler()
{
  I2C_LOG_EVENTS(I2C_LOG_SB); 
  if( m_I2CState == I2C_MASTER_TX )
  {
    I2C_DATA_REG = m_MasterTxn->SlaveAddress & I2C_DIR_WRITE; 
    I2C_LOG_STATES(I2C_LOG_SB_MASTER_TX);
  }
  else if((m_I2CState == I2C_MASTER_RX) || (m_I2CState == I2C_MASTER_RX_REPEATED_START))
  {
    /* start listening RxNE and TxE interrupts */  
    Enable_BUF_Interrupt();
    
#ifndef I2C_RX_METHOD_1                
    if(m_MasterTxn->RxLen == 2U) 
    {
      /* Enable Pos */
      EnablePOS();
    }
#endif 
    I2C_DATA_REG = m_MasterTxn->SlaveAddress | I2C_DIR_READ;    
    
    /* Repeated start is handled here, clear the flag*/
    m_MasterTxn->RepeatedStart = 0;
    
    m_I2CState = I2C_MASTER_RX;
    
    I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX);
  }
  else
  {
    while(1);
  }						
}

#pragma inline = forced
void i2c::ADDR_Handler()
{
  I2C_LOG_EVENTS(I2C_LOG_ADDR); 
  if(m_I2CState == I2C_MASTER_RX)
  {
    if(m_MasterTxn->RxLen == 1U)   
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
    else if(m_MasterTxn->RxLen == 2U)   
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
    
    if(m_MasterTxn->TxLen > 0)
    {                    
      I2C_BUF_BYTE_OUT(m_MasterTxn);
	  
      I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_TX_SIZE_GT_0);
    }
  }
  else //if((m_I2CState == I2C_READY) || (m_I2CState == I2C_SLAVE_RX) || (m_I2CState == I2C_SLAVE_TX) )
  {
    // changing state to Slave Tx here
    /* Check the addressing mode*/
//    if( (I2C->SR3 & I2C_SR3_DUALF ) == RESET)
//    {
//      m_MasterTxn->SlaveAddress = (I2C->OARL & 0xFE); // Bit 1-7 are address
//    }
//    else
//    {
//      m_MasterTxn->SlaveAddress = (I2C->OARL & 0xFE); // Bit 1-7 are address
//    }            
    /* Transfer Direction requested by Master */
    if( (I2C->SR3 & I2C_SR3_TRA) == 0)
    {
      m_I2CState = I2C_SLAVE_RX;
      I2C_LOG_STATES(I2C_LOG_ADDR_SLAVE_RX_SELECTED);
    }
    else
    {
      m_I2CState = I2C_SLAVE_TX;
      I2C_LOG_STATES(I2C_LOG_ADDR_SLAVE_TX_SELECTED);
    }
  }
}

#pragma inline = forced
void i2c::ADD10_Handler()
{
  
}

#pragma inline = forced
void i2c::RXNE_Handler()
{
  I2C_LOG_EVENTS(I2C_LOG_RXNE);
  if(m_I2CState == I2C_MASTER_RX)
  {
#ifdef I2C_RX_METHOD_1
    if(m_MasterTxn->RxLen == 2U)
    {
      /* Read data from DR */
      I2C_BUF_BYTE_IN(m_MasterTxn);
      
      /* Disable Acknowledge */
      DisableACK();
      
      /* Generate Stop */
      GenerateStop();
      
      I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_2);
    }
    else if(m_MasterTxn->RxLen == 1U)
    {
      /* Read data from DR */
      I2C_BUF_BYTE_IN(m_MasterTxn);
      
      /* Disable EVT, BUF and ERR interrupt */
      Disable_EVT_BUF_ERR_Interrupt();

      I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_1);
      
      TxnDoneHandler(0);                
    }
    else
    {
      /* Read data from DR */
      I2C_BUF_BYTE_IN(m_MasterTxn);
      
      I2C_LOG_STATES(I2C_LOG_RXNE_MASTER);
    }
#else
    if(m_MasterTxn->RxLen > 3U)
    {
      /* Read data from DR */
      I2C_BUF_BYTE_IN(m_MasterTxn);                
    }
    else if( (m_MasterTxn->RxLen == 2U) || (m_MasterTxn->RxLen == 3U) )
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
      I2C_BUF_BYTE_IN(m_MasterTxn);
      
      /* Disable EVT, BUF and ERR interrupt */
      Disable_EVT_BUF_ERR_Interrupt();
      
      I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_LAST);
      TxnDoneHandler(0);                
    }        
#endif
  }
  else if(m_I2CState == I2C_SLAVE_RX)
  {
    if(m_SlaveTxn.RxBuf->Idx >= m_SlaveTxn.RxBuf->Len - 1)
    {     
      /* Dummy read/Write to clear the RXNE interrupt*/
      I2C_DATA_REG = I2C_DATA_REG;
      I2C_LOG_STATES(I2C_LOG_SLAVE_RX_DONE_WITH_DEFAULT_BYTE); 
    }
    else
    {
      I2C_SLAVE_BUF_BYTE_IN(m_SlaveTxn);
      I2C_LOG_STATES(I2C_LOG_SLAVE_RX_BYTE_IN); 
    }
  }
  else
  {
    //while(1);/* Fatal Error*/ 
  }
}

#pragma inline = forced
void i2c::TXE_Handler()
{
  
  if(m_I2CState == I2C_MASTER_TX)   
  {
    if( m_I2CState == I2C_MASTER_RX_REPEATED_START)
      return;
    
    if(m_MasterTxn->TxLen > 0)
    {
      I2C_BUF_BYTE_OUT(m_MasterTxn);
	  
      I2C_LOG_STATES(I2C_LOG_TXE_GT_0);
    } 
    else
    {
      I2C_LOG_STATES(I2C_LOG_TXE_DONE);
      TxnDoneHandler(I2C_CR2_STOP);
    }
  }
  else if(m_I2CState == I2C_SLAVE_TX )  
  {
    if(m_SlaveTxn.TxBuf->Idx < m_SlaveTxn.TxBuf->Len)
    {
      I2C_SLAVE_BUF_BYTE_OUT(m_SlaveTxn);
      
      I2C_LOG_EVENTS(I2C_LOG_TXE);      
      
      if(m_SlaveTxn.TxBuf->Idx >= m_SlaveTxn.TxBuf->Len)
      {
        //Disable_BUF_Interrupt();
        //m_SlaveTxn.TxBuf->Idx = 0;
       // m_I2CState = I2C_READY;
        
        /* Execute the Callback */
       // if(m_SlaveTxn.XferDoneCallback)
       //   m_SlaveTxn.XferDoneCallback(I2C_SLAVE_TX_DONE);
        
        I2C_LOG_STATES(I2C_LOG_TXE_DONE);
      }
    }
    else
    {
      I2C_LOG_STATES(I2C_LOG_TXE_DEFAULT_BYTE);
      I2C_DATA_REG = m_SlaveTxn.DefaultByte;  
    }    
  }
  else if(m_I2CState == I2C_MASTER_RX_REPEATED_START)
  {
    return;
  }
  else
  {
    //while(1);
  }
  
}


#pragma inline = forced
void i2c::STOPF_Handler()
{
  I2C_LOG_EVENTS(I2C_LOG_STOPF);
  
  I2C_LOG_STATES(I2C_LOG_STOPF_FLAG);
  
  ClearSTOPF();   
   
  if(m_I2CState == I2C_SLAVE_RX)
  {     
    //  EnableACK();
    /* Execute the RxDone Callback */
    if(m_SlaveTxn.XferDoneCallback)
       m_SlaveTxn.XferDoneCallback(I2C_SLAVE_RX_DONE);   
  }
  else if(m_I2CState == I2C_SLAVE_TX)
  {
    /* Execute the TxDone Callback */
    if(m_SlaveTxn.XferDoneCallback)
       m_SlaveTxn.XferDoneCallback(I2C_SLAVE_TX_DONE);
  }
  
  m_I2CState = I2C_READY;
} 

#pragma inline = forced
void i2c::AF_Handler()
{
  I2C_LOG_EVENTS(I2C_LOG_AF);
  I2C_LOG_STATES(I2C_LOG_ACK_FAIL);
  
  ClearAF(); 
 
  
  if((m_I2CState == I2C_MASTER_TX)) 
  {
    /* Generate Stop */
    GenerateStop();    
    // m_I2CStatus = I2C_ACK_FAIL;    
    return;
  }
  else if((m_I2CState == I2C_SLAVE_TX))
  {                
    // In Slave mode just execute the transaction done callback if registered    
    m_I2CState = I2C_READY;
    
    if(m_SlaveTxn.XferDoneCallback)
      m_SlaveTxn.XferDoneCallback(I2C_SLAVE_TX_DONE); 
    //while(1);
  }
  else if((m_I2CState == I2C_SLAVE_RX))
  {
    //m_I2CState = I2C_READY;
    
    //I2C_LOG_STATES(I2C_LOG_SLAVE_RX_DONE_WITH_NACK);
    /* Last byte received here, execute the Callback */
    if(m_SlaveTxn.XferDoneCallback)
      m_SlaveTxn.XferDoneCallback(I2C_SLAVE_RX_DONE);
    //while(1);
  }
  else                                
  {
    //while(1);/* Fatal Error*/
  }
  
   m_I2CState = I2C_READY;
}

#pragma inline = forced
void i2c::BTF_Handler()
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
    
    if(m_MasterTxn->RxLen == 3U)
    {
      /* Disable Acknowledge */
      DisableACK();
      
      // Read data3
      I2C_BUF_BYTE_IN(m_MasterTxn);
      
      /* Generate Stop */
      GenerateStop(); 
      
      // Read data2
      I2C_BUF_BYTE_IN(m_MasterTxn);               
      
      I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_3);
    }
    else if(m_MasterTxn->RxLen == 2U)
    {                 
      /* Generate Stop */
      GenerateStop();         
      
      // Read data2
      I2C_BUF_BYTE_IN(m_MasterTxn);
      
      // Read data1
      I2C_BUF_BYTE_IN(m_MasterTxn);
      
      I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_2_STOPED);
      
      if (WaitOnFlag(&I2C->CR2, I2C_CR2_STOP, I2C_CR2_STOP, I2C_TIMEOUT))
      {          
        I2C_LOG_STATES(I2C_LOG_STOP_TIMEOUT);
        m_I2CStatus = I2C_STOP_TIMEOUT;                
      } 
      TxnDoneHandler(0); 
    }
    else 
    {
      /* Read data from DR */
      I2C_BUF_BYTE_IN(m_MasterTxn);
	  
      I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_GT_3);
    }
  }
  else if(m_I2CState == I2C_MASTER_TX)
  {
    if(m_MasterTxn->TxLen > 0)
    {
      I2C_BUF_BYTE_OUT(m_MasterTxn);
	  
      I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_GT_0);
    }
    else
    {
      TxnDoneHandler(I2C_CR2_STOP);
    }
    
  }
  else if(m_I2CState == I2C_SLAVE_RX ) 
  {    
    if(m_SlaveTxn.RxBuf->Idx >= m_SlaveTxn.RxBuf->Len - 1)
    {     
      /* Dummy read/Write to clear the RXNE interrupt*/
      I2C_DATA_REG = I2C_DATA_REG;
      I2C_LOG_STATES(I2C_LOG_SLAVE_RX_DONE_WITH_DEFAULT_BYTE); 
    }
    else
    {
      I2C_SLAVE_BUF_BYTE_IN(m_SlaveTxn);
      I2C_LOG_STATES(I2C_LOG_SLAVE_RX_BYTE_IN); 
    }
  }
  else if(m_I2CState == I2C_SLAVE_TX )    
  {
    if(m_SlaveTxn.TxBuf->Idx != m_SlaveTxn.TxBuf->Len)
    {
      I2C_LOG_EVENTS(I2C_LOG_TXE);
      
      I2C_SLAVE_BUF_BYTE_OUT(m_SlaveTxn);
      
      if(m_SlaveTxn.TxBuf->Idx == m_SlaveTxn.TxBuf->Len)
      {
        //m_I2CState = I2C_READY;
        
        /* Execute the Callback */
        if(m_SlaveTxn.XferDoneCallback)
          m_SlaveTxn.XferDoneCallback(I2C_SLAVE_TX_DONE);
        
        I2C_LOG_STATES(I2C_LOG_TXE_DONE);
      }
    } 
    else
    {
      I2C_LOG_STATES(I2C_LOG_TXE_DEFAULT_BYTE);
      I2C_DATA_REG = m_SlaveTxn.DefaultByte;
    }
  }
  else
  {
    while(1);/* Fatal Error*/ 
  }
}


void i2c::I2C_ISR_Master(void)
{
  uint8_t SR1 = I2C->SR1;
  uint8_t SR2 = I2C->SR2;
  
  if(SR1 & I2C_SR1_SB)
  {
    m_i2c_this->SB_Handler();
  }
  else if(SR1 & I2C_SR1_ADDR)
  {
    m_i2c_this->ADDR_Handler_Master();
  }
  else if(SR1 & I2C_SR1_RXNE)
  {
    m_i2c_this->RXNE_Handler_Master();
  }
  else if(SR1 & I2C_SR1_BTF) 
  {
    m_i2c_this->BTF_Handler_Master();
  }
  else if(SR1 & I2C_SR1_TXE)
  {
    m_i2c_this->TXE_Handler_Master();
  }
  else if(SR1 & I2C_SR1_STOPF)
  {
    m_i2c_this->STOPF_Handler_Master();
  }   
  else if(SR1 & I2C_SR1_ADD10)
  {
    m_i2c_this->ADD10_Handler();
  }
  else if(SR2 & I2C_SR2_AF)
  {
    m_i2c_this->AF_Handler_Master();
  }
  else
  {
    while(1);
  }
}

#pragma inline = forced
void i2c::ADDR_Handler_Master()
{
  I2C_LOG_EVENTS(I2C_LOG_ADDR); 
  if(m_I2CState == I2C_MASTER_RX)
  {
    if(m_MasterTxn->RxLen == 1U)   
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
    else if(m_MasterTxn->RxLen == 2U)   
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
    
    if(m_MasterTxn->TxLen > 0)
    {                    
      I2C_BUF_BYTE_OUT(m_MasterTxn);
	  
      I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_TX_SIZE_GT_0);
    }
    else
    {
      //while(1);
    }
  }
  else
  {
    while(1);/* Fatal Error*/      
  }
}

#pragma inline = forced
void i2c::RXNE_Handler_Master()
{
  I2C_LOG_EVENTS(I2C_LOG_RXNE);
  if(m_I2CState == I2C_MASTER_RX)
  {
#ifdef I2C_RX_METHOD_1
    if(m_MasterTxn->RxLen == 2U)
    {
      /* Read data from DR */
      I2C_BUF_BYTE_IN(m_MasterTxn);
      
      /* Disable Acknowledge */
      DisableACK();
      
      /* Generate Stop */
      GenerateStop();
      
      I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_2);
    }
    else if(m_MasterTxn->RxLen == 1U)
    {
      /* Read data from DR */
      I2C_BUF_BYTE_IN(m_MasterTxn);
      
      /* Disable EVT, BUF and ERR interrupt */
      Disable_EVT_BUF_ERR_Interrupt();

      I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_1);
      
      TxnDoneHandler(0);                
    }
    else
    {
      /* Read data from DR */
      I2C_BUF_BYTE_IN(m_MasterTxn);
      
      I2C_LOG_STATES(I2C_LOG_RXNE_MASTER);
    }
#else
    if(m_MasterTxn->RxLen > 3U)
    {
      /* Read data from DR */
      I2C_BUF_BYTE_IN(m_MasterTxn);                
    }
    else if( (m_MasterTxn->RxLen == 2U) || (m_MasterTxn->RxLen == 3U) )
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
      I2C_BUF_BYTE_IN(m_MasterTxn);
      
      /* Disable EVT, BUF and ERR interrupt */
      Disable_EVT_BUF_ERR_Interrupt();
      
      I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_LAST);
      TxnDoneHandler(0);                
    }        
#endif
  }
  else if(m_I2CState == I2C_SLAVE_RX)
  {
    if(m_SlaveTxn.RxBuf->Idx >= m_SlaveTxn.RxBuf->Len - 1)
    {     
      /* Dummy read/Write to clear the RXNE interrupt*/
      I2C_DATA_REG = I2C_DATA_REG;
      I2C_LOG_STATES(I2C_LOG_SLAVE_RX_DONE_WITH_DEFAULT_BYTE); 
    }
    else
    {
      I2C_SLAVE_BUF_BYTE_IN(m_SlaveTxn);
      I2C_LOG_STATES(I2C_LOG_SLAVE_RX_BYTE_IN); 
    }
  }
  else
  {
    //while(1);/* Fatal Error*/ 
  }
}

#pragma inline = forced
void i2c::TXE_Handler_Master()
{  
  if(m_I2CState == I2C_MASTER_RX_REPEATED_START)
  {
    return;
  }
  if(m_MasterTxn->TxLen > 0)
  {
    I2C_BUF_BYTE_OUT(m_MasterTxn);
    
    I2C_LOG_STATES(I2C_LOG_TXE_GT_0);
  } 
  else
  {
    I2C_LOG_STATES(I2C_LOG_TXE_DONE);
    TxnDoneHandler(I2C_CR2_STOP);
  }
}  


#pragma inline = forced
void i2c::STOPF_Handler_Master()
{
  I2C_LOG_EVENTS(I2C_LOG_STOPF);
  
  I2C_LOG_STATES(I2C_LOG_STOPF_FLAG);
  
  ClearSTOPF();   
  
  m_I2CState = I2C_READY;
} 

#pragma inline = forced
void i2c::AF_Handler_Master()
{
  I2C_LOG_EVENTS(I2C_LOG_AF);
  I2C_LOG_STATES(I2C_LOG_ACK_FAIL);
  
  ClearAF(); 
  
  GenerateStop();
  
  m_I2CState = I2C_READY; 
}

#pragma inline = forced
void i2c::BTF_Handler_Master()
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
    
    if(m_MasterTxn->RxLen == 3U)
    {
      /* Disable Acknowledge */
      DisableACK();
      
      // Read data3
      I2C_BUF_BYTE_IN(m_MasterTxn);
      
      /* Generate Stop */
      GenerateStop(); 
      
      // Read data2
      I2C_BUF_BYTE_IN(m_MasterTxn);               
      
      I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_3);
    }
    else if(m_MasterTxn->RxLen == 2U)
    {                 
      /* Generate Stop */
      GenerateStop();         
      
      // Read data2
      I2C_BUF_BYTE_IN(m_MasterTxn);
      
      // Read data1
      I2C_BUF_BYTE_IN(m_MasterTxn);
      
      I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_2_STOPED);
      
      if (WaitOnFlag(&I2C->CR2, I2C_CR2_STOP, I2C_CR2_STOP, I2C_TIMEOUT))
      {          
        I2C_LOG_STATES(I2C_LOG_STOP_TIMEOUT);
        m_I2CStatus = I2C_STOP_TIMEOUT;                
      } 
      TxnDoneHandler(0); 
    }
    else 
    {
      /* Read data from DR */
      I2C_BUF_BYTE_IN(m_MasterTxn);
	  
      I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_GT_3);
    }
  }
  else if(m_I2CState == I2C_MASTER_TX)
  {
    if(m_MasterTxn->TxLen > 0)
    {
      I2C_BUF_BYTE_OUT(m_MasterTxn);
	  
      I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_GT_0);
    }
    else
    {
      TxnDoneHandler(I2C_CR2_STOP);
    }
    
  }
  else
  {
    while(1);/* Fatal Error*/ 
  }
}

void i2c::I2C_ISR_Slave(void)
{
  uint8_t SR1 = I2C->SR1;
  uint8_t SR2 = I2C->SR2;
  
  if(SR1 & I2C_SR1_ADDR)
  {
    m_i2c_this->ADDR_Handler_Slave();
  }
  else if(SR1 & I2C_SR1_RXNE)
  {
    m_i2c_this->RXNE_Handler_Slave();
  }
  else if(SR1 & I2C_SR1_BTF) 
  {
    m_i2c_this->BTF_Handler_Slave();
  }
  else if(SR1 & I2C_SR1_TXE)
  {
    m_i2c_this->TXE_Handler_Slave();
  }
  else if(SR1 & I2C_SR1_STOPF)
  {
    m_i2c_this->STOPF_Handler_Slave();
  }   
  else if(SR1 & I2C_SR1_ADD10)
  {
    m_i2c_this->ADD10_Handler();
  }
  else if(SR2 & I2C_SR2_AF)
  {
    m_i2c_this->AF_Handler_Slave();
  }
  else
  {
    while(1);
  }
}

#pragma inline = forced
void i2c::SB_Handler_Slave()
{
  I2C_LOG_EVENTS(I2C_LOG_SB);						
}

#pragma inline = forced
void i2c::ADDR_Handler_Slave()
{
   I2C_LOG_EVENTS(I2C_LOG_ADDR); 
     // changing state to Slave Tx here
    /* Check the addressing mode*/
//    if( (I2C->SR3 & I2C_SR3_DUALF ) == RESET)
//    {
//      m_MasterTxn->SlaveAddress = (I2C->OARL & 0xFE); // Bit 1-7 are address
//    }
//    else
//    {
//      m_MasterTxn->SlaveAddress = (I2C->OARL & 0xFE); // Bit 1-7 are address
//    }            
    /* Transfer Direction requested by Master */
    if( (I2C->SR3 & I2C_SR3_TRA) == 0)
    {
      m_I2CState = I2C_SLAVE_RX;
      I2C_LOG_STATES(I2C_LOG_ADDR_SLAVE_RX_SELECTED);
    }
    else
    {
      m_I2CState = I2C_SLAVE_TX;
      I2C_LOG_STATES(I2C_LOG_ADDR_SLAVE_TX_SELECTED);
    }
}

#pragma inline = forced
void i2c::RXNE_Handler_Slave()
{
    I2C_LOG_EVENTS(I2C_LOG_RXNE);

    if(m_SlaveTxn.RxBuf->Idx >= m_SlaveTxn.RxBuf->Len - 1)
    {     
      /* Dummy read/Write to clear the RXNE interrupt*/
      I2C_DATA_REG = I2C_DATA_REG;
      I2C_LOG_STATES(I2C_LOG_SLAVE_RX_DONE_WITH_DEFAULT_BYTE); 
    }
    else
    {
      I2C_SLAVE_BUF_BYTE_IN(m_SlaveTxn);
      I2C_LOG_STATES(I2C_LOG_SLAVE_RX_BYTE_IN); 
    }
}

#pragma inline = forced
void i2c::TXE_Handler_Slave()
{
    if(m_SlaveTxn.TxBuf->Idx < m_SlaveTxn.TxBuf->Len)
    {
      I2C_SLAVE_BUF_BYTE_OUT(m_SlaveTxn);
      
      I2C_LOG_EVENTS(I2C_LOG_TXE);      
      
      if(m_SlaveTxn.TxBuf->Idx >= m_SlaveTxn.TxBuf->Len)
      {        
        I2C_LOG_STATES(I2C_LOG_TXE_DONE);
      }
    }
    else
    {
      I2C_LOG_STATES(I2C_LOG_TXE_DEFAULT_BYTE);
      I2C_DATA_REG = m_SlaveTxn.DefaultByte;  
    }
}

#pragma inline = forced
void i2c::STOPF_Handler_Slave()
{
  I2C_LOG_EVENTS(I2C_LOG_STOPF);
  
  ClearSTOPF();   
  
    /* Execute the RxDone Callback */
    if(m_SlaveTxn.XferDoneCallback)
       m_SlaveTxn.XferDoneCallback(I2C_SLAVE_RX_DONE);   
  
  m_I2CState = I2C_READY;
} 

#pragma inline = forced
void i2c::AF_Handler_Slave()
{
  I2C_LOG_EVENTS(I2C_LOG_AF);
  I2C_LOG_STATES(I2C_LOG_ACK_FAIL);
  
  ClearAF(); 
  
  if((m_I2CState == I2C_SLAVE_TX))
  {                
    // In Slave mode just execute the transaction done callback if registered    
    //m_I2CState = I2C_READY;
    
    if(m_SlaveTxn.XferDoneCallback)
      m_SlaveTxn.XferDoneCallback(I2C_SLAVE_TX_DONE); 
  }  
  m_I2CState = I2C_READY;
}

#pragma inline = forced
void i2c::BTF_Handler_Slave()
{
  I2C_LOG_EVENTS(I2C_LOG_BTF);
  
  if(m_I2CState == I2C_SLAVE_RX ) 
  {    
    if(m_SlaveTxn.RxBuf->Idx >= m_SlaveTxn.RxBuf->Len - 1)
    {     
      /* Dummy read/Write to clear the RXNE interrupt*/
      I2C_DATA_REG = I2C_DATA_REG;
      I2C_LOG_STATES(I2C_LOG_SLAVE_RX_DONE_WITH_DEFAULT_BYTE); 
    }
    else
    {
      I2C_SLAVE_BUF_BYTE_IN(m_SlaveTxn);
      I2C_LOG_STATES(I2C_LOG_SLAVE_RX_BYTE_IN); 
    }
  }
  else if(m_I2CState == I2C_SLAVE_TX )    
  {
    if(m_SlaveTxn.TxBuf->Idx != m_SlaveTxn.TxBuf->Len)
    {
      I2C_LOG_EVENTS(I2C_LOG_TXE);
      
      I2C_SLAVE_BUF_BYTE_OUT(m_SlaveTxn);
      
      if(m_SlaveTxn.TxBuf->Idx == m_SlaveTxn.TxBuf->Len)
      {        
        /* Execute the Callback */
        if(m_SlaveTxn.XferDoneCallback)
          m_SlaveTxn.XferDoneCallback(I2C_SLAVE_TX_DONE);
        
        I2C_LOG_STATES(I2C_LOG_TXE_DONE);
      }
    } 
    else
    {
      I2C_LOG_STATES(I2C_LOG_TXE_DEFAULT_BYTE);
      I2C_DATA_REG = m_SlaveTxn.DefaultByte;
    }
  }
}


#if (I2C_SLAVE_ONLY == 1) 
i2c::I2CStatus_t i2c::SlaveStartListening(i2cBuf_t* TxBuf, i2cBuf_t* RxBuf )
{
  if( (TxBuf == 0) || (RxBuf == 0) || (RxBuf->Len == 0)  )
  {
    return I2C_INVALID_PARAMS;
  }  
  m_SlaveTxn.TxBuf = TxBuf;
  m_SlaveTxn.RxBuf = RxBuf;
  
  Enable_EVT_BUF_ERR_Interrupt();
  
  EnableACK();
     
  m_I2CState = I2C_READY;
  
  return I2C_OK; 
}
#elif (I2C_SLAVE_ONLY == 0) && (I2C_MASTER_ONLY == 0)
i2c::I2CStatus_t i2c::SlaveStartListening(i2cBuf_t* TxBuf, i2cBuf_t* RxBuf )
{
  if( (TxBuf == 0) || (RxBuf == 0) || (RxBuf->Len == 0)  )
  {
    return I2C_INVALID_PARAMS;
  }  
  m_SlaveTxn.TxBuf = TxBuf;
  m_SlaveTxn.RxBuf = RxBuf;
  
  Enable_EVT_BUF_ERR_Interrupt();
  
  EnableACK();
     
  m_I2CState = I2C_READY;
  
  return I2C_OK; 
}
#elif (I2C_MASTER_ONLY == 1)
i2c::I2CStatus_t i2c::SlaveStartListening(i2cBuf_t* TxBuf, i2cBuf_t* RxBuf )
{
  return I2C_OK; 
}
#endif //I2C_SLAVE_ONLY




#else // I2C_INTERRUPT_MODE

void i2c::I2C_ISR(void)
{
  
}

#endif // I2C_INTERRUPT_MODE



#if 0
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
            
            
            
            
            
            
            
            
            
            
            
            
            