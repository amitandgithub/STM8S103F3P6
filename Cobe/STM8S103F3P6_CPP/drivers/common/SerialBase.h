/******************
** CLASS: Serial
**
** DESCRIPTION:
**  common Serial class for I2C, SPI, UART etc
**
** CREATED: 7 March 2021, by Amit Chaudhary
**
** FILE: Serial.h
**
******************/
#ifndef Serial_h
#define Serial_h

#include <cstdint>

namespace HAL
{

   class Serial
   {

      typedef enum
      {
         OK,
         BUSY_TIMEOUT,
         START_TIMEOUT,
         ADDR_TIMEOUT,
         BTF_TIMEOUT,
         RXNE_TIMEOUT,
         TXE_TIMEOUT,
         STOP_TIMEOUT,
         STOP_DETECTED,
         ACK_FAIL,
         ARB_LOST,
         INTR_PENDING,
         BUS_ERROR,
         DATA_OVR,
         INVALID_PARAMS,
         XFER_DONE,
         SLAVE_TX_DONE,
         SLAVE_RX_DONE,
         SLAVE_RX_DONE_WITH_NACK,
         TXN_POSTED,
         TXN_QUEUE_ERROR,
      } Status_t;
   };

}

#endif // Serial_h