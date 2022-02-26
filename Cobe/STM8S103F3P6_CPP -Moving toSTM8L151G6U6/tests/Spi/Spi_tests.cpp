


#include"Spi_tests.h"


using namespace HAL;

enum 
{
  MASTER_INIT,
  POLL_TX_16BIT,
  w25qxx_POLL_TX,
  w25qxx_POLL_RX,
  w25qxx_POLL_TX_RX,
  w25qxx_POLL_TXN,
  INTR_TX_16BIT,
  w25qxx_INTR_TX,
  w25qxx_INTR_RX,
  w25qxx_INTR_TX_RX,
  w25qxx_INTR_TXN,
  w25qxx_INTR_TXN_Q,  
  DMA_TX_16BIT,
  w25qxx_DMA_TX,
  w25qxx_DMA_RX,
  w25qxx_DMA_TX_RX,
  w25qxx_DMA_TXN,
  w25qxx_DMA_TXN_Q,

  SLAVE_DMA,
  SLAVE_INIT,
  SLAVE_INTR_TX,
  SLAVE_INTR_RX,
  SLAVE_INTR_RX_TX,
  SLAVE_DMA_RX,
  SLAVE_DMA_TX,
  POLL_SLAVE_TEST
};

#define SPI_TX_SIZE 8
#define SPI_RX_SIZE 6

DigitalOut<B3,OUTPUT_OPEN_DRAIN_PUSH_PULL> Cs;

uint8_t pBuffer[SPI_RX_SIZE];
uint8_t SPI_TxBuf[SPI_TX_SIZE] = "Sumit 1";
uint8_t SPI_RxBuf[SPI_RX_SIZE];
Spi spi;

void SpiCallback(Spi::SpiStatus_t Status)
{
  if(Status == Spi::SPI_SLAVE_RX_DONE )
  {
    SPI_TxBuf[SPI_TX_SIZE-2]++;
    spi.SrartListeningTXIntr8bit(SPI_TxBuf,SPI_TX_SIZE-1);
  } 
  else  if(Status == Spi::SPI_SLAVE_TX_DONE )
  {
    spi.SrartListeningRXIntr8bit(SPI_RxBuf,SPI_RX_SIZE);
  }
}
uint32_t Pc_Count;
void Spi_Tests()
{
  uint8_t test_id = SLAVE_INIT;//w25qxx_POLL_TX;
  Spi::Transaction_t Transaction;
  
  while(1)
  {
    switch(test_id)
    { 
    case MASTER_INIT:  
      Cs.HwInit();
      Cs.High();
      spi.HwInit(false);
      test_id = SLAVE_INTR_RX_TX;
      break;
      
    case w25qxx_POLL_TX:  
      Cs.Low();
      spi.Tx('A');
      spi.Tx('m');
      spi.Tx('i');
      spi.Tx('t');
      Cs.High();
      //break;
    case w25qxx_POLL_TXN:      
      Transaction.TxBuf = pBuffer;
      Transaction.TxLen = 5;
      Transaction.RxBuf = 0;
      Transaction.RxLen = 0;
      Transaction.XferDoneCallback = SpiCallback;      
      Cs.Low();
      spi.XferPoll(&Transaction);
      Cs.High();
    case w25qxx_INTR_TX:         
      Cs.Low();
      spi.XferIntr(&Transaction);
      while(spi.GetState() != Spi::SPI_READY);
      Cs.High();
    case SLAVE_INIT:    
      Transaction.TxBuf = SPI_RxBuf;
      Transaction.TxLen = 5;
      Transaction.RxBuf = SPI_RxBuf;
      Transaction.RxLen = 5;
      spi.m_Transaction.XferDoneCallback = 0;//SpiCallback;
      spi.HwInit(true);
      //spi.SrartListeningRXIntr8bit(SPI_RxBuf,SPI_RX_SIZE); // 32bytes - 456 us; 64bytes - 911 us; 128bytes - 1.82 ms
      //spi.SrartListeningTXIntr8bit(SPI_TxBuf,SPI_TX_SIZE-1);
     // while(1) Pc_Count++;
      test_id = SLAVE_INTR_RX_TX;
      break; 
      
    case SLAVE_INTR_TX:
      spi.SrartListeningTXIntr8bit(SPI_TxBuf,SPI_TX_SIZE-1);
      while(1)
      {
        if(spi.GetState() == HAL::Spi::SPI_TX_DONE)
        {
          spi.SrartListeningTXIntr8bit(SPI_TxBuf,SPI_TX_SIZE-1);
        }
        Pc_Count++;
      }
      break;  
      
    case SLAVE_INTR_RX:
      spi.SrartListeningRXIntr8bit(SPI_RxBuf,SPI_RX_SIZE);
      while(1)
      {
        if(spi.GetState() == HAL::Spi::SPI_RX_DONE)
        {
          spi.SrartListeningRXIntr8bit(SPI_RxBuf,SPI_RX_SIZE);
        }
        Pc_Count++;
      }
      break;
      
      case SLAVE_INTR_RX_TX:
      spi.SrartListeningRXIntr8bit(SPI_RxBuf,SPI_RX_SIZE);
      while(1)
      {
        if(spi.GetState() == HAL::Spi::SPI_RX_DONE)
        {
          SPI_TxBuf[SPI_TX_SIZE-2]++;
          spi.SrartListeningTXIntr8bit(SPI_TxBuf,SPI_TX_SIZE-1);
        }
        else if(spi.GetState() == HAL::Spi::SPI_TX_DONE)
        {
          spi.SrartListeningRXIntr8bit(SPI_RxBuf,SPI_RX_SIZE);
        }
        Pc_Count++;
      }
      break;
      
    default: break;
    }
    delay_ms(100);
  }
  
}