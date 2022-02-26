#ifndef I2C_H
#define I2C_H

#include"i2c_common.h"
#include"i2cPoll.h"
#include"i2cMasterIntr.h"

namespace HAL
{

  namespace i2c
  {
   /* This class is generic interface I2C Master class
    * It takes the "Impl" parameter as Master
    * Implementation, Impl can be Interrupts or DMA based
   */ 
    template <typename Impl>
    class Master
    {
    public:
      static Status_t HwInit(void)
      {
        return Impl::HwInit();
      }

      static void Start(uint8_t SlaveAddress)
      {
        Impl::Start(SlaveAddress);
      }

      static void Stop(void)
      {
        Impl::Stop();
      }

      static Status_t Tx(uint8_t *TxBuf, uint16_t TxLen)
      {
        return Impl::Tx(TxBuf, TxLen);
      }

      static Status_t Tx(uint8_t byte)
      {
        return Impl::Tx(byte);
      }

      static Status_t Tx(uint8_t byte1, uint8_t byte2)
      {
        return Impl::Tx(byte1, byte2);
      }

      static Status_t Tx(uint8_t SlaveAddress, uint8_t *TxBuf, uint16_t TxLen, uint8_t Stop = 1)
      {
        return Impl::Tx(SlaveAddress, TxBuf, TxLen, Stop);
      }

      static Status_t Rx(uint8_t SlaveAddress, uint8_t *RxBuf, uint8_t RxLen, uint8_t Stop = 1)
      {
        return Impl::Rx(SlaveAddress, RxBuf, RxLen, Stop);
      }

      static Status_t Xfer(uint8_t SlaveAddress, uint8_t *TxBuf, uint8_t TxLen, uint8_t *RxBuf = 0, uint8_t RxLen = 0, uint8_t Stop = 1)
      {
        return Impl::Xfer(SlaveAddress, TxBuf, TxLen, RxBuf, RxLen, Stop);
      }

      static Status_t SendSlaveAddress(uint8_t SlaveAddress, uint8_t Repeatedstart)
      {
        return Impl::SendSlaveAddress(SlaveAddress, Repeatedstart);
      }

      static void ScanBus(uint8_t *pFoundDevices, uint8_t size)
      {
        Impl::ScanBus(pFoundDevices, size);
      }

      static State_t GetState(void)
      {
        return Impl::GetState();
      }

      static Status_t Xfer(MasterTxn_t *pTransaction)
      {
        return Impl::Xfer(pTransaction);
      }

      static void IRQ_Handler(void)
      {
        Impl::IRQ_Handler();
      }
    };

  } // namespace i2c

} // namespace HAL

#endif // I2C_H