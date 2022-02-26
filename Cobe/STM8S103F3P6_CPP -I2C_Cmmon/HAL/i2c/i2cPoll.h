#ifndef I2C_POLL_H
#define I2C_POLL_H

#include "i2c_common.h"
#include "Gpio.h"
#include "GpioOut.h"
#include "GpioIn.h"

namespace HAL
{

    namespace i2c
    {
        static const uint16_t I2C_TIMEOUT = 5000U;

        template <i2c_t i2cx, Clk_t Clk>
        class Poll
        {

        protected:
            typedef I2C_TypeDef* I2c_t;
            static volatile State_t m_State;
            static volatile Status_t m_Status;

        public:

            static Status_t HwInit(void);

            static void ScanBus(uint8_t *pFoundDevices, uint8_t size);

            static Status_t Tx(uint8_t SlaveAddress, uint8_t *TxBuf, uint16_t TxLen, uint8_t Stop = 1);

            static Status_t Rx(uint8_t SlaveAddress, uint8_t *RxBuf, uint8_t RxLen, uint8_t Stop = 1);

            static Status_t XferPoll(uint8_t SlaveAddress, uint8_t *TxBuf, uint8_t TxLen, uint8_t *RxBuf = 0, uint8_t RxLen = 0, uint8_t Stop = 1); // 394 bytes -> Templates 311

            static Status_t SendSlaveAddress(uint8_t SlaveAddress, uint8_t Repeatedstart);

            static State_t GetState(void) { return m_State; }
            
             static void PinsInit(void)
            {
                Gpio::SetOutputMode(B4, Gpio::OD_FAST); //SCL = B4
                Gpio::SetOutputMode(B5, Gpio::OD_FAST); //SDA = B5
            }

            static void Start(uint8_t SlaveAddress)
            {
               SendSlaveAddress(SlaveAddress, 0);
            }
            
            static void Stop(void)
            {
                /* Generate Stop */
                GenerateStop();
            }

            static Status_t Tx(uint8_t *TxBuf, uint16_t TxLen)
            {
                if (!TxBuf)
                    return I2C_INVALID_PARAMS;

                while (TxLen)
                {
                    Tx(*TxBuf++);                    
                    TxLen--;
                }
                return I2C_OK;
            }

            static Status_t Tx(uint8_t byte)
            {
                /* Write data to DR */
                I2Cx->DR = byte;

                /* Wait until TXE flag is set */
                if (WaitOnFlag(&I2Cx->SR1, I2C_SR1_TXE, 0, I2C_TIMEOUT))
                {
                    I2C_LOG_STATES(I2C_LOG_TXE_TIMEOUT);
                    return I2C_TXE_TIMEOUT;
                }

                I2C_LOG_EVENTS(I2C_LOG_TXE);

                if (I2Cx->SR1 & I2C_SR1_BTF)
                {
                    I2C_LOG_EVENTS(I2C_LOG_BTF);
                }

                return I2C_OK;
            }

            static Status_t Tx(uint8_t byte1, uint8_t byte2)
            {
                if (Tx(byte1) == 0)
                {
                    return Tx(byte2);
                }
                return I2C_TXE_TIMEOUT;
            }            

            static void ClockEnable(void) { CLK->PCKENR1 |= ((uint8_t)CLK_PCKENR1_I2C); }

            static void ClockDisable(void) { CLK->PCKENR1 &= (uint8_t)(~(uint8_t)CLK_PCKENR1_I2C); }

            static void Enable(void) { I2Cx->CR1 |= I2C_CR1_PE; }

            static void Disable(void) { I2Cx->CR1 &= (uint8_t)(~I2C_CR1_PE); }

            static void GenerateStart(void) { I2Cx->CR2 |= I2C_CR2_START; }

            static void GenerateStop(void) { I2Cx->CR2 |= I2C_CR2_STOP; }

            static void EnableACK(void) { I2Cx->CR2 |= I2C_CR2_ACK; }

            static void DisableACK(void) { I2Cx->CR2 &= (uint8_t)(~I2C_CR2_ACK); }

            static void EnablePOS(void) { I2Cx->CR2 |= (uint8_t)I2C_CR2_POS; }

            static void DisablePOS(void) { I2Cx->CR2 &= (uint8_t)(~I2C_CR2_POS); }

            static void ClearAF(void) { I2Cx->SR2 &= (uint8_t)(~I2C_SR2_AF); }

            static void ClearARLO(void) { I2Cx->SR2 &= (uint8_t)(~I2C_SR2_ARLO); }

            static void ClearBERR(void) { I2Cx->SR2 &= (uint8_t)(~I2C_SR2_BERR); }

            static void ClearOVR(void) { I2Cx->SR1 &= (uint8_t)(~I2C_SR2_OVR); }

            static void Enable_EVT_BUF_ERR_Interrupt(void) { I2Cx->ITR = I2C_ITR_ITBUFEN | I2C_ITR_ITEVTEN | I2C_ITR_ITERREN; }

            static void Disable_EVT_BUF_ERR_Interrupt(void) { I2Cx->ITR &= (uint8_t) ~(I2C_ITR_ITBUFEN | I2C_ITR_ITEVTEN | I2C_ITR_ITERREN); }

            static void Enable_BUF_Interrupt(void) { I2Cx->ITR |= I2C_ITR_ITBUFEN; }

            static void Disable_BUF_Interrupt(void) { I2Cx->ITR &= (uint8_t)~I2C_ITR_ITBUFEN; }
            
            static void Softreset(void)
            {
                I2Cx->CR2 |= I2C_CR2_SWRST;
                I2Cx->CR2 &= (uint8_t)(~I2C_CR2_SWRST);
            }

            static void ClearADDR(void)
            {
                uint8_t dummy;
                dummy = I2Cx->SR1;
                dummy = I2Cx->SR3;
                dummy = dummy;
            }

            static void ClearSTOPF(void)
            {
                uint8_t reg;
                reg = I2Cx->SR1;
                I2Cx->CR2 = I2Cx->CR2;
                reg = reg;
            }           
        };

        template <i2c_t i2cx, Clk_t Clk>
        i2c::State_t i2c::Poll<i2cx, Clk>::m_State;

        template <i2c_t i2cx, Clk_t Clk>
        i2c::Status_t i2c::Poll<i2cx, Clk>::m_Status;

        template <i2c_t i2cx, Clk_t Clk>
        i2c::Status_t i2c::Poll<i2cx, Clk>::HwInit(void)
        {
          if(m_State != I2C_RESET) return I2C_OK;
          
            ClockEnable();

            I2C_Init(Clk, 0x08 << 1, I2C_DUTYCYCLE_2, I2C_ACK_NONE, I2C_ADDMODE_7BIT, I2C_MAX_INPUT_FREQ);

            PinsInit();

            //I2C_Cmd(ENABLE);
            Enable();
            
            m_State = I2C_READY;
              
            return I2C_OK;
        }

        template <i2c_t i2cx, Clk_t Clk>
        i2c::Status_t i2c::Poll<i2cx, Clk>::SendSlaveAddress(uint8_t SlaveAddress, uint8_t Repeatedstart)
        {
            /* To avoid the Busy Timeout here */
            /* During repeated start don't check the busy flag, as it will be always busy */
            if (!Repeatedstart)
            {
                /* Wait while BUSY flag is set */
                if (WaitOnFlag(&I2Cx->SR3, I2C_SR3_BUSY, I2C_SR3_BUSY, I2C_TIMEOUT))
                {
                    I2C_LOG_STATES(I2C_LOG_BUSY_TIMEOUT);
                    return I2C_BUSY_TIMEOUT;
                }
            }
            /* Disable Pos */
            DisablePOS();

            /* Enable Acknowledge, Generate Start */
            I2Cx->CR2 |= I2C_CR2_START | I2C_CR2_ACK;

            /* Wait while SB flag is 0 */
            if (WaitOnFlag(&I2Cx->SR1, I2C_SR1_SB, 0, I2C_TIMEOUT))
            {
                I2C_LOG_STATES(I2C_LOG_START_TIMEOUT);
                return I2C_START_TIMEOUT;
            }

            I2C_LOG_EVENTS(I2C_LOG_START);

            I2Cx->DR = SlaveAddress;

            /* Wait while ADDR flag is 0 */
            if (WaitOnFlag(&I2Cx->SR1, I2C_SR1_ADDR, 0, I2C_TIMEOUT))
            {
                /* if there is ack fail, clear the AF flag */
                if (I2Cx->SR2 & I2C_SR2_AF)
                {
                    ClearAF();

                    /* Generate Stop */
                    GenerateStop();

                    I2C_LOG_STATES(I2C_LOG_ACK_FAIL);

                    return I2C_ACK_FAIL;
                }

                I2C_LOG_STATES(I2C_LOG_ADDR_TIMEOUT);
                return I2C_ADDR_TIMEOUT;
            }
            I2C_LOG_EVENTS(I2C_LOG_ADDR);
            return I2C_OK;
        }

        template <i2c_t i2cx, Clk_t Clk>
        void i2c::Poll<i2cx, Clk>::ScanBus(uint8_t *pFoundDevices, uint8_t size)
        {
            uint8_t slave, i = 0;

            for (slave = 0; slave < 255; slave++)
            {
                if (SendSlaveAddress(slave & I2C_DIR_WRITE, 0) == I2C_OK)
                {
                    pFoundDevices[i++] = slave++;
                    GenerateStop();
                }

                if (i == size)
                    i = 0;
            }
        }

        template <i2c_t i2cx, Clk_t Clk>
        i2c::Status_t i2c::Poll<i2cx, Clk>::Tx(uint8_t SlaveAddress, uint8_t *TxBuf, uint16_t TxLen, uint8_t Stop)
        {
            if (!TxBuf)
                return I2C_INVALID_PARAMS;

            /* Send Slave address */
            if (SendSlaveAddress(SlaveAddress & I2C_DIR_WRITE, 0) != I2C_OK)
                return I2C_ADDR_TIMEOUT;

            /* Clear ADDR flag */
            ClearADDR();
            
           if( Tx(TxBuf,TxLen) != I2C_OK )
             return I2C_TXE_TIMEOUT;
            
            if (Stop)
            {
                /* Generate Stop */
                GenerateStop();
            }

            I2C_LOG_EVENTS(I2C_LOG_STOP);

            return I2C_OK;
        }

        template <i2c_t i2cx, Clk_t Clk>
        i2c::Status_t i2c::Poll<i2cx, Clk>::Rx(uint8_t SlaveAddress, uint8_t *RxBuf, uint8_t RxLen, uint8_t Stop)
        {
            if (!RxBuf)
                return I2C_INVALID_PARAMS;

            /* Send Slave address */
            if (SendSlaveAddress(SlaveAddress | I2C_DIR_READ, Stop) != I2C_OK)
                return I2C_ADDR_TIMEOUT;

            if (RxLen == 1)
            {
                /* Disable Acknowledge */
                DisableACK();

                /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                software sequence must complete before the current byte end of transfer */
                __disable_interrupt();

                /* Clear ADDR flag */
                ClearADDR();

                /* Generate Stop */
                GenerateStop();

                /* Re-enable IRQs */
                __enable_interrupt();

                I2C_LOG_STATES(I2C_LOG_RX_1);
            }
            else if (RxLen == 2)
            {
                /* Enable Pos */
                EnablePOS();

                /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                software sequence must complete before the current byte end of transfer */
                __disable_interrupt();

                /* Clear ADDR flag */
                ClearADDR();

                /* Disable Acknowledge */
                DisableACK();

                /* Re-enable IRQs */
                __enable_interrupt();

                I2C_LOG_STATES(I2C_LOG_RX_2);
            }
            else
            {
                /* Enable Acknowledge */
                EnableACK();

                /* Clear ADDR flag */
                ClearADDR();

                I2C_LOG_STATES(I2C_LOG_RX_GT_2);
            }

            while (RxLen > 0U)
            {
                if (RxLen <= 3U)
                {
                    /* One byte */
                    if (RxLen == 1U)
                    {
                        /* Wait until RXNE flag is set */
                        if (WaitOnFlag(&I2Cx->SR1, I2C_SR1_RXNE, 0, I2C_TIMEOUT))
                        {
                            I2C_LOG_STATES(I2C_LOG_RXNE_TIMEOUT);
                            return I2C_RXNE_TIMEOUT;
                        }

                        I2C_LOG_EVENTS(I2C_LOG_RXNE);

                        /* Read data from DR */
                        (*RxBuf) = I2Cx->DR;
                        RxLen--;
                        I2C_LOG_STATES(I2C_LOG_RX_1_DONE);
                    }
                    /* Two bytes */
                    else if (RxLen == 2U)
                    {
                        /* Wait until BTF flag is set */
                        if (WaitOnFlag(&I2Cx->SR1, I2C_SR1_BTF, 0, I2C_TIMEOUT))
                        {
                            I2C_LOG_STATES(I2C_LOG_BTF_TIMEOUT);
                            return I2C_BTF_TIMEOUT;
                        }

                        /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                        software sequence must complete before the current byte end of transfer */
                        __disable_interrupt();

                        /* Generate Stop */
                        GenerateStop();

                        /* Read data from DR */
                        (*RxBuf++) = I2Cx->DR;
                        RxLen--;

                        /* Re-enable IRQs */
                        __enable_interrupt();

                        /* Read data from DR */
                        (*RxBuf++) = I2Cx->DR;
                        RxLen--;

                        I2C_LOG_STATES(I2C_LOG_RX_2_DONE);
                    }
                    /* 3 Last bytes */
                    else
                    {
                        /* Wait until BTF flag is set */
                        if (WaitOnFlag(&I2Cx->SR1, I2C_SR1_BTF, 0, I2C_TIMEOUT))
                        {
                            I2C_LOG_STATES(I2C_LOG_BTF_TIMEOUT);
                            return I2C_BTF_TIMEOUT;
                        }

                        /* Disable Acknowledge */
                        DisableACK();

                        /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                        software sequence must complete before the current byte end of transfer */
                        __disable_interrupt();

                        /* Read data from DR */
                        (*RxBuf++) = I2Cx->DR;;
                        RxLen--;

                        /* Wait until BTF flag is set */
                        if (WaitOnFlag(&I2Cx->SR1, I2C_SR1_BTF, 0, I2C_TIMEOUT))
                        {
                            I2C_LOG_STATES(I2C_LOG_BTF_TIMEOUT);
                            return I2C_BTF_TIMEOUT;
                        }

                        /* Generate Stop */
                        GenerateStop();

                        /* Read data from DR */
                        (*RxBuf++) = I2Cx->DR;
                        RxLen--;

                        /* Re-enable IRQs */
                        __enable_interrupt();

                        /* Read data from DR */
                        (*RxBuf++) = I2Cx->DR;
                        RxLen--;

                        I2C_LOG_STATES(I2C_LOG_RX_3_DONE);
                    }
                }
                else
                {
                    /* Wait until RXNE flag is set */
                    if (WaitOnFlag(&I2Cx->SR1, I2C_SR1_RXNE, 0, I2C_TIMEOUT))
                    {
                        I2C_LOG_STATES(I2C_LOG_RXNE_TIMEOUT);
                        return I2C_RXNE_TIMEOUT;
                    }

                    I2C_LOG_EVENTS(I2C_LOG_RXNE);

                    /* Read data from DR */
                    (*RxBuf++) = I2Cx->DR;
                    RxLen--;

                    if (WaitOnFlag(&I2Cx->SR1, I2C_SR1_BTF, 0, I2C_TIMEOUT))
                    {
                        /* Read data from DR */
                        (*RxBuf++) = I2Cx->DR;
                        RxLen--;
                        I2C_LOG_EVENTS(I2C_LOG_BTF);
                    }
                }
            }

            I2C_LOG_STATES(I2C_LOG_RX_DONE);
            return I2C_OK;
        }

        template <i2c_t i2cx, Clk_t Clk>
        i2c::Status_t i2c::Poll<i2cx, Clk>::XferPoll(uint8_t SlaveAddress, uint8_t *TxBuf, uint8_t TxLen, uint8_t *RxBuf, uint8_t RxLen, uint8_t stop) // 394 bytes -> Templates 311
        {
            Status_t Status = I2C_OK;

            if (TxLen != 0)
            {
                Status = Tx(SlaveAddress, TxBuf, TxLen, stop);

                if ( Status != I2C_OK)
                {
                    return Status;
                }
            }

            if(RxLen != 0)
            {
                Status = Rx(SlaveAddress, RxBuf, RxLen, stop);

                if ( Status != I2C_OK)
                {
                    return Status;
                }
            }

            return Status;         
        }
        
    } // namespace i2c

} // namespace HAL

#endif // I2C_POLL_H