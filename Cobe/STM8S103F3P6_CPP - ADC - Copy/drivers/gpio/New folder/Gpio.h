

/******************
** FILE: Gpio.h
**
** DESCRIPTION:
**  Gpio related functionality
**
** CREATED: 28/7/2019, by Amit Chaudhary
******************/
#ifndef Gpio_h
#define Gpio_h

#include"system.h"

typedef enum
{
  INPUT_FLOATING,
  INPUT_PULLUP,
  INPUT_FLOATING_INTERRUPT,
  INPUT_PULLUP_INTERRUPT,
  OUTPUT_OPEN_DRAIN,
  OUTPUT_OPEN_DRAIN_PUSH_PULL,
  OUTPUT_OPEN_DRAIN_FAST,
  OUTPUT_OPEN_DRAIN_PUSH_PULL_FAST,
}GpioMode_t;

typedef enum
{
  FALLING_LOW,
  RISING,
  FALLING,
  RISING_FALLING
}Trigger_t;

typedef void (*GpioFptr_t)(void); 
typedef GPIO_TypeDef*       Port_t;
typedef GPIO_Pin_TypeDef    Pin_t ;
typedef GPIO_Mode_TypeDef   Mode_t ;

void Gpio_HwInit(Port_t port,Pin_t pin, Mode_t mode);
void SetGpioMode(Port_t port,Pin_t pin,GpioMode_t mode);
void Gpio_On(Port_t port,Pin_t pin);
void Gpio_Off(Port_t port,Pin_t pin);
void Gpio_Toggle(Port_t port,Pin_t pin);

void GpioSetInterrupt(Port_t port,Pin_t pin,Trigger_t trigger, GpioFptr_t fptr);

void GpioSetInputFloating(Port_t port,Pin_t pin);
void GpioSetInputPullup(Port_t port,Pin_t pin);
void GpioSetInputFloatingInterrupt(Port_t port,Pin_t pin);
void GpioSetInputPullupInterrupt(Port_t port,Pin_t pin);
void GpioSetOutputOpenDrain(Port_t port,Pin_t pin);
void GpioSetOutputOpenDrainPushPull(Port_t port,Pin_t pin);
void GpioSetOutputOpenDrainFast(Port_t port,Pin_t pin);
void GpioSetOutputOpenDrainPushPullFast(Port_t port,Pin_t pin);

void Dummy_ISR();

void Gpio0SetInterrupt(Trigger_t trigger, GpioFptr_t fptr);
void Gpio1SetInterrupt(Trigger_t trigger, GpioFptr_t fptr);
void Gpio2SetInterrupt(Trigger_t trigger, GpioFptr_t fptr);
void Gpio3SetInterrupt(Trigger_t trigger, GpioFptr_t fptr);
void Gpio4SetInterrupt(Trigger_t trigger, GpioFptr_t fptr);
void Gpio5SetInterrupt(Trigger_t trigger, GpioFptr_t fptr);
void Gpio6SetInterrupt(Trigger_t trigger, GpioFptr_t fptr);
void Gpio7SetInterrupt(Trigger_t trigger, GpioFptr_t fptr);

/* Port A pin definitions */
#define A0 GPIOA,GPIO_Pin_0
#define A1 GPIOA,GPIO_Pin_1
#define A2 GPIOA,GPIO_Pin_2
#define A3 GPIOA,GPIO_Pin_3
#define A4 GPIOA,GPIO_Pin_4
#define A5 GPIOA,GPIO_Pin_5
#define A6 GPIOA,GPIO_Pin_6
#define A7 GPIOA,GPIO_Pin_7

/* Port B pin definitions */
#define B0 GPIOB,GPIO_Pin_0
#define B1 GPIOB,GPIO_Pin_1
#define B2 GPIOB,GPIO_Pin_2
#define B3 GPIOB,GPIO_Pin_3
#define B4 GPIOB,GPIO_Pin_4
#define B5 GPIOB,GPIO_Pin_5
#define B6 GPIOB,GPIO_Pin_6
#define B7 GPIOB,GPIO_Pin_7

/* Port C pin definitions */
#define C0 GPIOC,GPIO_Pin_0
#define C1 GPIOC,GPIO_Pin_1
#define C2 GPIOC,GPIO_Pin_2
#define C3 GPIOC,GPIO_Pin_3
#define C4 GPIOC,GPIO_Pin_4
#define C5 GPIOC,GPIO_Pin_5
#define C6 GPIOC,GPIO_Pin_6
#define C7 GPIOC,GPIO_Pin_7

/* Port D pin definitions */
#define D0 GPIOD,GPIO_Pin_0
#define D1 GPIOD,GPIO_Pin_1
#define D2 GPIOD,GPIO_Pin_2
#define D3 GPIOD,GPIO_Pin_3
#define D4 GPIOD,GPIO_Pin_4
#define D5 GPIOD,GPIO_Pin_5
#define D6 GPIOD,GPIO_Pin_6
#define D7 GPIOD,GPIO_Pin_7

static inline
void Gpio_On(Port_t port,Pin_t pin)
{
  port->ODR |= pin;
}

static inline
void Gpio_Off(Port_t port,Pin_t pin)
{
  port->ODR &= (uint8_t)(~pin);
}

static inline
void Gpio_Toggle(Port_t port,Pin_t pin)
{
  port->ODR ^= pin;
}

#endif