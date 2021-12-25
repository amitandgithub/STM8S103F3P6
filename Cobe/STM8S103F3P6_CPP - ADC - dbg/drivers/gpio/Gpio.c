
#include"system.h"

/* Array of EXTI interrupts*/
GpioFptr_t EXTI_ISRs[8] = {Dummy_ISR,Dummy_ISR,Dummy_ISR,Dummy_ISR,Dummy_ISR,Dummy_ISR,Dummy_ISR,Dummy_ISR};

void Gpio_HwInit(Port_t port,Pin_t pin, Mode_t mode)
{
  GPIO_Init(port,pin,mode);
}

void GpioSetInterrupt(Port_t port,Pin_t pin,Trigger_t trigger, GpioFptr_t fptr)
{
  switch(pin)
  {
  case GPIO_Pin_0:
    EXTI->CR1 &= (uint8_t)(~EXTI_CR1_P0IS);
    EXTI->CR1 |= (uint8_t)((trigger) << 0);
    EXTI_ISRs[0] = fptr;
    break;
  case GPIO_Pin_1:
    EXTI->CR1 &= (uint8_t)(~EXTI_CR1_P1IS);
    EXTI->CR1 |= (uint8_t)((trigger) << 2);
    EXTI_ISRs[1] = fptr;
    break;
  case GPIO_Pin_2:
    EXTI->CR1 &= (uint8_t)(~EXTI_CR1_P2IS);
    EXTI->CR1 |= (uint8_t)((trigger) << 4);
    EXTI_ISRs[2] = fptr;
    break;
  case GPIO_Pin_3:
    EXTI->CR1 &= (uint8_t)(~EXTI_CR1_P3IS);
    EXTI->CR1 |= (uint8_t)((trigger) << 6);
    EXTI_ISRs[3] = fptr;
    break;
  case GPIO_Pin_4:
    EXTI->CR2 &= (uint8_t)(~EXTI_CR1_P0IS);
    EXTI->CR2 |= (uint8_t)((trigger) << 0);
    EXTI_ISRs[4] = fptr;
    break;
  case GPIO_Pin_5:
    EXTI->CR2 &= (uint8_t)(~EXTI_CR1_P1IS);
    EXTI->CR2 |= (uint8_t)((trigger) << 2);
    EXTI_ISRs[5] = fptr;
    break;
  case GPIO_Pin_6:
    EXTI->CR2 &= (uint8_t)(~EXTI_CR1_P2IS);
    EXTI->CR2 |= (uint8_t)((trigger) << 4);
    EXTI_ISRs[6] = fptr;
    break;
  case GPIO_Pin_7:
    EXTI->CR2 &= (uint8_t)(~EXTI_CR1_P3IS);
    EXTI->CR2 |= (uint8_t)((trigger) << 6);
    EXTI_ISRs[7] = fptr;
  default: break;
  }
  
  if(port == GPIOB)
  {
    EXTI->CR3 &= 0xFC;
    EXTI->CR3 |= (uint8_t)((trigger) << 0);
  }
  else 
  {
    EXTI->CR3 &= 0xF3;
    EXTI->CR3 |= (uint8_t)((trigger) << 2);
  }
  
}

void Gpio0SetInterrupt(Trigger_t trigger, GpioFptr_t fptr)
{
  EXTI->CR1 &= (uint8_t)(~EXTI_CR1_P0IS);
  EXTI->CR1 |= (uint8_t)((trigger) << 0);
  EXTI_ISRs[0] = fptr;
}

void Gpio1SetInterrupt(Trigger_t trigger, GpioFptr_t fptr)
{
  EXTI->CR1 &= (uint8_t)(~EXTI_CR1_P1IS);
  EXTI->CR1 |= (uint8_t)((trigger) << 2);
  EXTI_ISRs[1] = fptr;
}

void Gpio2SetInterrupt(Trigger_t trigger, GpioFptr_t fptr)
{
  EXTI->CR1 &= (uint8_t)(~EXTI_CR1_P2IS);
  EXTI->CR1 |= (uint8_t)((trigger) << 4);
  EXTI_ISRs[2] = fptr;
}
void Gpio3SetInterrupt(Trigger_t trigger, GpioFptr_t fptr)
{
  EXTI->CR1 &= (uint8_t)(~EXTI_CR1_P3IS);
  EXTI->CR1 |= (uint8_t)((trigger) << 6);
  EXTI_ISRs[3] = fptr;
}
void Gpio4SetInterrupt(Trigger_t trigger, GpioFptr_t fptr)
{
  EXTI->CR2 &= (uint8_t)(~EXTI_CR1_P0IS);
  EXTI->CR2 |= (uint8_t)((trigger) << 0);
  EXTI_ISRs[4] = fptr;
}
void Gpio5SetInterrupt(Trigger_t trigger, GpioFptr_t fptr)
{
  EXTI->CR2 &= (uint8_t)(~EXTI_CR1_P1IS);
  EXTI->CR2 |= (uint8_t)((trigger) << 2);
  EXTI_ISRs[5] = fptr;
}
void Gpio6SetInterrupt(Trigger_t trigger, GpioFptr_t fptr)
{
  EXTI->CR2 &= (uint8_t)(~EXTI_CR1_P2IS);
  EXTI->CR2 |= (uint8_t)((trigger) << 4);
  EXTI_ISRs[6] = fptr;
}
void Gpio7SetInterrupt(Trigger_t trigger, GpioFptr_t fptr)
{
  EXTI->CR2 &= (uint8_t)(~EXTI_CR1_P3IS);
  EXTI->CR2 |= (uint8_t)((trigger) << 6);
  EXTI_ISRs[7] = fptr;
}

void SetGpioMode(Port_t port,Pin_t pin,GpioMode_t mode)
{
  switch(mode)
  {
  case INPUT_FLOATING:
    port->DDR &= ~(uint8_t)(pin); // 0
    port->CR1 &= ~(uint8_t)(pin); // 0
    port->CR2 &= ~(uint8_t)(pin); // 0
    break;
  case INPUT_PULLUP:
    port->DDR &= ~(uint8_t)(pin);  // 0
    port->CR1 |=  (uint8_t)(pin);  // 1
    port->CR2 &= ~(uint8_t)(pin);  // 0
    break;
  case INPUT_FLOATING_INTERRUPT:
    port->DDR &= ~(uint8_t)(pin); // 0
    port->CR1 &= ~(uint8_t)(pin); // 0
    port->CR2 |=  (uint8_t)(pin); // 1
    break;
  case INPUT_PULLUP_INTERRUPT:
    port->DDR &= ~(uint8_t)(pin); // 0
    port->CR1 |=  (uint8_t)(pin); // 1
    port->CR2 |=  (uint8_t)(pin); // 1
    break;
  case OUTPUT_OPEN_DRAIN:
    port->DDR |=  (uint8_t)(pin); // 1
    port->CR1 &= ~(uint8_t)(pin); // 0
    port->CR2 &= ~(uint8_t)(pin); // 0
    break;
  case OUTPUT_OPEN_DRAIN_PUSH_PULL:
    port->DDR |=  (uint8_t)(pin); // 1
    port->CR1 |=  (uint8_t)(pin); // 1
    port->CR2 &= ~(uint8_t)(pin); // 0
    break;
  case OUTPUT_OPEN_DRAIN_FAST:
    port->DDR |=  (uint8_t)(pin); // 1
    port->CR1 &= ~(uint8_t)(pin); // 0
    port->CR2 |=  (uint8_t)(pin); // 1
    break;
  case OUTPUT_OPEN_DRAIN_PUSH_PULL_FAST:
    port->DDR |=  (uint8_t)(pin); // 1
    port->CR1 |=  (uint8_t)(pin); // 1
    port->CR2 |=  (uint8_t)(pin); // 1
  default: break;
  
  }
  
}

void GpioSetInputFloating(Port_t port,Pin_t pin)
{
  port->DDR &= ~(uint8_t)(pin); // 0
  port->CR1 &= ~(uint8_t)(pin); // 0
  port->CR2 &= ~(uint8_t)(pin); // 0
}

void GpioSetInputPullup(Port_t port,Pin_t pin)
{
  port->DDR &= ~(uint8_t)(pin);  // 0
  port->CR1 |=  (uint8_t)(pin);  // 1
  port->CR2 &= ~(uint8_t)(pin);  // 0
}

void GpioSetInputFloatingInterrupt(Port_t port,Pin_t pin)
{
  port->DDR &= ~(uint8_t)(pin); // 0
  port->CR1 &= ~(uint8_t)(pin); // 0
  port->CR2 |=  (uint8_t)(pin); // 1
}

void GpioSetInputPullupInterrupt(Port_t port,Pin_t pin)
{
  port->DDR &= ~(uint8_t)(pin); // 0
  port->CR1 |=  (uint8_t)(pin); // 1
  port->CR2 |=  (uint8_t)(pin); // 1
}

void GpioSetOutputOpenDrain(Port_t port,Pin_t pin)
{
  port->DDR |=  (uint8_t)(pin); // 1
  port->CR1 &= ~(uint8_t)(pin); // 0
  port->CR2 &= ~(uint8_t)(pin); // 0
}

void GpioSetOutputOpenDrainPushPull(Port_t port,Pin_t pin)
{
  port->DDR |=  (uint8_t)(pin); // 1
  port->CR1 |=  (uint8_t)(pin); // 1
  port->CR2 &= ~(uint8_t)(pin); // 0
}

void GpioSetOutputOpenDrainFast(Port_t port,Pin_t pin)
{
  port->DDR |=  (uint8_t)(pin); // 1
  port->CR1 &= ~(uint8_t)(pin); // 0
  port->CR2 |=  (uint8_t)(pin); // 1
}

void GpioSetOutputOpenDrainPushPullFast(Port_t port,Pin_t pin)
{
  port->DDR |=  (uint8_t)(pin); // 1
  port->CR1 |=  (uint8_t)(pin); // 1
  port->CR2 |=  (uint8_t)(pin); // 1
}

void Dummy_ISR(){}