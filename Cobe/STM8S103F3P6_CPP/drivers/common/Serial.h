/******************
** CLASS: SerialInterface
**
** DESCRIPTION:
**  common SerialInterface class for I2C, SPI, UART etc
**
** CREATED: 4 March 2021, by Amit Chaudhary
**
** FILE: SerialInterface.h
**
******************/
#ifndef SerialInterface_h
#define SerialInterface_h

#include<cstdint>

template < typename Implementation_t >
struct SerialInterface
{

typedef Implementation_t::Base_t Base_t;
typedef Implementation_t::Callback_t Callback_t;
typedef Implementation_t::Status_t Status_t;

static Status_t Tx(Base_t* Buf, Base_t Len)
{
   return Implementation_t::Tx(Buf, Len);
}

static Status_t Rx(Base_t* Buf, Base_t Len)
{
   return Implementation_t::Rx(Buf, Len);
}

static Status_t Tx(Base_t* Buf, Base_t Len, Callback_t Callback)
{
   return Implementation_t::Tx(Buf, Len, Callback);
}

static Status_t Rx(Base_t* Buf, Base_t Len,  Callback_t Callback)
{
   return Implementation_t::Rx(Buf, Len, Callback);
}

static void Isr(void)
{
    Implementation_t::Isr();
}

}





























#endif // SerialInterface_h