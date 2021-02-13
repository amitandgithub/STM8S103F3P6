/******************
** CLASS: Spi
**
** DESCRIPTION:
**  Implements the Spi driver
**
** CREATED: 8/11/2019, by Amit Chaudhary
**
** FILE: Spi.h
**
******************/
#ifndef Spi_h
#define Spi_h

//#include"stm8l10x_spi.h"
#include"system.h"
#include"DigitalOut.h"
#include"QueueCpp.h"  


namespace HAL
{    
  
#define SPI_DEBUG 0
  
#define SPI_POLL                1  // 638
  
#define SPI_MASTER_Q            0  // 646
  
#define SPI_MASTER_INTR         0  // 1'724 , 1'892 Q
#define SPI_SLAVE_INTR          0  // 2'776, 2'824Q
#define SPI_MASTER_DMA          0  // 1'920, 2'238Q
#define SPI_SLAVE_DMA           0  // 2'620, 2'676Q - 1'456
  
  // Full SPI module - 4'646
  
#define SPI_LOG_STATES_SIZE 500
  
  
  
  class Spi
  {
  public:

    static const uint16_t SPI_TIMEOUT = 5000U;

    typedef uint16_t Hz_t ;
    
    typedef SPI_TypeDef* SPIx_t;  
    
    typedef enum 
    {
      SPI_OK,
      SPI_DMA_ERROR,
      SPI_SLAVE_TX_DONE,
      SPI_SLAVE_RX_DONE,
      SPI_SLAVE_TXRX_DONE,
      SPI_SLAVE_TX_DMA_DONE,
      SPI_SLAVE_RX_DMA_DONE,
      SPI_SLAVE_TXRX_DMA_DONE,
      SPI_BUSY,
      SPI_INVALID_PARAMS,
      SPI_TXE_TIMEOUT,
      SPI_RXNE_TIMEOUT,
      SPI_BUSY_TIMEOUT,
      SPI_TXN_POSTED,
      SPI_TXN_QUEUE_ERROR,
    }SpiStatus_t;
    
    typedef void(*spiMasterCallback_t)(SpiStatus_t Status);
    
    typedef void(*spiSlaveCallback_t)(SpiStatus_t Status);
    
    typedef enum
    {
      SPI_RESET,
      SPI_READY,
      SPI_TX_DONE,
      SPI_RX_DONE,
      SPI_MASTER_TX,
      SPI_MASTER_RX,
      SPI_MASTER_TXRX,
      SPI_SLAVE_TX,
      SPI_SLAVE_RX,
      SPI_SLAVE_TXRX,
      SPI_SLAVE_RX_LISTENING,
      SPI_MASTER_TX_DMA,
      SPI_MASTER_RX_DMA,
      SPI_SLAVE_TX_DMA,
      SPI_SLAVE_RX_DMA,
      SPI_MASTER_TXRX_DMA,
      SPI_ERROR_BUSY_TIMEOUT,
    }SPIState_t;
    
    typedef struct
    {
      uint8_t*                  TxBuf; 
      uint8_t                   TxLen;
      uint8_t                   TxIdx;
      uint8_t*                  RxBuf; 
      uint8_t                   RxLen;
      uint8_t                   RxIdx;      
      SPIState_t                SPIState;
      spiMasterCallback_t       XferDoneCallback;
    }Transaction_t;

    typedef void (Spi::*CurrentIsr)(); 
    
    Spi();
    
    ~Spi(){};
    
    SpiStatus_t HwInit(bool Slave = false);

    void PinsInitMaster();
    
    void PinsInitSlave();
      
    //uint8_t WaitOnFlag(volatile uint8_t* reg, uint8_t bitmask, uint8_t status, uint16_t timeout);
    
    SpiStatus_t HwDeinit();
    
    void ClockEnable(){CLK->PCKENR1 |= ((uint8_t)CLK_PCKENR1_SPI);}
    
    void ClockDisable(){CLK->PCKENR1 &= (uint8_t)(~(uint8_t)CLK_PCKENR1_SPI);}
  
    void Enable(){SPI->CR1 |= SPI_CR1_SPE;}
    
    void Disable(){SPI->CR1 &= (uint8_t)(~SPI_CR1_SPE);}
    
    void EnableInterrupt(uint8_t InterruptFlags){SPI->ICR |= InterruptFlags;}
    
    void DisableInterrupt(uint8_t InterruptFlags){SPI->ICR &= (uint8_t)(~InterruptFlags);}
    
   // void LL_SPI_ClearFlag_OVR();
    
    SpiStatus_t TxRxPoll(uint8_t* TxBuf, uint8_t* RxBuf, uint16_t Len);
    
    SpiStatus_t CheckAndLoadTxn(Transaction_t const *pTransaction);
    
    SpiStatus_t CheckAndLoadTxn(uint8_t* TxBuf, uint32_t TxLen,uint8_t* RxBuf, uint32_t RxLen,spiMasterCallback_t XferDoneCallback);
      
    SPIState_t GetState(){return m_Transaction.SPIState;}   
    
    void SetMode(uint8_t mode){ SPI->CR1 &= 0xFFFCU; SPI->CR1 |= mode&3; } // Shd not call when Comm is ongoing, register SPI_CR1 CPHA: bit 0, CPOL: bit 1

    SpiStatus_t     Tx(uint8_t data);   
    
    SpiStatus_t     TxPoll(uint8_t* TxBuf, uint16_t TxLen);
    
    SpiStatus_t     RxPoll(uint8_t* RxBuf, uint16_t RxLen);

    SpiStatus_t     XferPoll(Transaction_t const *pTransaction);
    
    SpiStatus_t     TxRxIntr(Transaction_t const * pTransaction);    
    
    SpiStatus_t     XferIntr(Transaction_t const *pTransaction);  
    
    SpiStatus_t RxIntr(uint8_t* RxBuf, uint32_t RxLen,spiMasterCallback_t XferDoneCallback);
    
    void TxnDoneHandler_INTR();
    
    void ISR();
    
    //static void Spi_ISR(){m_this};
      
    void Master_Tx_8bit_Isr();
    
    void Master_Rx_8bit_Isr();
    
    void Slave_Tx_8bit_Isr();
    
    void Slave_Rx_8bit_Isr();
    
    void SrartListeningTXIntr8bit(uint8_t* Buf, uint8_t Len);
    
    void SrartListeningRXIntr8bit(uint8_t* Buf, uint8_t Len);  
    
    //static volatile Transaction_t           m_Transaction;
    static Transaction_t           m_Transaction;
  private:   
   
    volatile SPIState_t     m_SPIState;         /* It must be volatile becoz it is shared between ISR and main loop */   
    volatile SpiStatus_t    m_SpiStatus;        /* It must be volatile becoz it is shared between ISR and main loop */ 
    
    CurrentIsr              m_CurrentIsr;
  };     
  
#define SPI_WAIT_FOR_TXE_FLAG_TO_SET(_SPI_,TIMEOUT) WaitOnFlag(&_SPI_->SR, SPI_SR_TXE, 0, TIMEOUT)
  
#define SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(_SPI_,TIMEOUT) WaitOnFlag(&_SPI_->SR, SPI_SR_BSY, SPI_SR_BSY, TIMEOUT)
  
#define SPI_WAIT_FOR_RXNE_FLAG_TO_SET(_SPI_,TIMEOUT) WaitOnFlag(&_SPI_->SR, SPI_SR_RXNE, 0, TIMEOUT)
  
#define SPI_TXE(_SPI_) _SPI_->SR & SPI_SR_TXE
  
#define SPI_RXNE(_SPI_) _SPI_->SR & SPI_SR_RXNE
  
#define SPI_BYTE_IN_8_BIT(_SPI_,__BUF)       __BUF.RxBuf[__BUF.RxIdx++] = _SPI_->DR
  
#define SPI_BYTE_OUT_8_BIT(_SPI_,__BUF)     _SPI_->DR = __BUF.TxBuf[__BUF.TxIdx++]
  
#define SPI_TX_DATA_AVAILABLE(__BUF)  (__BUF.TxIdx < __BUF.TxLen)

#define SPI_TX_DATA_COMPLETE(__BUF)  (__BUF.TxIdx >= __BUF.TxLen)
  
#define SPI_RESET_TX_BUF(__BUF) __BUF.TxIdx = 0
  
#define SPI_RX_DATA_AVAILABLE(__BUF)  (__BUF.RxIdx < __BUF.RxLen)

#define SPI_RX_DATA_COMPLETE(__BUF)  (__BUF.RxIdx >= __BUF.RxLen)

#define SPI_RESET_RX_BUF(__BUF) __BUF.RxIdx = 0 
  
#define LL_SPI_ClearFlag_OVR(_SPI_) _SPI_->DR = _SPI_->DR; _SPI_->SR = _SPI_->SR
  
#define SPI_DISABLE_ALL_INTERRUPTS(_SPI_)  _SPI_->ICR = 0
  
#define SPI_ENABLE_INTERRUPTS(_SPI_,Interrupt_Flags)  _SPI_->ICR |= (uint8_t)Interrupt_Flags
  
#define SPI_CS_LOW()     //(((GPIO_TypeDef*)m_Transaction.ChipSelectPort)->ODR |= 1 << m_Transaction.ChipSelectPin)
  
#define SPI_CS_HIGH()    //(((GPIO_TypeDef*)m_Transaction.ChipSelectPort)->ODR &= ~(1 << m_Transaction.ChipSelectPin))
  
}
#endif //SPI_h


/*
SPI Driver Facts:

1. In Spi slave mode Spi driver needs atleast 13 micro seconds for the Spi ISR to process one byte.
   So, Spi Master shall wait for 13us to send next byte to this SPI splave


*/