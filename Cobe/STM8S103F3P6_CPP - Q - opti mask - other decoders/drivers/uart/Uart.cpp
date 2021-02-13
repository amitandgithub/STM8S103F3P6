/******************
** FILE: Uart.cpp
**
** DESCRIPTION:
**  Uart implementation
**
** CREATED: 8/11/2019, by Amit Chaudhary
******************/

#include "Uart.h"
#include "DigitalIn.h"

namespace HAL
{
  
  Uart::Uart(UARTx_t UARTx) : m_UARTx(UARTx)
#if UART_INTR || UART_DMA 
  ,m_Transaction{0,nullptr,nullptr}
#endif
#if UART_DMA
  ,m_UART_DMA_Tx_Callback(this),
  m_UART_DMA_Rx_Callback(this)
#endif
  {
    
  }
  
  void Uart::PinsHwInit()
  {
    //DigitalIO::SetInputMode(RxPort,RxPin,INPUT_PULLUP); //Rx.HwInit(INPUT_PULLDOWN);
    //DigitalIO::SetOutputMode(TxPort,TxPin,OUTPUT_AF_PUSH_PULL,LL_GPIO_SPEED_FREQ_HIGH); // Tx.HwInit(OUTPUT_AF_PUSH_PULL);
    
      DigitalIn<C2,INPUT_PULLUP> Rx;
      DigitalOut<C3,OUTPUT_OPEN_DRAIN_PUSH_PULL_FAST> Tx;
      
      Rx.HwInit();
      Tx.HwInit();
  }
  
  Uart::UartStatus_t Uart::HwInit(uint32_t Baudrate, uint8_t Stop, uint8_t Parity)
  {   
   
    PinsHwInit();
    ClockEnable();      
    Disable();
    USART_Init(Baudrate,USART_WordLength_8D,(USART_StopBits_TypeDef)Stop,(USART_Parity_TypeDef)Parity, (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx) );
    Enable();
    
#if UART_DMA     
    dma1.XferConfig(&DMAConfig, m_Curent_DMA_Tx_Channel);
    dma1.XferConfig(&DMAConfig, m_Curent_DMA_Rx_Channel); 
    dma1.EnableTransferCompleteInterrupt(m_Curent_DMA_Tx_Channel);
    dma1.EnableTransferCompleteInterrupt(m_Curent_DMA_Rx_Channel);
#endif 
    
    m_UARTStatus = UART_OK;
    m_UARTState = UART_READY;    
    return UART_OK;     
  }
  
  void Uart::SetBaudrate(uint32_t Baudrate)
  {
 
  }
  
  Uart::UartStatus_t Uart::TxPoll(uint8_t data)
  {
    if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) return UART_TXE_TIMEOUT; 
    
    m_UARTx->DR = data;
    
    if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) return UART_TXE_TIMEOUT;
    
    if(UART_WAIT_FOR_TC_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) return UART_TC_TIMEOUT;
    
    return UART_OK;     
  }
  
  Uart::UartStatus_t Uart::TxPoll(uint8_t* TxBuf, uint16_t TxLen)
  {
    if( (TxBuf == nullptr) || (TxLen == 0) ) return UART_INVALID_PARAMS;
    
    if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) return UART_TXE_TIMEOUT; 
    
    while(TxLen != 0)
    {        
      m_UARTx->DR = *TxBuf++;
      TxLen--;
      
      if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) return UART_TXE_TIMEOUT; 
    }
    
    if(UART_WAIT_FOR_TC_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) return UART_TC_TIMEOUT;
    return UART_OK;     
  }
  
  Uart::UartStatus_t Uart::RxPoll(uint8_t* RxBuf, uint16_t RxLen)
  {    
    if((RxBuf == nullptr) || (RxLen == 0) ) return UART_INVALID_PARAMS;
    
    while(RxLen != 0)
    {        
      *RxBuf++ = m_UARTx->DR;
      RxLen--;      
      if(UART_WAIT_FOR_RXNE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) return UART_RXNE_TIMEOUT; 
    }    
    return UART_OK;     
  }
  
  void Uart::TxHex8(uint16_t num)
  {
    TxPoll(HEX_CHARS[(num >> 4)   % 0x10]);
    TxPoll(HEX_CHARS[(num & 0x0f) % 0x10]);
  }
  
  void Uart::TxHex16(uint16_t num)
  {
    uint8_t i;
    for (i = 12; i > 0; i -= 4) TxPoll(HEX_CHARS[(num >> i) % 0x10]);
    TxPoll(HEX_CHARS[(num & 0x0f) % 0x10]);
  }
  
  void Uart::TxHex32(uint32_t num)
  {
    uint8_t i;
    for (i = 28; i > 0; i -= 4)	TxPoll(HEX_CHARS[(num >> i) % 0x10]);
    TxPoll(HEX_CHARS[(num & 0x0f) % 0x10]);
  }

#if UART_INTR 
  
  Uart::UartStatus_t Uart::TxIntr(uint8_t* TxBuf, uint16_t TxLen,UARTCallback_t* pUARTCallback)
  {
    if( (TxBuf == nullptr) || (TxLen == 0) ) return UART_INVALID_PARAMS;
    
    m_UARTState = UART_TX_BUSY;
    m_Transaction.Len = TxLen;
    m_Transaction.Buf = TxBuf;
    m_Transaction.XferDoneCallback = pUARTCallback; 
    EnableInterrupt(IT_TXE | IT_PE | IT_ERR);
    return UART_OK;
  }
  
  Uart::UartStatus_t Uart::RxIntr(uint8_t* RxBuf, uint16_t RxLen,UARTCallback_t* pUARTCallback)
  {
    if( (RxBuf == nullptr) || (RxLen == 0) ) return UART_INVALID_PARAMS;
    
    m_UARTState = UART_TX_BUSY;
    m_Transaction.Len = RxLen;
    m_Transaction.Buf = RxBuf;
    m_Transaction.XferDoneCallback = pUARTCallback; 
    EnableInterrupt(IT_RXNE | IT_PE | IT_ERR);
    return UART_OK;
  }
#endif
   
#if UART_INTR 
  
  void Uart::ISR()
  {
    if(UART_RXNE(m_UARTx))
    {
      UART_BYTE_IN(m_Transaction);
      
      if(m_Transaction.Len == 0)
      {
        DisableInterrupt(IT_TXE | IT_RXNE | IT_PE | IT_ERR );   
        
        if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) m_UARTStatus = UART_TXE_TIMEOUT; 
        if(UART_WAIT_FOR_TC_FLAG_TO_SET(m_UARTx,UART_TIMEOUT))  m_UARTStatus = UART_TC_TIMEOUT;    
        
        if(m_Transaction.XferDoneCallback) m_Transaction.XferDoneCallback->CallbackFunction(m_UARTStatus);   
        
        m_UARTState = UART_READY;
      }
    }
    else if(UART_TXE(m_UARTx))
    {
      if(m_Transaction.Len > 0)
      {
        UART_BYTE_OUT(m_Transaction);
      }
      else
      {
        DisableInterrupt(IT_TXE | IT_RXNE | IT_PE | IT_ERR );   
        
        if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) m_UARTStatus = UART_TXE_TIMEOUT;   
        
        if(m_Transaction.XferDoneCallback) m_Transaction.XferDoneCallback->CallbackFunction(m_UARTStatus);       
        
        m_UARTState = UART_READY;
      }
    }
//    else if(UART_TC(m_UARTx))
//    {
//      DisableInterrupt(IT_TXE | IT_RXNE | IT_PE | IT_ERR );   
//      
//      if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) m_UARTStatus = UART_TXE_TIMEOUT;   
//      
//      if(m_Transaction.XferDoneCallback) m_Transaction.XferDoneCallback->CallbackFunction(m_UARTStatus);       
//      
//      m_UARTState = UART_READY;
//    }
    else // Handle remaining events here
    {
      if(LL_USART_IsActiveFlag_ORE(m_UARTx))
      {
        LL_USART_ClearFlag_ORE(m_UARTx);
      }
      
      if(LL_USART_IsActiveFlag_nCTS(m_UARTx))
      {
        LL_USART_ClearFlag_nCTS(m_UARTx);
      }
      
       if(LL_USART_IsActiveFlag_LBD(m_UARTx))
      {
        LL_USART_ClearFlag_LBD(m_UARTx);
      }
      
      if(LL_USART_IsActiveFlag_IDLE(m_UARTx))
      {
        LL_USART_ClearFlag_IDLE(m_UARTx);
      }
      
      if(LL_USART_IsActiveFlag_NE(m_UARTx))
      {
        LL_USART_ClearFlag_NE(m_UARTx);
      }
      
      if(LL_USART_IsActiveFlag_FE(m_UARTx))
      {
        LL_USART_ClearFlag_FE(m_UARTx);
      }
      
      if(LL_USART_IsActiveFlag_PE(m_UARTx))
      {
        LL_USART_ClearFlag_PE(m_UARTx);
      }        
    }
  } 
#else
  void Uart::UartISR()
  {
  }
#endif  // UART_INTR

#if UART_DMA 
  
  Uart::UartStatus_t Uart::TxDMA(uint8_t* TxBuf, uint16_t TxLen,UARTCallback_t* pUARTCallback)
  {
    if( (TxBuf == nullptr) || (TxLen == 0) ) return UART_INVALID_PARAMS;
    
    m_UARTState = UART_TX_BUSY;
    m_Transaction.Len = TxLen;
    m_Transaction.Buf = TxBuf;
    m_Transaction.XferDoneCallback = pUARTCallback; 
    //EnableInterrupt(IT_TXE | IT_PE | IT_ERR);
    LoadTxDmaChannel(TxBuf,TxLen);
    LL_USART_EnableDMAReq_TX(m_UARTx);
    return UART_OK;
  }
  
  Uart::UartStatus_t Uart::RxDMA(uint8_t* RxBuf, uint16_t RxLen,UARTCallback_t* pUARTCallback)
  {
    if( (RxBuf == nullptr) || (RxLen == 0) ) return UART_INVALID_PARAMS;
    
    m_UARTState = UART_TX_BUSY;
    m_Transaction.Len = RxLen;
    m_Transaction.Buf = RxBuf;
    m_Transaction.XferDoneCallback = pUARTCallback; 
    //EnableInterrupt(IT_RXNE | IT_PE | IT_ERR);
    LoadRxDmaChannel(RxBuf,RxLen);
    LL_USART_EnableDMAReq_RX(m_UARTx);
    return UART_OK;
  }
  
  void Uart::UART_DMA_Tx_Done_Handler()
  {			      
    /* Transfer Complete Interrupt management ***********************************/
    if(dma1.GetFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TC_FLAG))
    {
      /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
      if(LL_DMA_GetMode(dma1.m_DMAx,m_Curent_DMA_Tx_Channel) == LL_DMA_MODE_CIRCULAR)
      {
        /* Disable all the DMA interrupt */
        dma1.DisableInterrupt(m_Curent_DMA_Tx_Channel,DMA_CCR_HTIE);
      }                   
      
      dma1.ClearFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TC_FLAG | HAL::DMA::DMA_HC_FLAG);
      
      LL_USART_DisableDMAReq_TX(m_UARTx);  
      
      if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) m_UARTStatus = UART_TXE_TIMEOUT;   
      else                                                    m_UARTStatus = UART_OK;
    }     
    /* Half Transfer Complete Interrupt management ******************************/
    else if(dma1.GetFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_HC_FLAG))
    {        
      /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
      if(LL_DMA_GetMode(dma1.m_DMAx,UART1_RX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
      {
        /* Disable the half transfer interrupt */
        dma1.DisableInterrupt(m_Curent_DMA_Tx_Channel,DMA_CCR_HTIE);
      }
      /* Clear the half transfer complete flag */
      dma1.ClearFlag(m_Curent_DMA_Tx_Channel, HAL::DMA::DMA_HC_FLAG);
    }      
    /* Transfer Error Interrupt management **************************************/
    else if(dma1.GetFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TE_FLAG))
    {
      /* Disable all the DMA interrupt */
      m_UARTStatus = UART_DMA_ERROR;  
      dma1.DisableInterrupt(m_Curent_DMA_Tx_Channel,DMA_CCR_HTIE | DMA_CCR_TCIE | DMA_CCR_TEIE);
    }
    
    if(m_Transaction.XferDoneCallback) m_Transaction.XferDoneCallback->CallbackFunction(m_UARTStatus);       
    
    m_UARTState = UART_READY;      
  }
  
  void Uart::UART_DMA_Rx_Done_Handler()
  {			      
    /* Transfer Complete Interrupt management ***********************************/
    if(dma1.GetFlag(m_Curent_DMA_Rx_Channel,HAL::DMA::DMA_TC_FLAG))
    {
      /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
      if(LL_DMA_GetMode(dma1.m_DMAx,m_Curent_DMA_Rx_Channel) == LL_DMA_MODE_CIRCULAR)
      {
        dma1.DisableInterrupt(m_Curent_DMA_Rx_Channel,DMA_CCR_HTIE);
      }                  
      
      dma1.ClearFlag(m_Curent_DMA_Rx_Channel,HAL::DMA::DMA_TC_FLAG | HAL::DMA::DMA_HC_FLAG);  
      
      LL_USART_DisableDMAReq_RX(m_UARTx); 
      
      if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) m_UARTStatus = UART_TXE_TIMEOUT;   
      else                                                    m_UARTStatus = UART_OK;    
    }     
    /* Half Transfer Complete Interrupt management ******************************/
    else if(dma1.GetFlag(m_Curent_DMA_Rx_Channel,HAL::DMA::DMA_HC_FLAG))
    {        
      /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
      if(LL_DMA_GetMode(dma1.m_DMAx,UART1_RX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
      {
        /* Disable the half transfer interrupt */
        dma1.DisableInterrupt(m_Curent_DMA_Rx_Channel,DMA_CCR_HTIE);
      }
      /* Clear the half transfer complete flag */
      dma1.ClearFlag(m_Curent_DMA_Rx_Channel, HAL::DMA::DMA_HC_FLAG);
    }      
    /* Transfer Error Interrupt management **************************************/
    else if(dma1.GetFlag(m_Curent_DMA_Rx_Channel,HAL::DMA::DMA_TE_FLAG))
    {
      m_UARTStatus = UART_DMA_ERROR;        
      dma1.DisableInterrupt(m_Curent_DMA_Rx_Channel,DMA_CCR_HTIE | DMA_CCR_TCIE | DMA_CCR_TEIE);
    }     
    
    if(m_Transaction.XferDoneCallback) m_Transaction.XferDoneCallback->CallbackFunction(m_UARTStatus);       
    
    m_UARTState = UART_READY;
  }  
  
  void Uart::LoadRxDmaChannel(uint8_t* Buf, uint32_t len)
  {
    dma1.Load(m_Curent_DMA_Rx_Channel, (uint32_t)&(m_UARTx->DR),(uint32_t)Buf,len, LL_DMA_DIRECTION_PERIPH_TO_MEMORY, LL_DMA_PDATAALIGN_BYTE);
  }
  
  void Uart::LoadTxDmaChannel(uint8_t* Buf, uint32_t len)
  {
    dma1.Load(m_Curent_DMA_Tx_Channel, (uint32_t)&(m_UARTx->DR),(uint32_t)Buf,len, LL_DMA_DIRECTION_MEMORY_TO_PERIPH, LL_DMA_PDATAALIGN_BYTE);
  }
  
//  void Uart::UART_DMA_Tx_Callback::ISR( )
//  {      
//    _this->UART_DMA_Tx_Done_Handler(); 
//  }
//  
//  void Uart::UART_DMA_Rx_Callback::ISR( )
//  {      
//    _this->UART_DMA_Rx_Done_Handler(); 
//  }
#endif // UART_DMA 
  
  
  
  
}