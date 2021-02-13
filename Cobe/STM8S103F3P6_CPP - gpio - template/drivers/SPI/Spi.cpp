/******************
** FILE: Spi.cpp
**
** DESCRIPTION:
**  Spi implementation
**
** CREATED: 8/11/2019, by Amit Chaudhary
******************/

#include "Spi.h"
#include "DigitalIn.h"
#include"DigitalInOut.h"

extern HAL::Spi* m_Spi_ISR;
HAL::Spi::Transaction_t           HAL::Spi::m_Transaction;
namespace HAL
{
  
  
  Spi::Spi() :
    m_SPIState(SPI_RESET)
      {        

      }    
    
    Spi::SpiStatus_t Spi::HwInit(bool Slave)
    {     
      
      m_Spi_ISR = this;
      
      ClockEnable();
      
      if(Slave)
      {
        PinsInitSlave();
        SPI_Init(SPI_FirstBit_MSB,SPI_BaudRatePrescaler_2,SPI_Mode_Slave, SPI_CPOL_Low,SPI_CPHA_1Edge,SPI_Direction_2Lines_FullDuplex, SPI_NSS_Soft); 
      }
      else
      {
        PinsInitMaster();
        SPI_Init(SPI_FirstBit_MSB,SPI_BaudRatePrescaler_16,SPI_Mode_Master, SPI_CPOL_Low,SPI_CPHA_1Edge,SPI_Direction_2Lines_FullDuplex, SPI_NSS_Soft); 
      }
      
      Enable();        
      m_SpiStatus = SPI_OK;
      m_Transaction.SPIState = SPI_READY;      
      return SPI_OK;            
    }         
    
    void Spi::PinsInitMaster()
    {        
      DigitalOut<B5,OUTPUT_OPEN_DRAIN_PUSH_PULL_FAST> Clk;
      DigitalOut<B6,OUTPUT_OPEN_DRAIN_PUSH_PULL_FAST> Mosi;
      DigitalIn<B7,INPUT_PULLUP> Miso;

      Clk.HwInit();
      Mosi.HwInit();
      Miso.HwInit();
    }
    
    void Spi::PinsInitSlave()
    {        
      DigitalIn<B4,INPUT_PULLUP> Cs;
      DigitalIn<B5,INPUT_PULLUP> Clk;
      DigitalIn<B6,INPUT_FLOATING> Mosi;
      DigitalOut<B7,OUTPUT_OPEN_DRAIN_PUSH_PULL_FAST> Miso;
      
      Cs.SetInputMode();
      Clk.SetInputMode();
      Mosi.SetInputMode();
      Miso.SetOutputMode();
    }    
    
    Spi::SpiStatus_t Spi::Tx(uint8_t data)
    {     
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(SPI,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;    
      
      SPI->DR = data;            
      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(SPI,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
      
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(SPI,SPI_TIMEOUT)) return SPI_BUSY_TIMEOUT;
      
      return SPI_OK;
    }
   
    Spi::SpiStatus_t Spi::TxPoll(uint8_t* TxBuf, uint16_t TxLen)
    {      
      if(TxBuf == 0)  return SPI_INVALID_PARAMS;  
      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(SPI,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT; 
      
      while(TxLen != 0)
      {        
        SPI->DR = *TxBuf++;
        TxLen--;
        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(SPI,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;   
      }
      
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(SPI,SPI_TIMEOUT)) return SPI_BUSY_TIMEOUT;
      
      return SPI_OK;
    }
    
    Spi::SpiStatus_t Spi::RxPoll(uint8_t* RxBuf, uint16_t RxLen)
    {      
      if(RxBuf == 0) return SPI_INVALID_PARAMS;
      
      LL_SPI_ClearFlag_OVR(SPI);
      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(SPI,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;      
      
      while(RxLen != 0)
      {
        SPI->DR = 0xFF;
        
        if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(SPI,SPI_TIMEOUT)) return SPI_RXNE_TIMEOUT; 
        
        *RxBuf++ = SPI->DR;        
        RxLen--;
      }
      
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(SPI,SPI_TIMEOUT)) return SPI_BUSY_TIMEOUT;
      
      return SPI_OK;
    }
    
    
    Spi::SpiStatus_t Spi::TxRxPoll(uint8_t* TxBuf, uint8_t* RxBuf, uint16_t Len)
    {      
      if((TxBuf == 0) || (RxBuf == 0)) return SPI_INVALID_PARAMS;        
      
      while(Len != 0)
      {        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(SPI,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
        
        SPI->DR = *TxBuf++;
        
        if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(SPI,SPI_TIMEOUT)) return SPI_RXNE_TIMEOUT; 
        
        *RxBuf++ = SPI->DR;        
        Len--;
      }

      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(SPI,SPI_TIMEOUT)) return SPI_BUSY_TIMEOUT;
      
      return SPI_OK;
    }
    
    Spi::SpiStatus_t Spi::XferPoll(Transaction_t const *pTransaction)
    {
      SpiStatus_t SpiStatus;
      
      if(pTransaction == 0) return SPI_INVALID_PARAMS;
      
      if(pTransaction->TxLen)
      {
        SpiStatus = TxPoll(pTransaction->TxBuf,pTransaction->TxLen);
      }
      
      if(pTransaction->RxLen)
      {
        SpiStatus =  RxPoll(pTransaction->RxBuf,pTransaction->RxLen);
      }
      return SpiStatus;
    }
    
    Spi::SpiStatus_t Spi::CheckAndLoadTxn(Transaction_t const *pTransaction)
    {
      if(m_SPIState != SPI_READY) return SPI_BUSY;
      
      if((pTransaction == 0) || ( (!pTransaction->TxBuf) && (!pTransaction->RxBuf) ))  return SPI_INVALID_PARAMS;   
      
      memcpy((void*)&m_Transaction,pTransaction,sizeof(Transaction_t));
      
      //SetBaudrate();
      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(SPI,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;  
      
      SPI_CS_LOW();
      
      return SPI_OK;      
    } 
    
    Spi::SpiStatus_t Spi::CheckAndLoadTxn(uint8_t* TxBuf, uint32_t TxLen,uint8_t* RxBuf, uint32_t RxLen,spiMasterCallback_t XferDoneCallback)
    {
      if(m_SPIState != SPI_READY) return SPI_BUSY;
      
      if( (TxBuf == 0) || (RxBuf == 0) || ((TxLen == 0) && (RxLen == 0)) ) return SPI_INVALID_PARAMS;
      
      m_Transaction.TxBuf = TxBuf;
      m_Transaction.TxLen = TxLen;
      m_Transaction.RxBuf = RxBuf;
      m_Transaction.RxLen = RxLen;
      m_Transaction.XferDoneCallback = XferDoneCallback; 
      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(SPI,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
      
      SPI_CS_LOW();
      
      return SPI_OK;      
    } 

    void Spi::Master_Tx_8bit_Isr()
    {
      if(m_Transaction.TxLen > 0)
      {    
        SPI_BYTE_OUT_8_BIT(SPI,m_Transaction);           
      }
      else
      {               
        TxnDoneHandler_INTR();            
      }
    }
    
    void Spi::Master_Rx_8bit_Isr()
    {
      if(m_Transaction.RxLen > 0)
      {    
        SPI_BYTE_IN_8_BIT(SPI,m_Transaction);  
        
        SPI->DR = 0xBA;
      }
      else
      {               
        TxnDoneHandler_INTR();            
      }
    } 
    
    Spi::SpiStatus_t Spi::XferIntr(Transaction_t const *pTransaction)
    {      
      m_SpiStatus = CheckAndLoadTxn(pTransaction);
      
      if(m_SpiStatus != SPI_OK) return m_SpiStatus;
      
      if(m_Transaction.TxLen)
      {        
        m_SPIState = SPI_MASTER_TX; 
        m_CurrentIsr = &Spi::Master_Tx_8bit_Isr;   
        
        SPI_BYTE_OUT_8_BIT(SPI,m_Transaction);            
      }
      else
      {
        m_SPIState = SPI_MASTER_RX; 
        
        m_CurrentIsr = &Spi::Master_Rx_8bit_Isr;   
        
        SPI->DR = 0xff;
        if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(SPI,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;                
      }
      LL_SPI_ClearFlag_OVR(SPI);
      EnableInterrupt(SPI_ICR_RXIE | SPI_ICR_ERRIE );  
      
      return m_SpiStatus; 
    }
    
    Spi::SpiStatus_t Spi::RxIntr(uint8_t* RxBuf, uint32_t RxLen,spiMasterCallback_t XferDoneCallback)
    {      
      SpiStatus_t SpiStatus = CheckAndLoadTxn(RxBuf,0,RxBuf,RxLen,XferDoneCallback);
      
      if(SpiStatus != SPI_OK) return SpiStatus;
      
      LL_SPI_ClearFlag_OVR(SPI);
      
      m_SPIState = SPI_MASTER_RX; 
      
      m_CurrentIsr = &Spi::Master_Rx_8bit_Isr;         
      
      SPI->DR = 0xff;
      if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(SPI,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
      
      EnableInterrupt(SPI_ICR_RXIE | SPI_ICR_ERRIE );
      
      return SPI_OK;        
    }
    
    void Spi::TxnDoneHandler_INTR()
    {     
      if(m_Transaction.RxLen > 0)
      {
        m_SPIState = SPI_MASTER_RX;
        m_CurrentIsr = &Spi::Master_Rx_8bit_Isr;   
        LL_SPI_ClearFlag_OVR(SPI);
        SPI->DR = 0xff; // clock out to receive data
        return;
      }
      
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(SPI,SPI_TIMEOUT))
      {
        m_SpiStatus = SPI_BUSY_TIMEOUT;
      }
      else
      {
        m_SpiStatus = SPI_OK;
      }
      
      SPI_CS_HIGH();
      
      if( m_Transaction.XferDoneCallback) 
        m_Transaction.XferDoneCallback(m_SpiStatus);
      
      m_SPIState = SPI_READY; 
      DisableInterrupt(SPI_ICR_TXIE | SPI_ICR_RXIE | SPI_ICR_ERRIE );
    }
    
    Spi::SpiStatus_t Spi::TxRxIntr(Transaction_t const * pTransaction)
    {      
      m_SpiStatus = CheckAndLoadTxn(pTransaction);
      
      if(m_SpiStatus != SPI_OK) return m_SpiStatus;
      
      LL_SPI_ClearFlag_OVR(SPI);   
      
      m_SPIState = SPI_MASTER_TXRX;  
             
      SPI_BYTE_OUT_8_BIT(SPI,m_Transaction); 
      
      EnableInterrupt(SPI_ICR_RXIE | SPI_ICR_ERRIE );
      
      return m_SpiStatus; 
    } 
    

    void Spi::ISR()
    {
      if( SPI_RXNE(SPI) )
      {
        (m_Spi_ISR->*m_CurrentIsr)();
      }
      else
      {
        if((SPI->SR & SPI_FLAG_OVR) == SPI_FLAG_OVR)
        {
          LL_SPI_ClearFlag_OVR(SPI);
        }
      }
    }
    
    void Spi::Slave_Tx_8bit_Isr()
    {
      if(m_Transaction.TxLen > 0)
      {    
        SPI_BYTE_OUT_8_BIT(SPI,m_Transaction);            
      }
      else
      {               
        DisableInterrupt(SPI_ICR_TXIE | SPI_ICR_RXIE | SPI_ICR_ERRIE );
        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(SPI,SPI_TIMEOUT))
        {
          m_SpiStatus =  SPI_TXE_TIMEOUT;
        }
        else
        {
          m_SpiStatus = SPI_SLAVE_TX_DONE;
        }        
        //LL_SPI_ClearFlag_OVR(SPI);
        if(m_Transaction.XferDoneCallback) m_Transaction.XferDoneCallback(m_SpiStatus);
      }
    }
    
    void Spi::Slave_Rx_8bit_Isr()
    {
      SPI_BYTE_IN_8_BIT(SPI,m_Transaction); 
      
      if(m_Transaction.RxLen == 0)
      {               
        DisableInterrupt(SPI_ICR_TXIE | SPI_ICR_RXIE | SPI_ICR_ERRIE );            
        if(m_Transaction.XferDoneCallback) m_Transaction.XferDoneCallback(SPI_SLAVE_RX_DONE);     
      }     
    }
    
    void Spi::SrartListeningTXIntr8bit(uint8_t* Buf, uint8_t Len)
    {      
      m_Transaction.TxBuf = Buf;
      m_Transaction.TxLen = Len; 
      m_Transaction.SPIState = SPI_SLAVE_TX;
      SPI_RESET_TX_BUF(m_Transaction);
      SPI_BYTE_OUT_8_BIT(SPI,m_Transaction);         
      EnableInterrupt(SPI_ICR_TXIE);
    }
    
    void Spi::SrartListeningRXIntr8bit(uint8_t* Buf, uint8_t Len)
    {     
      m_Transaction.RxBuf = Buf;
      m_Transaction.RxLen = Len;      
      m_Transaction.SPIState = SPI_SLAVE_RX;
      SPI_RESET_RX_BUF(m_Transaction);
      EnableInterrupt(SPI_ICR_RXIE);
    }
    
#if 0  

#if SPI_MASTER_Q && (SPI_MASTER_DMA || SPI_MASTER_INTR)
    
    Spi::SpiStatus_t Spi::Post(Transaction_t const *pTransaction, uint32_t Mode)
    {      
      if(m_SPIState == SPI_READY)
      {  
        if(Mode == 0)
        {
#if (SPI_MASTER_INTR == 1)
          return XferIntr(pTransaction); 
#else
          return SPI_INVALID_PARAMS; 
#endif          
        }
        else
        {
#if (SPI_MASTER_DMA == 1)
          return XferDMA(pTransaction);
#else
          return SPI_INVALID_PARAMS; 
#endif          
        }
      }
      else
      {      
        if( (pTransaction == 0) || ((pTransaction->TxBuf == 0) && (pTransaction->RxBuf == 0))  )
        {                       
          return SPI_INVALID_PARAMS;                
        }        
        if( !m_SPITxnQueue.Write(pTransaction) )
        {
          return SPI_TXN_POSTED;
        }
        else 
        {                  
          return SPI_TXN_QUEUE_ERROR;
        }           
      }
    }
#endif //SPI_MASTER_Q   
    
#if SPI_MASTER_INTR || SPI_MASTER_DMA || SPI_SLAVE_INTR  || SPI_SLAVE_DMA
    
    Spi::SpiStatus_t Spi::CheckAndLoadTxn(Transaction_t const *pTransaction)
    {
      if(m_SPIState != SPI_READY) return SPI_BUSY;
      
      if((pTransaction == 0) || ( (!pTransaction->TxBuf) && (!pTransaction->RxBuf) ))  return SPI_INVALID_PARAMS;   
      
      memcpy(&m_Transaction,pTransaction,sizeof(Transaction_t));
      
      SetBaudrate();
      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(SPI,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;  
      
      SPI_CS_LOW();
      
      return SPI_OK;      
    }    
    
    Spi::SpiStatus_t Spi::CheckAndLoadTxn(uint8_t* TxBuf, uint32_t TxLen,uint8_t* RxBuf, uint32_t RxLen, XferMode_t mode,Spi_Baudrate_t Spi_Baudrate, SPICallback_t XferDoneCallback,GpioOutput* pCS)
    {
      if(m_SPIState != SPI_READY) return SPI_BUSY;
      
      if( (TxBuf == 0) || (RxBuf == 0) || ((TxLen == 0) && (RxLen == 0)) ) return SPI_INVALID_PARAMS;
      
      SetBaudrate(Spi_Baudrate);
      
      m_Transaction.Mode = mode;
      m_Transaction.TxBuf = TxBuf;
      m_Transaction.TxLen = TxLen;
      m_Transaction.RxBuf = RxBuf;
      m_Transaction.RxLen = RxLen;
      m_Transaction.pChipselect = pCS;
      m_Transaction.XferDoneCallback = XferDoneCallback;     
      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(SPI,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
      
      SPI_CS_LOW();
      
      return SPI_OK;      
    }    
#endif    
    
#if SPI_MASTER_DMA    
    
    Spi::SpiStatus_t Spi::TxDMA(uint8_t* TxBuf, uint32_t TxLen, XferMode_t mode,Spi_Baudrate_t Spi_Baudrate, SPICallback_t XferDoneCallback,GpioOutput* pCS)
    {   
      SpiStatus_t SpiStatus = CheckAndLoadTxn(TxBuf,TxLen,TxBuf,0,mode,Spi_Baudrate,XferDoneCallback,pCS);
      
      if(SpiStatus != SPI_OK) return SpiStatus;
      
      m_SPIState = SPI_MASTER_TX_DMA;          
      
      LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen); 
      
      LL_SPI_EnableDMAReq_TX(SPI);
      
      return SPI_OK;        
    }
    
    Spi::SpiStatus_t Spi::RxDMA(uint8_t* RxBuf, uint32_t RxLen, XferMode_t mode,Spi_Baudrate_t Spi_Baudrate, SPICallback_t XferDoneCallback,GpioOutput* pCS)
    {      
      SpiStatus_t SpiStatus = CheckAndLoadTxn(RxBuf,0,RxBuf,RxLen,mode,Spi_Baudrate,XferDoneCallback,pCS);
      
      if(SpiStatus != SPI_OK) return SpiStatus;
      
      LL_SPI_ClearFlag_OVR(SPI);
      
      m_SPIState = SPI_MASTER_RX_DMA;        
      
      /* Load DMA Rx transaction*/    
      LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen); 
      
      LL_SPI_EnableDMAReq_RX(SPI);
      
      LoadTxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);   
      
      LL_SPI_EnableDMAReq_TX(SPI);
      
      return SPI_OK;        
    }
    
    Spi::SpiStatus_t Spi::TxRxDMA(Transaction_t const * pTransaction)
    {      
      
      m_SpiStatus = CheckAndLoadTxn(pTransaction);
      
      if(m_SpiStatus != SPI_OK) return m_SpiStatus;
      
      LL_SPI_ClearFlag_OVR(SPI);
      
      m_SPIState = SPI_MASTER_TXRX_DMA;       
      
      /* Load DMA Tx transaction*/  
      LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen);
      
      /* Load DMA Rx transaction*/    
      LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen); 
      
      LL_SPI_EnableDMAReq_TX(SPI);
      LL_SPI_EnableDMAReq_RX(SPI);
      
      return m_SpiStatus;        
    } 
    
    Spi::SpiStatus_t Spi::XferDMA(Transaction_t const * pTransaction)
    {      
      m_SpiStatus = CheckAndLoadTxn(pTransaction);
      
      if(m_SpiStatus != SPI_OK) return m_SpiStatus;
      
      if(pTransaction->TxLen > 0)
      {       
        LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen);            
        m_SPIState = SPI_MASTER_TX_DMA;
      }
      else
      {            
        LL_SPI_ClearFlag_OVR(SPI);
        
        /* Load Tx DMA for generating clock for Rx*/ 
        LoadTxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);
        
        /* Load DMA Rx transaction*/    
        LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen); 
        LL_SPI_EnableDMAReq_RX(SPI);
        
        m_SPIState = SPI_MASTER_RX_DMA;
      }    
      
      LL_SPI_EnableDMAReq_TX(SPI);
      
      return m_SpiStatus;        
    } 
    
    void Spi::TxnDoneHandler_DMA()
    {  
      SpiStatus_t SpiStatus;
      
      if( (m_SPIState == SPI_MASTER_TX_DMA) && (m_Transaction.RxLen > 0) )
      {
        /* Load Tx DMA for generating clock for Rx*/ 
        LoadTxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);
        
        /* Load DMA Rx transaction*/    
        LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);
        
        LL_SPI_ClearFlag_OVR(SPI);
        
        m_SPIState = SPI_MASTER_RX_DMA; 
        
        LL_SPI_EnableDMAReq_RX(SPI);
        LL_SPI_EnableDMAReq_TX(SPI);
      }
      else
      {       
        DisableInterrupt(SPI_ICR_TXIE | SPI_ICR_RXIE | SPI_ICR_ERRIE );    
        
        if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(SPI,SPI_TIMEOUT))           SpiStatus = SPI_TXE_TIMEOUT;          
        else if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(SPI,SPI_TIMEOUT))         SpiStatus = SPI_BUSY_TIMEOUT;
        else SpiStatus = SPI_OK;
        
        SPI_CS_HIGH();
        
        if( m_Transaction.XferDoneCallback) 
          m_Transaction.XferDoneCallback->CallbackFunction(SpiStatus);
        
#if SPI_MASTER_Q   
        // Check if any transaction pending in Txn Queue, if yes then load the next transaction
        Transaction_t const* _pCurrentTxn;
        if(m_SPITxnQueue.AvailableEnteries())
        {         
          SPI_CS_LOW();
          
          m_SPITxnQueue.Read(&_pCurrentTxn);
          memcpy(&m_Transaction,_pCurrentTxn,sizeof(Transaction_t));
          
          SetBaudrate();
          
          if(m_Transaction.TxLen)
          {          
            LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen);            
            m_SPIState = SPI_MASTER_TX_DMA;           
          }
          else
          {          
            LL_SPI_ClearFlag_OVR(SPI);
            
            /* Load Tx DMA for generating clock for Rx*/ 
            LoadTxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);
            
            /* Load DMA Rx transaction*/    
            LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen); 
            LL_SPI_EnableDMAReq_RX(SPI);
            
            m_SPIState = SPI_MASTER_RX_DMA;             
          }
          
          LL_SPI_EnableDMAReq_TX(SPI);           
          return;
        }       
#endif    // SPI_MASTER_Q    
        m_SPIState = SPI_READY;
      }                              
    }
    
   void Spi::SPI_DMA_MasterTx_Done_Handler()
    {		
      /* Transfer Complete Interrupt management ***********************************/
      if(dma1.GetFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TC_FLAG))
      {
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if(LL_DMA_GetMode(dma1.m_DMAx,m_Curent_DMA_Tx_Channel) == LL_DMA_MODE_CIRCULAR)
        {
         /* Disable all the Half Transfer interrupt */
          dma1.DisableInterrupt(m_Curent_DMA_Tx_Channel,DMA_CCR_HTIE);
          dma1.DisableInterrupt(m_Curent_DMA_Rx_Channel,DMA_CCR_HTIE);
        }        
        
        dma1.ClearFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TC_FLAG | HAL::DMA::DMA_HC_FLAG);
        dma1.ClearFlag(m_Curent_DMA_Rx_Channel,HAL::DMA::DMA_TC_FLAG | HAL::DMA::DMA_HC_FLAG);
        
        LL_SPI_DisableDMAReq_TX(SPI); 
        LL_SPI_DisableDMAReq_RX(SPI);      
        
        TxnDoneHandler_DMA();     
      }          
      /* Transfer Error Interrupt management **************************************/
      else if(dma1.GetFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TE_FLAG))
      {
        /* Disable all the DMA interrupt */
        m_SpiStatus = SPI_DMA_ERROR;   
        dma1.DisableInterrupt(m_Curent_DMA_Tx_Channel,DMA_CCR_HTIE | DMA_CCR_TCIE | DMA_CCR_TEIE);
        dma1.DisableInterrupt(m_Curent_DMA_Rx_Channel,DMA_CCR_HTIE | DMA_CCR_TCIE | DMA_CCR_TEIE);
      }      
    }        
    
#endif //SPI_MASTER_DMA

#if (SPI_MASTER_DMA == 1) || (SPI_SLAVE_DMA == 1)
    
    void Spi::LoadRxDmaChannel(uint8_t* Buf, uint32_t len)
    {
      dma1.Load(m_Curent_DMA_Rx_Channel, (uint32_t)&(SPI->DR),(uint32_t)Buf,len, LL_DMA_DIRECTION_PERIPH_TO_MEMORY, m_Transaction.Mode == SPI_8BIT ? LL_DMA_PDATAALIGN_BYTE : LL_DMA_PDATAALIGN_HALFWORD);
    }
    
    void Spi::LoadTxDmaChannel(uint8_t* Buf, uint32_t len)
    {
      dma1.Load(m_Curent_DMA_Tx_Channel, (uint32_t)&(SPI->DR),(uint32_t)Buf,len, LL_DMA_DIRECTION_MEMORY_TO_PERIPH, m_Transaction.Mode == SPI_8BIT ? LL_DMA_PDATAALIGN_BYTE : LL_DMA_PDATAALIGN_HALFWORD);
    }
    
#endif   
    
#if SPI_SLAVE_DMA  
    
    //SPI1_RX_DMA_CHANNEL = 2;
    void Spi::SPI_DMA_SlaveRx_Done_Handler()
    {		
      /* Transfer Complete Interrupt management ***********************************/
      if(dma1.GetFlag(m_Curent_DMA_Rx_Channel,HAL::DMA::DMA_TC_FLAG))
      {        
        /* Clear the half transfer complete flag */
        //LL_DMA_ClearFlag_TC2(dma1.m_DMAx);
        
        /* Clear the half transfer complete flag */
        //LL_DMA_ClearFlag_HT2(dma1.m_DMAx); 
        
        dma1.ClearFlag(m_Curent_DMA_Rx_Channel,HAL::DMA::DMA_TC_FLAG | HAL::DMA::DMA_HC_FLAG);  
        
        LL_SPI_DisableDMAReq_RX(SPI);        
        
         if( m_Transaction.XferDoneCallback) 
          m_Transaction.XferDoneCallback->CallbackFunction(SPI_SLAVE_RX_DMA_DONE);       
      }
      else if(dma1.GetFlag(m_Curent_DMA_Rx_Channel,HAL::DMA::DMA_TE_FLAG))
      {
        /* Disable all the DMA interrupt */
        m_SpiStatus = SPI_DMA_ERROR;  
        dma1.DisableInterrupt(m_Curent_DMA_Rx_Channel,DMA_CCR_HTIE | DMA_CCR_TCIE | DMA_CCR_TEIE);
      }
    }
    
     //SPI1_TX_DMA_CHANNEL = 3;
    void Spi::SPI_DMA_SlaveTx_Done_Handler()
    {		
      /* Transfer Complete Interrupt management ***********************************/
      if(dma1.GetFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TC_FLAG))
      {        
        /* Clear the half transfer complete flag */
        //LL_DMA_ClearFlag_TC3(dma1.m_DMAx);
        
        /* Clear the half transfer complete flag */
        //LL_DMA_ClearFlag_HT3(dma1.m_DMAx);
        
        dma1.ClearFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TC_FLAG | HAL::DMA::DMA_HC_FLAG);  
        
        LL_SPI_DisableDMAReq_TX(SPI);       
        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(SPI,SPI_TIMEOUT))
        {
          m_SpiStatus =  SPI_TXE_TIMEOUT;
        }
        else
        {
          m_SpiStatus = SPI_SLAVE_TX_DMA_DONE;
        } 
        
        if( m_Transaction.XferDoneCallback) 
          m_Transaction.XferDoneCallback->CallbackFunction(m_SpiStatus);
      }
      else if(dma1.GetFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TE_FLAG))
      {
        /* Disable all the DMA interrupt */
        m_SpiStatus = SPI_DMA_ERROR;  
        dma1.DisableInterrupt(m_Curent_DMA_Tx_Channel,DMA_CCR_HTIE | DMA_CCR_TCIE | DMA_CCR_TEIE);
      }
    }  
    
    void Spi::SrartListeningTXDma(uint8_t* Buf, uint16_t Len)
    {
     // LL_SPI_ClearFlag_OVR(SPI);
      m_SPIState = SPI_SLAVE_TX_DMA;
      LoadTxDmaChannel(Buf,Len); // oopps, 198 cycles 
      LL_SPI_EnableDMAReq_TX(SPI);
    }
    
    void Spi::SrartListeningRXDma(uint8_t* Buf, uint16_t Len)
    {
      LL_SPI_ClearFlag_OVR(SPI);
      m_SPIState = SPI_SLAVE_RX_DMA;
      LoadRxDmaChannel(Buf,Len);
      LL_SPI_EnableDMAReq_RX(SPI);
    } 
    
    void Spi::SPI_DMA_Rx_Callback::ISR( )
    {
      _this->SPI_DMA_SlaveRx_Done_Handler(); 
    } 

#endif //SPI_SLAVE_DMA
    
#if SPI_MASTER_DMA  || SPI_SLAVE_DMA   
    
    void Spi::SPI_DMA_Tx_Callback::ISR( )
    {      
#if SPI_MASTER_DMA && SPI_SLAVE_DMA 
      if((_this->m_SPIState == SPI_MASTER_TX_DMA) || (_this->m_SPIState == SPI_MASTER_RX_DMA) )
      {
        _this->SPI_DMA_MasterTx_Done_Handler();    
      }
      else
      {
        _this->SPI_DMA_SlaveTx_Done_Handler(); 
      }
#elif SPI_MASTER_DMA
      _this->SPI_DMA_MasterTx_Done_Handler();    
#elif SPI_SLAVE_DMA
      _this->SPI_DMA_SlaveTx_Done_Handler(); 
#endif
    }     
    
#endif // SPI_MASTER_DMA || SPI_SLAVE_DMA   
    
   
#endif // #if 0
 
    

    
}


