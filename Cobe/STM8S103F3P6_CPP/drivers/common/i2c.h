/******************
** CLASS: i2cDevice
**
** DESCRIPTION:
**  common i2cDevice class for I2C, SPI, UART etc
**
** CREATED: 4 March 2021, by Amit Chaudhary
**
** FILE: i2cDevice.h
**
******************/
#ifndef i2cDevice_h
#define i2cDevice_h

#include<cstdint>

class i2c
{

    typedef uint8_t Base_t;
    typedef uint8_t Callback_t;
    typedef uint8_t Status_t;

    static Status_t TxPoll(Base_t *Buf, Base_t Len)
    {
        return 0;
    }

    static Status_t RxPoll(Base_t *Buf, Base_t Len)
    {
        return 0;
    }

    static Status_t TxIntr(Base_t *Buf, Base_t Len, Callback_t Callback)
    {
        return 0;
    }

    static Status_t RxIntr(Base_t *Buf, Base_t Len, Callback_t Callback)
    {
        return 0;
    }

    static Status_t TxDMA(Base_t *Buf, Base_t Len, Callback_t Callback)
    {
        return 0;
    }

    static Status_t RxDMA(Base_t *Buf, Base_t Len, Callback_t Callback)
    {
        return 0;
    }
};

class spi
{

    typedef uint8_t Base_t;
    typedef uint8_t Callback_t;
    typedef uint8_t Status_t;

    static Status_t TxPoll(Base_t *Buf, Base_t Len)
    {
        return 0;
    }

    static Status_t RxPoll(Base_t *Buf, Base_t Len)
    {
        return 0;
    }

    static Status_t TxIntr(Base_t *Buf, Base_t Len, Callback_t Callback)
    {
        return 0;
    }

    static Status_t RxIntr(Base_t *Buf, Base_t Len, Callback_t Callback)
    {
        return 0;
    }

    static Status_t TxDMA(Base_t *Buf, Base_t Len, Callback_t Callback)
    {
        return 0;
    }

    static Status_t RxDMA(Base_t *Buf, Base_t Len, Callback_t Callback)
    {
        return 0;
    }
};

class uart
{

    typedef uint8_t Base_t;
    typedef uint8_t Callback_t;
    typedef uint8_t Status_t;

    static Status_t TxPoll(Base_t *Buf, Base_t Len)
    {
        return 0;
    }

    static Status_t RxPoll(Base_t *Buf, Base_t Len)
    {
        return 0;
    }

    static Status_t TxIntr(Base_t *Buf, Base_t Len, Callback_t Callback)
    {
        return 0;
    }

    static Status_t RxIntr(Base_t *Buf, Base_t Len, Callback_t Callback)
    {
        return 0;
    }

    static Status_t TxDMA(Base_t *Buf, Base_t Len, Callback_t Callback)
    {
        return 0;
    }

    static Status_t RxDMA(Base_t *Buf, Base_t Len, Callback_t Callback)
    {
        return 0;
    }
};





#endif // i2cDevice_h