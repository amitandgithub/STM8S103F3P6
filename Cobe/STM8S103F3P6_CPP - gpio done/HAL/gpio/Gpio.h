#ifndef _GPIO_H_
#define _GPIO_H_

#include "system.h"

namespace HAL
{
    class Gpio
    {
    public:
        typedef enum
        {
            PORTA = GPIOA_BaseAddress,
            PORTB = GPIOB_BaseAddress,
            PORTC = GPIOC_BaseAddress,
            PORTD = GPIOD_BaseAddress,
            PORTE = GPIOE_BaseAddress
        } Port_t;

        typedef enum
        {
            PIN_0 = 1 << 0U,
            PIN_1 = 1 << 1U,
            PIN_2 = 1 << 2U,
            PIN_3 = 1 << 3U,
            PIN_4 = 1 << 4U,
            PIN_5 = 1 << 5U,
            PIN_6 = 1 << 6U,
            PIN_7 = 1 << 7U
        } Pin_t;

        typedef enum
        {
            ANALOG,
            FL,
            PU,
            FL_INT,
            PU_INT,
            OD,
            OD_PUSH_PULL,
            OD_FAST,
            OD_PUSH_PULL_FAST,
        } Mode_t;

        typedef enum
        {
            FALLING_LOW,
            RISING,
            FALLING,
            RISING_FALLING,
            NO_INTERRUPT
        } IntEdge_t;

        typedef void (*GpioFptr_t)(void);

        static void SetOutputMode(Port_t Port, Pin_t Pin, Mode_t Mode)
        {
            ((GPIO_TypeDef *)Port)->DDR |= Pin; // Output
            
            switch (Mode)
            {
            case OD:
                ((GPIO_TypeDef *)Port)->CR1 &= ~Pin; // 0
                ((GPIO_TypeDef *)Port)->CR2 &= ~Pin; // 0
                break;
            case OD_PUSH_PULL:
                ((GPIO_TypeDef *)Port)->CR1 |= Pin;  // 1
                ((GPIO_TypeDef *)Port)->CR2 &= ~Pin; // 0
                break;
            case OD_FAST:
                ((GPIO_TypeDef *)Port)->CR1 &= ~Pin; // 0
                ((GPIO_TypeDef *)Port)->CR2 |= Pin;  // 1
                break;
            case OD_PUSH_PULL_FAST:
                ((GPIO_TypeDef *)Port)->CR1 |= Pin; // 1
                ((GPIO_TypeDef *)Port)->CR2 |= Pin; // 1
            default:
                break;
            }
        }

        static void SetInputMode(Port_t Port, Pin_t Pin, Mode_t Mode)
        {
          ((GPIO_TypeDef *)Port)->DDR &= ~Pin; // Input
            switch (Mode)
            {
            case ANALOG:
            case FL:
                ((GPIO_TypeDef *)Port)->CR1 &= ~Pin; // 0
                ((GPIO_TypeDef *)Port)->CR2 &= ~Pin; // 0
                break;
            case PU:
                ((GPIO_TypeDef *)Port)->CR1 |= Pin;  // 1
                ((GPIO_TypeDef *)Port)->CR2 &= ~Pin; // 0
                break;
            case FL_INT:
                ((GPIO_TypeDef *)Port)->CR1 &= ~Pin; // 0
                ((GPIO_TypeDef *)Port)->CR2 |= Pin;  // 1
                break;
            case PU_INT:
                ((GPIO_TypeDef *)Port)->CR1 |= Pin;  // 1
                ((GPIO_TypeDef *)Port)->CR2 |= Pin;  // 1
                break;
            default:
                break;
            }
        }

        static void SetInterrupt(Port_t Port, IntEdge_t Edge)
        {
            switch (Port)
            {
            case PORTA:
                EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PAIS);
                EXTI->CR1 |= (uint8_t)((Edge) << 0);
                break;
            case PORTB:
                EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PBIS);
                EXTI->CR1 |= (uint8_t)((Edge) << 2);
                break;
            case PORTC:
                EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PCIS);
                EXTI->CR1 |= (uint8_t)((Edge) << 4);
                break;
            case PORTD:
                EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PDIS);
                EXTI->CR1 |= (uint8_t)((Edge) << 6);
                break;

            default:
                break;
            }
        }
        
        static void Dummy_ISR(void)
        {
          
        }
    };

    /* Port A Pin definitions */
#define A0 Gpio::PORTA, Gpio::PIN_0
#define A1 Gpio::PORTA, Gpio::PIN_1
#define A2 Gpio::PORTA, Gpio::PIN_2
#define A3 Gpio::PORTA, Gpio::PIN_3
#define A4 Gpio::PORTA, Gpio::PIN_4
#define A5 Gpio::PORTA, Gpio::PIN_5
#define A6 Gpio::PORTA, Gpio::PIN_6
#define A7 Gpio::PORTA, Gpio::PIN_7

/* Gpio::PORT B Gpio::PIN definitions */
#define B0 Gpio::PORTB, Gpio::PIN_0
#define B1 Gpio::PORTB, Gpio::PIN_1
#define B2 Gpio::PORTB, Gpio::PIN_2
#define B3 Gpio::PORTB, Gpio::PIN_3
#define B4 Gpio::PORTB, Gpio::PIN_4
#define B5 Gpio::PORTB, Gpio::PIN_5
#define B6 Gpio::PORTB, Gpio::PIN_6
#define B7 Gpio::PORTB, Gpio::PIN_7

/* Gpio::PORT C Gpio::PIN definitions */
#define C0 Gpio::PORTC, Gpio::PIN_0
#define C1 Gpio::PORTC, Gpio::PIN_1
#define C2 Gpio::PORTC, Gpio::PIN_2
#define C3 Gpio::PORTC, Gpio::PIN_3
#define C4 Gpio::PORTC, Gpio::PIN_4
#define C5 Gpio::PORTC, Gpio::PIN_5
#define C6 Gpio::PORTC, Gpio::PIN_6
#define C7 Gpio::PORTC, Gpio::PIN_7

/* Gpio::PORT D Gpio::PIN definitions */
#define D0 Gpio::PORTD, Gpio::PIN_0
#define D1 Gpio::PORTD, Gpio::PIN_1
#define D2 Gpio::PORTD, Gpio::PIN_2
#define D3 Gpio::PORTD, Gpio::PIN_3
#define D4 Gpio::PORTD, Gpio::PIN_4
#define D5 Gpio::PORTD, Gpio::PIN_5
#define D6 Gpio::PORTD, Gpio::PIN_6
#define D7 Gpio::PORTD, Gpio::PIN_7

}
#endif // _GPIO_H_