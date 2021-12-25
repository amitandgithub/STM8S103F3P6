/******************
** CLASS: Templates
**
** DESCRIPTION:
**  Templates functionality
**
** CREATED: 06-Jan-2021, by Amit Chaudhary
**
** FILE: Templates.h
**
******************/
#ifndef Templates_h
#define Templates_h
#include"System.h"

template <typename T>
struct NumericalFunctions
{
    void scale(uint8_t multiplicator)
    {
        T& underlying = static_cast<T&>(*this);
        underlying.setValue(underlying.getValue() * multiplicator);
    }
    void square()
    {
        T& underlying = static_cast<T&>(*this);
        underlying.setValue(underlying.getValue() * underlying.getValue());
    }
    void setToOpposite()
    {
        scale(-1);
    };
};

class Sensitivity : public NumericalFunctions<Sensitivity>
{
  uint8_t Val;
public:
    uint8_t getValue() const
    {
      return Val;
    }
    void setValue(uint8_t value)
    {
      Val = value;
    }
    // rest of the sensitivity's rich interface...
};


template <typename T>
struct SerialPoll
{
    void Tx(uint8_t* Buf, uint8_t len)
    {
        T& underlying = static_cast<T&>(*this);
        underlying.tx(Buf,len);
    }
    
    void Rx(uint8_t* Buf, uint8_t len)
    {
        T& underlying = static_cast<T&>(*this);
        underlying.rx(Buf,len);
    }
};


template <typename T>
struct Interrupt
{
    void IRQ(void)
    {
        T& underlying = static_cast<T&>(*this);
        underlying.IRQ_Handler();
    }
};

class I2C_Device : public Interrupt<I2C_Device>
{
  uint8_t* Val;
  volatile uint8_t Len;
  
public:
  
  void IRQ_Handler(void)
  {
    Len = *Val + 32;
    Len++;
  }
  
};

class SpiDevice : public SerialPoll<SpiDevice>
{
  uint8_t* Val;
  uint8_t Len;
public:
    void tx(uint8_t* Buf, uint8_t len)
    {
      Val = Buf;
      Len = len;
    }
    void rx(uint8_t* Buf, uint8_t len)
    {
      Val = Buf + 10;
      Len = len + 8;
    }
    
    uint8_t Get(void)
    {
      return Len;
    }   
    
    void Set(uint8_t len)
    {
      Len = len;
    }
    
    // rest of the sensitivity's rich interface...
};
uint8_t Buff = 64;
void Templates_Test(void)
{
  volatile uint8_t len;
  SpiDevice SpiDriver;
  I2C_Device I2C_Device1;
  
  len = SpiDriver.Get()*10;
  SpiDriver.Set(len);
  SpiDriver.Tx(&Buff,len);
  SpiDriver.Rx(&Buff,len);
  I2C_Device1.IRQ();

}





template <typename T>
struct IRQHandler
{
  void Execute(void)
  {
    T& underlying = static_cast<T&>(*this);
    underlying.ISR();
  }
};




#endif // Templates_h