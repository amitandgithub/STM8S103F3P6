/******************
** CLASS: I2c
**
** DESCRIPTION:
**  I2C class implementation
**
** CREATED: 8/14/2019, by Amit Chaudhary
**
** FILE: i2c.h
**
******************/
#ifndef i2c_hpp
#define i2c_hpp

#include"system.h"
#include"DigitalOut.h"
//#include"QueueCpp.h"

#define I2C_DEBUG 0

#define I2C_INTERRUPT_MODE 1

                        // ALL i2c.o                    1 257       2        2
#define I2C_SLAVE_ONLY 1  // i2c.o                       566        2        2
#define I2C_MASTER_ONLY 0  //i2c.o                       913        2        2

/*Method 1: This method is for the case when the I2C is used with interrupts that have the highest priority in the application.
This method is faster in communication. */
#define I2C_RX_METHOD_1

#define I2C_SLAVE_TX_BUF_SIZE 32
#define I2C_SLAVE_RX_BUF_SIZE 32


#define __INLINE static inline

#define DBG_LOG_CREATE_ID(__MODULE_ID,__X) __X

#if I2C_DEBUG
#define I2C_LOG_STATES_SIZE 255
#define I2C_LOG_EVENTS(x) I2cLogStates(x)
#define I2C_LOG_STATES(x) I2cLogStates(x)
#endif

#define I2C_TIMEOUT 5000u
#define DGB_PRINT(x) 

#define I2C_DIR_READ  0x01u
#define I2C_DIR_WRITE 0xFEu

#define I2C_DATA_REG    (I2C->DR)
#define I2C_SR3_DUALF   ((uint8_t)0x80)

#define I2C_BUF_BYTE_IN(__I2C_BUF)              (*__I2C_BUF->RxBuf++) = I2C_DATA_REG; __I2C_BUF->RxLen--

#define I2C_BUF_BYTE_OUT(__I2C_BUF)             I2C_DATA_REG = (*__I2C_BUF->TxBuf++); __I2C_BUF->TxLen--

#define I2C_SLAVE_BUF_BYTE_IN(__I2C_BUF)        __I2C_BUF.RxBuf->Buf[__I2C_BUF.RxBuf->Idx++] = I2C_DATA_REG

#define I2C_SLAVE_BUF_BYTE_OUT(__I2C_BUF)        I2C_DATA_REG = __I2C_BUF.TxBuf->Buf[__I2C_BUF.TxBuf->Idx++]

class i2c
{
public:
  
//  typedef void(*I2CCallback_t)(void);
//  
//  typedef void(*I2CSlaveCallback_t)(uint8_t Len, uint8_t* Buf);
  
  typedef enum 
  {
    I2C_OK,
    //I2C_BUSY,
    I2C_BUSY_TIMEOUT,
    I2C_START_TIMEOUT,
    I2C_ADDR_TIMEOUT,
    I2C_BTF_TIMEOUT,
    I2C_RXNE_TIMEOUT,
    I2C_TXE_TIMEOUT,
    I2C_STOP_TIMEOUT,
    I2C_STOP_DETECTED,
    I2C_ACK_FAIL,
    I2C_ARB_LOST,
    I2C_INTR_PENDING,
    I2C_BUS_ERROR,
    I2C_DATA_OVR,
    I2C_INVALID_PARAMS,
    I2C_XFER_DONE,
    I2C_SLAVE_TX_DONE,
    I2C_SLAVE_RX_DONE,
    I2C_SLAVE_RX_DONE_WITH_NACK,
    I2C_TXN_POSTED,
    I2C_TXN_QUEUE_ERROR,
  }I2CStatus_t;
  
  typedef struct
  {
    uint8_t* Buf;
    uint8_t  Len;
    uint8_t  Idx;     
  }i2cBuf_t;
  
  typedef void(*i2cMasterCallback_t)(I2CStatus_t Status);
  
  typedef void(*i2cSlaveCallback_t)(I2CStatus_t Status);
  
  typedef struct
  {
    uint8_t                 SlaveAddress;
    uint8_t                 RepeatedStart;
    uint8_t                 TxLen;
    uint8_t                 RxLen; 
    uint8_t*                TxBuf; 
    uint8_t*                RxBuf;
    i2cMasterCallback_t     XferDoneCallback;
  }MasterTxn_t;
  
  typedef struct
  {
    i2cBuf_t*           TxBuf;
    i2cBuf_t*           RxBuf;
    uint8_t             DefaultByte;
    i2cSlaveCallback_t  XferDoneCallback;
  }SlaveTxn_t;
  
  typedef enum
  {
    I2C_RESET,
    //I2C_BUSY,
    I2C_READY,
    I2C_MASTER_TX,
    I2C_MASTER_RX,
    I2C_SLAVE_TX,
    I2C_SLAVE_RX,
    I2C_SLAVE_RX_LISTENING,
    I2C_MASTER_RX_REPEATED_START,
    I2C_MASTER_TX_ACK_FAIL,
    I2C_MASTER_TX_DMA,
    I2C_MASTER_RX_DMA,
    I2C_SLAVE_TX_DMA,
    I2C_SLAVE_RX_DMA,
  }I2CState_t; 
  
  typedef enum
  {														  
    I2C_LOG_STOPF_FLAG                           	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,1),
    I2C_LOG_STOPF_NOT_CLEARED                    	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,2),
    I2C_LOG_START_MASTER_TX                      	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,3),
    I2C_LOG_START_MASTER_RX                      	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,4),
    I2C_LOG_START_MASTER_TXRX                    	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,5),
    I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_0           	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,6),
    I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_1_DMA       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,7),
    I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_1           	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,8),
    I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_2_DMA       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,9),
    I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_2           	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,10),
    I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_OTHER       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,11),
    I2C_LOG_ADDR_INTR_MASTER_TX_SIZE_GT_0        	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,12),
    I2C_LOG_ADDR_INTR_MASTER_TX_REPEATED_START   	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,13),
    I2C_LOG_ADDR_INTR_MASTER_TX_STOP             	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,14),
    I2C_LOG_TXE_GT_0                              =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,15),
    I2C_LOG_TXE_DONE                             	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,16),
    I2C_LOG_REPEATED_START                       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,17),
    I2C_LOG_BTF_MASTER_TX_GT_0                   	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,18),
    I2C_LOG_BTF_MASTER_TX_REPEATED_START         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,19),
    I2C_LOG_BTF_MASTER_TX_STOP                   	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,20),
    I2C_LOG_BTF_MASTER_RX_SIZE_3                 	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,21),
    I2C_LOG_BTF_MASTER_RX_SIZE_2_STOPED          	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,22),
    I2C_LOG_BTF_MASTER_RX_SIZE_GT_3              	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,23),
    I2C_LOG_RXNE_MASTER_SIZE_GT_3                	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,24),
    I2C_LOG_RXNE_MASTER                          	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,25),
    I2C_LOG_RXNE_MASTER_SIZE_1                   	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,26),
    I2C_LOG_RXNE_MASTER_SIZE_2                   	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,27),
    I2C_LOG_RXNE_MASTER_SIZE_2_OR_3              	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,28),
    I2C_LOG_BTF_MASTER_RXNE_LAST              	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,29),
    I2C_LOG_RXNE_MASTER_LAST                     	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,30),
    I2C_LOG_BTF_MASTER_BERR                      	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,31),
    I2C_LOG_BTF_MASTER_ACK_FAIL                  	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,32),
    I2C_LOG_BTF_MASTER_ARB_LOST                  	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,33),
    I2C_LOG_SB_MASTER_TX                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,34),
    I2C_LOG_SB_MASTER_RX                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,35),
    I2C_LOG_SB_MASTER_RX_2                       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,36),
    I2C_LOG_SB_MASTER_RX_REPEATED_START          	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,37),
    I2C_LOG_STOP_MASTER_RX_WITHOUT_REPEATED_START	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,38),
    I2C_LOG_STOP_XFER_DONE                       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,39),
    I2C_LOG_BTF_MASTER_DATA_OVR                  	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,40),
    I2C_LOG_START_SLAVE_RX                       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,41),
    I2C_LOG_BTF_SLAVE_RX                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,42),
    I2C_LOG_RXNE_SLAVE_RX                        	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,43),
    I2C_LOG_BTF_SLAVE_RX_QUEUED                  	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,44),
    I2C_LOG_BTF_SLAVE_RX_QUEUE_FULL              	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,45),
    I2C_LOG_RXNE_SLAVE_RX_QUEUED                 	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,46),
    I2C_LOG_RXNE_SLAVE_RX_QUEUE_FULL             	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,47),
    I2C_LOG_AF_SLAVE_ACK_FAIL                    	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,48),
    I2C_LOG_TxE_SLAVE_TX                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,49),
    I2C_LOG_TxE_SLAVE_TX_QUEUE_EMPTY             	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,50),
    I2C_LOG_BTF_SLAVE_TX                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,51),
    I2C_LOG_BTF_SLAVE_TX_QUEUE_EMPTY             	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,52),
    I2C_LOG_MASTER_BTF_XFER_DONE                 	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,53),
    I2C_LOG_ADDR_XFER_DONE                       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,54),
    I2C_LOG_TXN_QUEUED                           	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,55),
    I2C_LOG_TXN_DEQUEUED                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,56),
    I2C_LOG_TXN_QUEUE_EMPTY                      	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,57),
    I2C_LOG_TXN_QUEUE_ERROR                      	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,58),
    I2C_LOG_TXN_DONE                             	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,59),
    I2C_LOG_TXN_DONE_ALL                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,60),
    I2C_LOG_DMA_TX_DONE                          	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,61),
    I2C_LOG_DMA_HALF_TX_DONE                     	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,62),
    I2C_LOG_DMA_TX_ERROR                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,63),
    I2C_LOG_ADDR_INTR_MASTER_TX_DMA              	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,64),
    I2C_LOG_ADDR_INTR_MASTER_RX_DMA              	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,65),
    I2C_LOG_BTF_MASTER_RX_DMA_STOP               	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,66),
    I2C_LOG_BTF_MASTER_TX_DMA_STOP               	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,67),
    I2C_LOG_START_MASTER_TX_DMA                  	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,68),
    I2C_LOG_START_MASTER_RX_DMA                  	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,69),
    I2C_LOG_REPEATED_START_MASTER_RX_DMA         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,70),
    I2C_LOG_DMA_HALF_RX_DONE                     	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,71),
    I2C_LOG_DMA_RX_ERROR                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,72),
    I2C_LOG_DMA_RX_DONE                          	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,73),
    I2C_LOG_ADDR_INTR_MASTER_RX_DMA_SIZE_1       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,74),
    I2C_LOG_ADDR_INTR_MASTER_RX_DMA_SIZE_2       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,75),
    I2C_LOG_DMA_TX_BTF_TIMEOUT                   	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,76),
    I2C_LOG_DMA_REPEATED_START                   	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,77),
    I2C_LOG_DMA_STOP_GENERATED                   	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,78),
    I2C_LOG_DMA_TXN_DONE                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,79),
    I2C_LOG_SB                                    =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,80),
    I2C_LOG_ADDR                                  =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,81),
    I2C_LOG_BTF                                   =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,82),
    I2C_LOG_RXNE                                  =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,83),
    I2C_LOG_TXE                                   =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,84),
    I2C_LOG_STOPF                                 =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,85),
    I2C_LOG_BERR                                  =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,86),
    I2C_LOG_ARLO                                  =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,87),
    I2C_LOG_AF                                    =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,88),
    I2C_LOG_OVR                                   =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,89),
    I2C_LOG_PECERR                                =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,90),
    I2C_LOG_ADD10                                	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,91),
    I2C_LOG_START_TIMEOUT                        	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,92),
    I2C_LOG_START_DONE                            =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,93),
    I2C_LOG_ADDR_TIMEOUT                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,94),
    I2C_LOG_ADDR_SENT                             =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,95),
    I2C_LOG_TXE_TIMEOUT                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,96),
    I2C_LOG_TX_DONE                               =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,97),
    I2C_LOG_RX_DONE                               =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,98),
    I2C_LOG_RXNE_TIMEOUT                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,99),
    I2C_LOG_BTF_TIMEOUT                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,100),
    I2C_LOG_RX_1                                  =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,101),
    I2C_LOG_RX_2                                  =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,102),
    I2C_LOG_RX_GT_2                               =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,103),
    I2C_LOG_RX_1_DONE                             =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,104),
    I2C_LOG_RX_2_DONE                             =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,105),
    I2C_LOG_RX_3_DONE                             =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,106),
    I2C_LOG_START                                 =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,107),
    I2C_LOG_STOP                                  =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,108),
    I2C_LOG_STOP_TIMEOUT                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,109),
    I2C_LOG_BUSY_TIMEOUT                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,110),
    I2C_LOG_ACK_FAIL                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,111),
    I2C_LOG_SLAVE_RX_DONE_WITH_DEFAULT_BYTE     =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,112),
    I2C_LOG_TXE_DEFAULT_BYTE                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,113),
    I2C_LOG_ADDR_SLAVE_RX_SELECTED              =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,114),
    I2C_LOG_ADDR_SLAVE_TX_SELECTED      =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,115),
    I2C_LOG_SLAVE_RX_BYTE_IN          	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,116),/*
    I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,117),
    I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,118),
    I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,119),
    I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,120),
    I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,121),
    I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,122),
    I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,123),
    I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,124),
    I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,125),
    I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,126),
    I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,127) */           
  }I2CLogs_t;
  
  
  //i2c(){m_i2c_this = this;};  
  
  // ~i2c(){};
  
  I2CStatus_t HwInit();
  
  void ClockEnable(){CLK->PCKENR1 |= ((uint8_t)CLK_PCKENR1_I2C);}
  
  void ClockDisable(){CLK->PCKENR1 &= (uint8_t)(~(uint8_t)CLK_PCKENR1_I2C);}
  
  void Enable(){I2C->CR1 |= I2C_CR1_PE;}
    
  void Disable(){I2C->CR1 &= (uint8_t)(~I2C_CR1_PE);}
  
  
  I2CStatus_t TxPoll(uint8_t SlaveAddress,uint8_t* TxBuf, uint8_t TxLen);  
  
  I2CStatus_t RxPoll(uint8_t SlaveAddress,uint8_t* RxBuf, uint8_t RxLen);
  
  I2CStatus_t XferPoll(uint8_t SlaveAddress,uint8_t* TxBuf, uint8_t TxLen, uint8_t* RxBuf = 0, uint8_t RxLen = 0, uint8_t RepeatedStart = 0);
  
  I2CStatus_t SendSlaveAddress(uint8_t SlaveAddress, uint8_t Repeatedstart);
  
  uint8_t WaitOnFlag(volatile uint8_t* reg, uint8_t bitmask, uint8_t status, uint16_t timeout);
  
  void PinsInit();
  
  void ScanBus(uint8_t* pFoundDevices, uint8_t size);
  
  static void I2C_ISR(void);
  
  I2CStatus_t XferIntr(MasterTxn_t* pTransaction);
  
  void TxnDoneHandler(uint8_t StopFlag);
  
  void GenerateStart(){ I2C->CR2 |= I2C_CR2_START;}
  
  void GenerateStop(){I2C->CR2 |= I2C_CR2_STOP;}
  
  inline void EnableACK(){I2C->CR2 |= I2C_CR2_ACK;}
  
  inline void DisableACK(){I2C->CR2 &= (uint8_t)(~I2C_CR2_ACK);}
  
  void EnablePOS(){I2C->CR2 |= (uint8_t)I2C_CR2_POS;}
  
  void DisablePOS(){I2C->CR2 &= (uint8_t)(~I2C_CR2_POS);;}
  
  void EnableI2c(){I2C->CR1 |= I2C_CR1_PE;}
  
  void DisableI2c(){I2C->CR1 &= (uint8_t)(~I2C_CR1_PE);}
  
  void Softreset(){I2C->CR2 |= I2C_CR2_SWRST; I2C->CR2 &= (uint8_t)(~I2C_CR2_SWRST); }
  
  void ClearADDR() {uint8_t dummy; dummy = I2C->SR1; dummy = I2C->SR3; dummy  = dummy;}
  
  void ClearAF(){I2C->SR2 &= (uint8_t)(~I2C_SR2_AF);}
  
  void ClearARLO(){I2C->SR2 &= (uint8_t)(~I2C_SR2_ARLO);}
  
  void ClearBERR(){I2C->SR2 &= (uint8_t)(~I2C_SR2_BERR);}
  
  void ClearOVR(){I2C->SR1 &= (uint8_t)(~I2C_SR2_OVR);}
  
  void ClearSTOPF(){uint8_t reg;reg = I2C->SR1;I2C->CR2 = I2C->CR2;reg = reg;}
  
  void Enable_EVT_BUF_ERR_Interrupt(){I2C->ITR = I2C_ITR_ITBUFEN | I2C_ITR_ITEVTEN | I2C_ITR_ITERREN;}
  
  void Disable_EVT_BUF_ERR_Interrupt(){I2C->ITR &= (uint8_t)~(I2C_ITR_ITBUFEN | I2C_ITR_ITEVTEN | I2C_ITR_ITERREN);}
  
  void Enable_BUF_Interrupt(){I2C->ITR |= I2C_ITR_ITBUFEN;}
  
  void Disable_BUF_Interrupt(){I2C->ITR &= (uint8_t)~I2C_ITR_ITBUFEN;}
  
  I2CState_t GetState(){return m_I2CState;}  
  
  void SB_Handler();
  
  void ADDR_Handler();
  
  void RXNE_Handler();
  
  void TXE_Handler();
  
  void BTF_Handler();
  
  void ADD10_Handler();
  
  void AF_Handler();
  
  void STOPF_Handler();  
    
  /* Master only Mode */
  
  static void I2C_ISR_Master(void);
  
  void ADDR_Handler_Master();
  
  void RXNE_Handler_Master();
  
  void TXE_Handler_Master();
  
  void BTF_Handler_Master(); 
  
  void AF_Handler_Master();
  
  void STOPF_Handler_Master(); 
  
  /* Slave only Mode */
  
  static void I2C_ISR_Slave(void);
  
  void SB_Handler_Slave();
  
  void ADDR_Handler_Slave();
  
  void RXNE_Handler_Slave();
  
  void TXE_Handler_Slave();
  
  void BTF_Handler_Slave(); 
  
  void AF_Handler_Slave();
  
  void STOPF_Handler_Slave(); 

  
  void SetSlaveCallback(i2cSlaveCallback_t I2CSlaveCallback){m_SlaveTxn.XferDoneCallback = I2CSlaveCallback;}
  
  void SwapMasterBuf(MasterTxn_t* i2cBuf){MasterTxn_t* temp = i2cBuf; i2cBuf = m_MasterTxn; m_MasterTxn = temp; }
  
  void SwitchSlaveTxBuf(i2cBuf_t* i2cSlaveTxBuf);
  
  void SwitchSlaveRxBuf(i2cBuf_t* i2cSlaveRxBuf);
  
  I2CStatus_t SlaveStartListening(i2cBuf_t* TxBuf, i2cBuf_t* RxBuf );
  
  void SlaveStartReceiving(){EnableACK();}
  
  void SlaveStopReceiving(){DisableACK();}
  
  void SetSlaveTxDefaultByte(uint8_t default_byte){m_SlaveTxn.DefaultByte = default_byte;}
  
  
  
#if I2C_DEBUG 
  inline void I2cLogStates(I2CLogs_t log);
  I2CLogs_t I2CStates[I2C_LOG_STATES_SIZE];
  uint8_t I2CStates_Idx;
#endif
  
private:
  static i2c*                   m_i2c_this;
  
  MasterTxn_t*                  m_MasterTxn;
  
  SlaveTxn_t                    m_SlaveTxn;
  
  /* It must be volatile becoz it is shared between ISR and main loop */
  volatile I2CState_t           m_I2CState; 
  
  /* It must be volatile becoz it is shared between ISR and main loop */
  volatile I2CStatus_t          m_I2CStatus;  
};





#if I2C_DEBUG 

inline void i2c::I2cLogStates(I2CLogs_t log)
{
  I2CStates[I2CStates_Idx++] = log;
  
  if(I2CStates_Idx == I2C_LOG_STATES_SIZE )
    I2CStates_Idx = 0;
}
#else

#define I2C_LOG_EVENTS(x) 
#define I2C_LOG_STATES(x)

#endif



#endif //i2c_hpp





















