#ifndef I2C_MASTER_INTR_H
#define I2C_MASTER_INTR_H

#include "i2cPoll.h"

namespace HAL
{
  
  namespace i2c
  {
    template <i2c_t i2cx, Clk_t Clk>
      class MasterIntr : public Poll<i2cx, Clk>
      {
        
      protected:
        typedef Poll<i2cx, Clk> I2c;
        static MasterTxn_t m_MasterTxn;
        
      public:
        static Status_t HwInit(void);
        
        static Status_t Xfer(uint8_t SlaveAddress, uint8_t *TxBuf, uint8_t TxLen, uint8_t *RxBuf = 0, uint8_t RxLen = 0, uint8_t Stop = 1)
        {
          return I2c::XferPoll(SlaveAddress, TxBuf, TxLen, RxBuf, RxLen, Stop);
        }
        
        static void ScanBus(uint8_t *pFoundDevices, uint8_t size)
        {
          I2c::ScanBus(pFoundDevices,size);
        }
        
        static Status_t Xfer(MasterTxn_t *pTransaction); // 0x69           
        
        static void IRQ_Handler(void);
        
      private:
        static void TxnDoneHandler(uint8_t StopFlag); //0x62
        
        static void SB_Handler();
        
        static void ADDR_Handler();
        
        static void RXNE_Handler();
        
        static void TXE_Handler();
        
        static void BTF_Handler();
        
        static void ADD10_Handler();
        
        static void AF_Handler();
        
        static void STOPF_Handler();
      };
      
      template <i2c_t i2cx, Clk_t Clk>
        i2c::MasterTxn_t i2c::MasterIntr<i2cx, Clk>::m_MasterTxn;
      
      template <i2c_t i2cx, Clk_t Clk>
        i2c::Status_t i2c::MasterIntr<i2cx, Clk>::HwInit(void)
        {
          I2c::HwInit();
          
          return I2C_OK;
        }
      
      template <i2c_t i2cx, Clk_t Clk>
        i2c::Status_t i2c::MasterIntr<i2cx, Clk>::Xfer(MasterTxn_t *pTransaction)
        {
          if (pTransaction == 0)
          {
            return I2C_INVALID_PARAMS;
          }
          
          memcpy(&m_MasterTxn,pTransaction,sizeof(MasterTxn_t));
          
          if ((!m_MasterTxn.TxBuf) && (!m_MasterTxn.RxBuf))
            return I2C_INVALID_PARAMS;
          
          if ((m_MasterTxn.TxLen == 0) || (m_MasterTxn.RxLen == 0))
          {
            /* RepeatedStart is only valid for TX and Rx type transfer */
            m_MasterTxn.RepeatedStart = 0;
          }
          
          /* Wait while BUSY flag is set */
          if (WaitOnFlag(&I2Cx->SR3, I2C_SR3_BUSY, I2C_SR3_BUSY, I2C_TIMEOUT))
          {
            I2C_LOG_STATES(I2C_LOG_BUSY_TIMEOUT);
            return I2C_BUSY_TIMEOUT;
          }
          
          /* Disable Pos */
          I2c::DisablePOS();
          
          if (m_MasterTxn.TxLen)
          {
            I2c::m_State = I2C_MASTER_TX;
          }
          else
          {
            I2c::m_State = I2C_MASTER_RX;
          }
          
          I2c::Enable_EVT_BUF_ERR_Interrupt();
          
          /* Enable Acknowledge, Generate Start */
          I2Cx->CR2 |= I2C_CR2_START | I2C_CR2_ACK;
          
          return I2C_OK;
        }
      
      template <i2c_t i2cx, Clk_t Clk>
        void i2c::MasterIntr<i2cx, Clk>::IRQ_Handler(void)
        {
          uint8_t SR1 = I2C->SR1;
          uint8_t SR2 = I2C->SR2;
          
          if (SR1 & I2C_SR1_SB)
          {
            I2C_LOG_EVENTS(I2C_LOG_SB);
            if (I2c::m_State == I2C_MASTER_TX)
            {
              I2Cx->DR = m_MasterTxn.SlaveAddress & I2C_DIR_WRITE;
              I2C_LOG_STATES(I2C_LOG_SB_MASTER_TX);
            }
            else 
            {
              /* start listening RxNE and TxE interrupts */
              I2c::Enable_BUF_Interrupt();
              
#ifndef I2C_RX_METHOD_1
              if (m_MasterTxn.RxLen == 2U)
              {
                /* Enable Pos */
                I2c::EnablePOS();
              }
#endif
              I2Cx->DR = m_MasterTxn.SlaveAddress | I2C_DIR_READ;
              
              /* Repeated start is handled here, clear the flag*/
              m_MasterTxn.RepeatedStart = 0;
              
              I2c::m_State = I2C_MASTER_RX;
              
              I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX);
            }
          }
          else if (SR1 & I2C_SR1_ADDR)
          {
            I2C_LOG_EVENTS(I2C_LOG_ADDR);
            if (I2c::m_State == I2C_MASTER_RX)
            {
              if (m_MasterTxn.RxLen == 1U)
              {
                /* Clear ADDR flag */
                I2c::ClearADDR();
                
                /* Disable Acknowledge */
                I2c::DisableACK();
                
                /* Generate Stop */
                I2c::GenerateStop();
                
                I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_1);
              }
#ifndef I2C_RX_METHOD_1
              else if (m_MasterTxn.RxLen == 2U)
              {
                /* Clear ADDR flag */
                I2c::ClearADDR();
                
                /* Disable Acknowledge */
                I2c::DisableACK();
                
                I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_2);
              }
#endif
              else
              {
                /* Clear ADDR flag */
                I2c::ClearADDR();
                
                I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_OTHER);
              }
            }
            else if (I2c::m_State == I2C_MASTER_TX)
            {
              /* Clear ADDR flag */
              I2c::ClearADDR();
              
              if (m_MasterTxn.TxLen > 0)
              {
                I2C_BUF_BYTE_OUT(m_MasterTxn);
                
                I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_TX_SIZE_GT_0);
              }
              else
              {
                //while(1);
              }
            }
          }
          else if (SR1 & I2C_SR1_RXNE)
          {
            I2C_LOG_EVENTS(I2C_LOG_RXNE);
            if (I2c::m_State == I2C_MASTER_RX)
            {
#ifdef I2C_RX_METHOD_1
              if (m_MasterTxn.RxLen == 2U)
              {
                /* Read data from DR */
                I2C_BUF_BYTE_IN(m_MasterTxn);
                
                /* Disable Acknowledge */
                DisableACK();
                
                /* Generate Stop */
                GenerateStop();
                
                I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_2);
              }
              else if (m_MasterTxn.RxLen == 1U)
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
              if (m_MasterTxn.RxLen > 3U)
              {
                /* Read data from DR */
                I2C_BUF_BYTE_IN(m_MasterTxn);
              }
              else if ((m_MasterTxn.RxLen == 2U) || (m_MasterTxn.RxLen == 3U))
              {
                // Do nothing here, data 3 is here in data register.
                // Let the data 2 also accumulate in shift register in next BTF.
                // After that we read data 3 and data2 in the Master Rx BTF handler
                if (I2C->SR1 & I2C_SR1_BTF)
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
                I2c::Disable_EVT_BUF_ERR_Interrupt();
                
                I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_LAST);
                TxnDoneHandler(0);
              }
#endif
            }
          }
          else if (SR1 & I2C_SR1_BTF)
          {
            BTF_Handler();
          }
          else if (SR1 & I2C_SR1_TXE)
          {
            if (I2c::m_State == I2C_MASTER_RX_REPEATED_START)
            {
              return;
            }
            if (m_MasterTxn.TxLen > 0)
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
          else if (SR1 & I2C_SR1_STOPF)
          {
            I2C_LOG_EVENTS(I2C_LOG_STOPF);
            
            I2C_LOG_STATES(I2C_LOG_STOPF_FLAG);
            
            I2c::ClearSTOPF();
            
            I2c::m_State = I2C_READY;
          }
          else if (SR1 & I2C_SR1_ADD10)
          {
            //ADD10_Handler();
          }
          else if (SR2 & I2C_SR2_AF)
          {
            I2C_LOG_EVENTS(I2C_LOG_AF);
            I2C_LOG_STATES(I2C_LOG_ACK_FAIL);
            
            I2c::ClearAF();
            
            I2c::GenerateStop();
            
            I2c::m_State = I2C_READY;
          }
          else
          {
            // Fatal Error
          }
        }
      
      template <i2c_t i2cx, Clk_t Clk>
        void i2c::MasterIntr<i2cx, Clk>::TxnDoneHandler(uint8_t StopFlag)
        {
          if (m_MasterTxn.RxLen != 0)
          {
            // From here we don't expect buffer interrupts till SB,ADDR is handled
            I2c::Disable_BUF_Interrupt();
            
            // Rx is pending, generate start or repeated start
            if (m_MasterTxn.RepeatedStart)
            {
              /* Generate Start */
              I2c::GenerateStart();
              
              I2c::m_State = I2C_MASTER_RX_REPEATED_START;
              
              I2C_LOG_STATES(I2C_LOG_REPEATED_START);
            }
            else
            {
              /* Generate Stop */
              I2C->CR2 |= StopFlag; //I2C_CR1_STOP;
              
              if (WaitOnFlag(&I2C->CR2, I2C_CR2_STOP, I2C_CR2_STOP, I2C_TIMEOUT))
              {
                I2c::m_Status = I2C_STOP_TIMEOUT;
                I2C_LOG_STATES(I2C_LOG_STOP_TIMEOUT);
              }
              
              /* Generate Start */
              I2c::GenerateStart();
              
              I2c::m_State = I2C_MASTER_RX;
            }
            return;
          }
          else
          {
            // TxLen and RxLen is 0, Txn finished, Load next Txn if available
            /* Generate Stop */
            I2C->CR2 |= StopFlag;
            
            I2c::m_Status = I2C_XFER_DONE;
            
            // Transaction ended here, call the completion callback
            if (m_MasterTxn.XferDoneCallback)
              m_MasterTxn.XferDoneCallback(I2c::m_Status);
            
            if (WaitOnFlag(&I2C->CR2, I2C_CR2_STOP, I2C_CR2_STOP, I2C_TIMEOUT))
            {
              I2C_LOG_STATES(I2C_LOG_STOP_TIMEOUT);
              I2c::m_Status = I2C_STOP_TIMEOUT;
            }
            else
            {
              I2c::Disable_EVT_BUF_ERR_Interrupt();
              
              I2c::m_State = I2C_READY;
              
              /* Disable Acknowledge */
              I2c::DisableACK();
              
              I2C_LOG_STATES(I2C_LOG_TXN_DONE_ALL);
            }
          }
        }
      
      template <i2c_t i2cx, Clk_t Clk>
        void i2c::MasterIntr<i2cx, Clk>::SB_Handler()
        {
          I2C_LOG_EVENTS(I2C_LOG_SB);
          if (I2c::m_State == I2C_MASTER_TX)
          {
            I2Cx->DR = m_MasterTxn.SlaveAddress & I2C_DIR_WRITE;
            I2C_LOG_STATES(I2C_LOG_SB_MASTER_TX);
          }
          else if ((I2c::m_State == I2C_MASTER_RX) || (I2c::m_State == I2C_MASTER_RX_REPEATED_START))
          {
            /* start listening RxNE and TxE interrupts */
            I2c::Enable_BUF_Interrupt();
            
#ifndef I2C_RX_METHOD_1
            if (m_MasterTxn.RxLen == 2U)
            {
              /* Enable Pos */
              I2c::EnablePOS();
            }
#endif
            I2Cx->DR = m_MasterTxn.SlaveAddress | I2C_DIR_READ;
            
            /* Repeated start is handled here, clear the flag*/
            m_MasterTxn.RepeatedStart = 0;
            
            I2c::m_State = I2C_MASTER_RX;
            
            I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX);
          }
          else
          {
            while (1)
              ;
          }
        }
      
      template <i2c_t i2cx, Clk_t Clk>
        void i2c::MasterIntr<i2cx, Clk>::ADDR_Handler()
        {
          I2C_LOG_EVENTS(I2C_LOG_ADDR);
          if (I2c::m_State == I2C_MASTER_RX)
          {
            if (m_MasterTxn.RxLen == 1U)
            {
              /* Clear ADDR flag */
              I2c::ClearADDR();
              
              /* Disable Acknowledge */
              I2c::DisableACK();
              
              /* Generate Stop */
              I2c::GenerateStop();
              
              I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_1);
            }
#ifndef I2C_RX_METHOD_1
            else if (m_MasterTxn.RxLen == 2U)
            {
              /* Clear ADDR flag */
              I2c::ClearADDR();
              
              /* Disable Acknowledge */
              I2c::DisableACK();
              
              I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_2);
            }
#endif
            else
            {
              /* Clear ADDR flag */
              I2c::ClearADDR();
              
              I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_OTHER);
            }
          }
          else if (I2c::m_State == I2C_MASTER_TX)
          {
            /* Clear ADDR flag */
            I2c::ClearADDR();
            
            if (m_MasterTxn.TxLen > 0)
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
            while (1)
              ; /* Fatal Error*/
          }
        }
      
      template <i2c_t i2cx, Clk_t Clk>
        void i2c::MasterIntr<i2cx, Clk>::RXNE_Handler()
        {
          I2C_LOG_EVENTS(I2C_LOG_RXNE);
          if (I2c::m_State == I2C_MASTER_RX)
          {
#ifdef I2C_RX_METHOD_1
            if (m_MasterTxn.RxLen == 2U)
            {
              /* Read data from DR */
              I2C_BUF_BYTE_IN(m_MasterTxn);
              
              /* Disable Acknowledge */
              DisableACK();
              
              /* Generate Stop */
              GenerateStop();
              
              I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_2);
            }
            else if (m_MasterTxn.RxLen == 1U)
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
            if (m_MasterTxn.RxLen > 3U)
            {
              /* Read data from DR */
              I2C_BUF_BYTE_IN(m_MasterTxn);
            }
            else if ((m_MasterTxn.RxLen == 2U) || (m_MasterTxn.RxLen == 3U))
            {
              // Do nothing here, data 3 is here in data register.
              // Let the data 2 also accumulate in shift register in next BTF.
              // After that we read data 3 and data2 in the Master Rx BTF handler
              if (I2C->SR1 & I2C_SR1_BTF)
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
              I2c::Disable_EVT_BUF_ERR_Interrupt();
              
              I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_LAST);
              TxnDoneHandler(0);
            }
#endif
          }
          //            else if (I2c::m_State == I2C_SLAVE_RX)
          //            {
          //                if (m_SlaveTxn.RxBuf->Idx >= m_SlaveTxn.RxBuf->Len - 1)
          //                {
          //                    /* Dummy read/Write to clear the RXNE interrupt*/
          //                    I2C_DATA_REG = I2C_DATA_REG;
          //                    I2C_LOG_STATES(I2C_LOG_SLAVE_RX_DONE_WITH_DEFAULT_BYTE);
          //                }
          //                else
          //                {
          //                    I2C_SLAVE_BUF_BYTE_IN(m_SlaveTxn);
          //                    I2C_LOG_STATES(I2C_LOG_SLAVE_RX_BYTE_IN);
          //                }
          //            }
          else
          {
            //while(1);/* Fatal Error*/
          }
        }
      
      template <i2c_t i2cx, Clk_t Clk>
        void i2c::MasterIntr<i2cx, Clk>::TXE_Handler()
        {
          if (I2c::m_State == I2C_MASTER_RX_REPEATED_START)
          {
            return;
          }
          if (m_MasterTxn.TxLen > 0)
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
      
      template <i2c_t i2cx, Clk_t Clk>
        void i2c::MasterIntr<i2cx, Clk>::STOPF_Handler()
        {
          I2C_LOG_EVENTS(I2C_LOG_STOPF);
          
          I2C_LOG_STATES(I2C_LOG_STOPF_FLAG);
          
          I2c::ClearSTOPF();
          
          I2c::m_State = I2C_READY;
        }
      
      template <i2c_t i2cx, Clk_t Clk>
        void i2c::MasterIntr<i2cx, Clk>::AF_Handler()
        {
          I2C_LOG_EVENTS(I2C_LOG_AF);
          I2C_LOG_STATES(I2C_LOG_ACK_FAIL);
          
          I2c::ClearAF();
          
          I2c::GenerateStop();
          
          I2c::m_State = I2C_READY;
        }
      
      template <i2c_t i2cx, Clk_t Clk>
        void i2c::MasterIntr<i2cx, Clk>::BTF_Handler()
        {
          I2C_LOG_EVENTS(I2C_LOG_BTF);
          
          if (I2c::m_State == I2C_MASTER_RX_REPEATED_START)
          {
            return;
          }
          else if (I2c::m_State == I2C_MASTER_RX)
          {
            if (I2c::m_State == I2C_MASTER_RX_REPEATED_START)
              return;
            
            if (m_MasterTxn.RxLen == 3U)
            {
              /* Disable Acknowledge */
              I2c::DisableACK();
              
              // Read data3
              I2C_BUF_BYTE_IN(m_MasterTxn);
              
              /* Generate Stop */
              I2c::GenerateStop();
              
              // Read data2
              I2C_BUF_BYTE_IN(m_MasterTxn);
              
              I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_3);
            }
            else if (m_MasterTxn.RxLen == 2U)
            {
              /* Generate Stop */
              I2c::GenerateStop();
              
              // Read data2
              I2C_BUF_BYTE_IN(m_MasterTxn);
              
              // Read data1
              I2C_BUF_BYTE_IN(m_MasterTxn);
              
              I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_2_STOPED);
              
              if (WaitOnFlag(&I2C->CR2, I2C_CR2_STOP, I2C_CR2_STOP, I2C_TIMEOUT))
              {
                I2C_LOG_STATES(I2C_LOG_STOP_TIMEOUT);
                I2c::m_Status = I2C_STOP_TIMEOUT;
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
          else if (I2c::m_State == I2C_MASTER_TX)
          {
            if (m_MasterTxn.TxLen > 0)
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
            while (1)
              ; /* Fatal Error*/
          }
        }
      
  } // namespace i2c
  
} // namespace HAL

#endif // I2C_MASTER_INTR_H