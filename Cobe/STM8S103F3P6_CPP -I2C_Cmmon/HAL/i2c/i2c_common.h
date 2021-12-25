#ifndef I2C_COMMON_H
#define I2C_COMMON_H

#include "system.h"
#include "stm8s_i2c.h"

#if I2C_DEBUG
#define I2C_LOG_STATES_SIZE 255
#define I2C_LOG_EVENTS(x) I2cLogStates(x)
#define I2C_LOG_STATES(x) I2cLogStates(x)
#else
#define I2C_LOG_STATES_SIZE 1
#define I2C_LOG_EVENTS(x)
#define I2C_LOG_STATES(x)
#endif

#define I2Cx ((I2C_TypeDef *)i2cx)
#define I2C_DIR_READ 0x01u
#define I2C_DIR_WRITE 0xFEu

#define I2C_BUF_BYTE_IN(__I2C_BUF)  \
  (*__I2C_BUF.RxBuf++) = I2Cx->DR; \
  __I2C_BUF.RxLen--

#define I2C_BUF_BYTE_OUT(__I2C_BUF) \
  I2Cx->DR = (*__I2C_BUF.TxBuf++); \
  __I2C_BUF.TxLen--

#define I2C_SLAVE_BUF_BYTE_IN(__I2C_BUF) __I2C_BUF.RxBuf->Buf[__I2C_BUF.RxBuf->Idx++] = I2Cx->DR

#define I2C_SLAVE_BUF_BYTE_OUT(__I2C_BUF) I2Cx->DR = __I2C_BUF.TxBuf->Buf[__I2C_BUF.TxBuf->Idx++]

namespace HAL
{

  namespace i2c
  {

    typedef enum
    {
      i2c1 = I2C_BaseAddress
    } i2c_t;

    typedef enum
    {
      CLK_100KHz = (uint32_t)100000,
      CLK_400KHz = (uint32_t)400000
    } Clk_t;

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
    } Status_t;

    typedef struct
    {
      uint8_t *Buf;
      uint8_t Len;
      uint8_t Idx;
    } Buf_t;

    typedef void (*Callback_t)(Status_t Status);

    typedef struct
    {
      uint8_t SlaveAddress;
      uint8_t RepeatedStart;
      uint8_t TxLen;
      uint8_t RxLen;
      uint8_t *TxBuf;
      uint8_t *RxBuf;
      Callback_t XferDoneCallback;
    } MasterTxn_t;

    template <typename Data_t, typename Len_t, typename Cb_t>
    struct Transaction
    {
      typedef struct
      {
        Data_t *TxBuf;
        Data_t *RxBuf;
        Len_t   TxLen;
        Len_t   RxLen;
        Cb_t    XferDoneCallback;
        uint8_t SlaveAddress;
        uint8_t RepeatedStart;
      } Txn_t;

      Txn_t Txn;

      void Write(Data_t data)
      {
        *Txn.RxBuf++ = data;
        Txn.RxLen--;
      }

      Data_t Read(void)
      {
        Data_t data;

        data = *Txn.TxBuf++;
        Txn.TxLen--;
        return data;
      }

      void SetTx(Data_t *TxBuf, Len_t TxLen)
      {
        Txn.TxBuf = TxBuf;
        Txn.TxLen = TxLen;
      }

      void SetRx(Data_t *RxBuf, Len_t RxLen)
      {
        Txn.RxBuf = RxBuf;
        Txn.RxLen = RxLen;
      }

      void Set(Cb_t Cb)
      {
        Txn.XferDoneCallback = Cb;
      }

      void Set(Txn_t* pTxn)
      {

      }

    };

    typedef struct
    {
      Buf_t *TxBuf;
      Buf_t *RxBuf;
      uint8_t DefaultByte;
      Callback_t XferDoneCallback;
    } SlaveTxn_t;

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
    } State_t;

    static void DummyCallback(Status_t Status)
    {
      return;
    }

  

  } // namespace i2c

} // namespace HAL

#endif // I2C_COMMON_H