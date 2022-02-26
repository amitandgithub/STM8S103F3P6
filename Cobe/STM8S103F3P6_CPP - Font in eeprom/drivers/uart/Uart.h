/******************
** CLASS: Uart
**
** DESCRIPTION:
**  Implements the Uart driver
**
** CREATED: 8/11/2019, by Amit Chaudhary
**
** FILE: Uart.h
**
******************/
#ifndef Uart_h
#define Uart_h

//#include"stm8l10x_usart.h"
#include"system.h"
#include"DigitalOut.h"


namespace HAL
{    
  
#define UART_POLL       1  // 502,
#define UART_INTR       0  // 808    - 502
#define UART_DMA        0  // 1'380  - 502
#define nullptr 0
  
#define HEX_CHARS      "0123456789ABCDEF"
  
  class Uart
  {
  public:
    static const uint32_t UART1_TX_DMA_CHANNEL = 4;
    static const uint32_t UART1_RX_DMA_CHANNEL = 5;
    static const uint32_t UART2_RX_DMA_CHANNEL = 6;
    static const uint32_t UART2_TX_DMA_CHANNEL = 7;
    static const uint32_t UART3_TX_DMA_CHANNEL = 2;
    static const uint32_t UART3_RX_DMA_CHANNEL = 3;
    static const uint16_t UART_TIMEOUT = 10000U;
    
    typedef enum 
    {
    UART_OK,
    UART_DMA_ERROR,
    UART_SLAVE_TX_DONE,
    UART_SLAVE_RX_DONE,
    UART_SLAVE_TXRX_DONE,
    UART_SLAVE_TX_DMA_DONE,
    UART_SLAVE_RX_DMA_DONE,
    UART_SLAVE_TXRX_DMA_DONE,
    UART_BUSY,
    UART_INVALID_PARAMS,
    UART_TXE_TIMEOUT,
    UART_RXNE_TIMEOUT,
    UART_TC_TIMEOUT,
    UART_TXN_POSTED,
    UART_TXN_QUEUE_ERROR,
  }UartStatus_t;
    
    typedef USART_TypeDef* UARTx_t;    

    typedef enum
    {
      UART_RESET,
      UART_READY,
      UART_TX_BUSY,
      UART_RX_BUSY,
    }UartState_t;      
    
    
    typedef void(*UARTCallback_t)(UartStatus_t Status);
    
    typedef struct
    {
      uint8_t          Len;
      uint8_t*          Buf;     
      UARTCallback_t*   XferDoneCallback;
    }Transaction_t; 
    
    Uart(UARTx_t UARTx);
    
    ~Uart(){};

    UartStatus_t HwInit(uint32_t Baudrate = 9600, uint8_t Stop = USART_StopBits_1, uint8_t Parity = USART_Parity_No);
    
    UartStatus_t HwDeinit();
    
    void ClockEnable(){CLK->PCKENR |= ((uint8_t)CLK_Peripheral_USART);}
    
    void ClockDisable(){CLK->PCKENR &= (uint8_t)(~(uint8_t)CLK_Peripheral_USART);}
  
    void Disable(){m_UARTx->CR1 |= USART_CR1_USARTD;}
    
    void Enable(){m_UARTx->CR1 &= (uint8_t)(~USART_CR1_USARTD);}
    
    void EnableInterrupt(uint8_t InterruptFlags){m_UARTx->CR1 |= (uint8_t)(InterruptFlags);}
    
    void DisableInterrupt(uint8_t InterruptFlags){m_UARTx->CR1 &= (uint8_t)(~InterruptFlags);}
    
    void PinsHwInit();
    
    UartState_t GetState(){return m_UARTState;}  
    
    void SetBaudrate(uint32_t Baudrate);
      
    void TxHex8(uint16_t num);
    
    void TxHex16(uint16_t num);
    
    void TxHex32(uint32_t num);
      
    UartStatus_t     TxPoll(uint8_t data);
    
    UartStatus_t     TxPoll(uint8_t* TxBuf, uint16_t TxLen);
    
    UartStatus_t     RxPoll(uint8_t* RxBuf, uint16_t RxLen);
    
    void UartISR();
    
    //UartStatus_t     TxIntr(uint8_t* TxBuf, uint16_t TxLen,UARTCallback_t* pUARTCallback = nullptr);
    
    //UartStatus_t     RxIntr(uint8_t* RxBuf, uint16_t RxLen,UARTCallback_t* pUARTCallback = nullptr);    

    
#if UART_DMA   
    
    UartStatus_t     TxDMA(uint8_t* TxBuf, uint16_t TxLen,UARTCallback_t* pUARTCallback = nullptr);
    
    UartStatus_t     RxDMA(uint8_t* RxBuf, uint16_t RxLen,UARTCallback_t* pUARTCallback = nullptr);
    
    void LoadRxDmaChannel(uint8_t* Buf, uint32_t len);
    
    void LoadTxDmaChannel(uint8_t* Buf, uint32_t len);  
    
    class UART_DMA_Rx_Callback : public InterruptSource
    {
    public:
      UART_DMA_Rx_Callback(Uart* This):_this(This){};
      virtual void ISR(){_this->UART_DMA_Rx_Done_Handler();}
    private:
      Uart* _this;
    }; 
    
    class UART_DMA_Tx_Callback : public InterruptSource
    {
    public:
      UART_DMA_Tx_Callback(Uart* This):_this(This){};
      virtual void ISR(){_this->UART_DMA_Tx_Done_Handler();}
    private:
      Uart* _this;
    }; 
    
    void UART_DMA_Tx_Done_Handler();
    
    void UART_DMA_Rx_Done_Handler();
#endif
    
  protected:  
    UARTx_t                     m_UARTx; 
    uint16_t                    m_Baudrate;
    volatile UartState_t        m_UARTState;         /* It must be volatile becoz it is shared between ISR and main loop */   
    volatile UartStatus_t       m_UARTStatus;        /* It must be volatile becoz it is shared between ISR and main loop */
    
#if UART_INTR || UART_DMA
    Transaction_t               m_Transaction;
#endif
    
#if UART_DMA 
    uint8_t                     m_Curent_DMA_Tx_Channel;
    uint8_t                     m_Curent_DMA_Rx_Channel;
    UART_DMA_Tx_Callback        m_UART_DMA_Tx_Callback;
    UART_DMA_Rx_Callback        m_UART_DMA_Rx_Callback;
#endif 
  };
  
} 

#define UART_TXE(_UART_) _UART_->SR & USART_SR_TXE
  
#define UART_RXNE(_UART_) _UART_->SR & USART_SR_RXNE

#define UART_TC(_UART_) _UART_->SR & USART_SR_TC

#define UART_WAIT_FOR_TXE_FLAG_TO_SET(_UART_,TIMEOUT) WaitOnFlag(&_UART_->SR, USART_SR_TXE, 0, TIMEOUT)
  
#define UART_WAIT_FOR_RXNE_FLAG_TO_SET(_UART_,TIMEOUT) WaitOnFlag(&_UART_->SR, USART_SR_RXNE, 0, TIMEOUT)

#define UART_WAIT_FOR_TC_FLAG_TO_SET(_UART_,TIMEOUT) WaitOnFlag(&_UART_->SR, USART_SR_TC, 0, TIMEOUT)

#define UART_BYTE_IN(__BUF)      *__BUF.Buf++ = m_UARTx->DR; __BUF.Len--

#define UART_BYTE_OUT(__BUF)     m_UARTx->DR = *__BUF.Buf++; __BUF.Len--

#endif