

//#include "Uart.h"

extern DigitalOut<B1> PB1;

#define UART_X uart1
void Uart_DMA_Test();
void EchoPoll();

static HAL::Uart uart1(USART);

uint8_t Uart_RxBuf[20] = "Amit Chaudhary\n\r";

volatile static uint8_t uart_Rx_Done=0;


void Uart_Test()
{  
  static bool InitDone;  
  if(InitDone == false)
  {
    UART_X.HwInit(115200);
    InitDone = true;
  }
  
  UART_X.TxPoll(0x31);
//  UART_X.TxPoll((uint8_t)'\n');
//  UART_X.TxPoll(0x33,0x34,0x35,0x36);
//  UART_X.TxPoll((uint8_t)'\n');
  while(1)
  {
    // Poll test
    UART_X.TxPoll((uint8_t*)"Amit Chaudhary\r", sizeof("Amit Chaudhary\r")); 
    
    UART_X.TxHex8(0xBA);
    UART_X.TxHex8(0xDA);
    UART_X.TxPoll('\r');
    
    UART_X.TxHex16(0xDEAD);
    UART_X.TxPoll('\r');
    
    UART_X.TxHex32(0xBEADBABE);
    UART_X.TxPoll('\r');
    
    //EchoPoll();

    
#if UART_INTR
    // Interrupt Test
    uart_Rx_Done = 0;
    UART_X.RxIntr(Uart_RxBuf, sizeof("Amit Chaudhary")-1,&UartCallback);
    while(uart_Rx_Done == 0);    
    
    LL_mDelay(500);
    UART_X.TxIntr(Uart_RxBuf, sizeof("Amit Chaudhary\n\r"));    
    while(UART_X.GetState() != HAL::Uart::UART_READY );
    LL_mDelay(500); 
#endif

#if UART_DMA
  // DMA Test
  uart_Rx_Done = 0;
  UART_X.RxDMA(Uart_RxBuf, sizeof("Amit Chaudhary")-1,&UartCallback);
  while(uart_Rx_Done == 0);    

  UART_X.TxDMA(Uart_RxBuf, sizeof("Amit Chaudhary\n\r"));    
  while(UART_X.GetState() != HAL::Uart::UART_READY );  
#endif
  //UART_X.TxPoll('A');
  delay_ms(100);
  }
  
}


void EchoPoll()
{
  uint8_t RxByte;
  while( UART_X.RxPoll(&RxByte,1) != HAL::Uart::UART_OK) ;
  UART_X.TxPoll(RxByte);}
