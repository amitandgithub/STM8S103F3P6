#ifndef AT24Cxx_H
#define AT24Cxx_H

#include "system.h"

using namespace HAL;

namespace Devices
{
  
    /* 
    AT24C128 Pin map
    1 A0-------VCC  8
    2 A1-------WP   7
    3 NC-------SCL  6
    4 GND------SDA  5
    */
    template <typename I2c, uint8_t Device_Address, uint32_t size, uint8_t PageSize>
    class AT24Cxx
    {
    public:
        static const uint8_t  ZeroBytes = 24;
        static const uint32_t Bytes = size;        
        static const uint16_t Pages = size / PageSize;
        static const uint8_t Write_Delay_ms = 5U;  
        static const uint8_t READ = 0;
        static const uint8_t WRITE = 1;        

        static i2c::MasterTxn_t Transaction;

        static uint8_t byte_Read(uint16_t address)
        {
            uint8_t data;
            I2c::XferPoll(Device_Address, (uint8_t*)&address, sizeof(uint16_t), &data, 1, 0);
            return data;
        }

        static void byte_Write(uint16_t address, uint8_t data)
        {
            // Send Device address
            I2c::StartWrite(Device_Address);

            // Send memory address
            I2c::Tx(address>>8U,address & 0xff);

            I2c::Tx(data);

            I2c::Stop();

            delay_ms(Write_Delay_ms);
        }
        
        static void Write(uint16_t page_address, uint8_t* buf, int16_t len)
        {
          Read_Write(page_address,buf, len,WRITE);
        }

        static void Read(uint16_t page_address, uint8_t *buf, uint16_t len)
        {
          Read_Write(page_address,buf, len,READ);
        }

        static void Read_Write(uint16_t page_address, uint8_t *buf, uint16_t len, uint8_t RW)
        {
            uint8_t Page_len = PageSize - page_address % PageSize;

            Page_len = min(Page_len, (uint8_t)len);

            while (len > 0)
            {
              if(RW == READ)
              {
                page_Read(page_address, buf, Page_len);
              }
              else
              {
                page_Write(page_address,buf, Page_len);
              }

                page_address = page_address + Page_len;
                buf = buf + Page_len;
                len = len - Page_len;

                Page_len = min(PageSize, (uint8_t)len);
            }
        }       
        
        static void page_Read(uint16_t page_address, uint8_t data[], uint8_t len)
        {
#if 0 // use XferPoll or cmbination of Tx and Rx
            I2c::Xfer(Device_Address, (uint8_t*)&page_address, sizeof(uint16_t), data, len);
#else
          I2c::Tx(Device_Address, (uint8_t*)&page_address, sizeof(uint16_t));
          I2c::Rx(Device_Address, data, len);
#endif
        }       
        
        static void page_Write(uint16_t page_address, uint8_t data[], uint8_t len)
        {
            I2c::Tx(Device_Address, (uint8_t*)&page_address, sizeof(uint16_t),0);
            //delay_ms(Write_Delay_ms);
            
             // Send data
            I2c::Tx(data,len);
            
            // Page write done
            I2c::Stop();
            
            // EEPROM write delay
            delay_ms(Write_Delay_ms);
        }    

        static void page_Write(uint8_t data[], uint8_t len)
        {
            //I2c::XferPoll(Device_Address, data, len);
            I2c::Tx(Device_Address, data, len);
            
            // EEPROM write delay
            delay_ms(Write_Delay_ms);
        } 
        
        static void page_Read_Intr(uint16_t page_address, uint8_t data[], uint8_t len)
        {
            Transaction.SlaveAddress = Device_Address;
            Transaction.RepeatedStart = 1;
            Transaction.TxBuf = (uint8_t*)&page_address;
            Transaction.TxLen = sizeof(uint16_t);
            Transaction.RxBuf = data;
            Transaction.RxLen = len;
            Transaction.XferDoneCallback = 0;//PacketHandler;

            I2c::Xfer(&Transaction);
            while(I2c::GetState() != i2c::I2C_READY);
        }
        
        static void page_Write_Intr(uint8_t data[], uint8_t len)
        {
            Transaction.SlaveAddress = Device_Address;
            Transaction.RepeatedStart = 1;
            Transaction.TxBuf = data;
            Transaction.TxLen = len;
            Transaction.RxBuf = data;
            Transaction.RxLen = 0;
            Transaction.XferDoneCallback = 0;
            
            I2c::Xfer(&Transaction);

            while(I2c::GetState() != i2c::I2C_READY);
            
            // EEPROM write delay
            delay_ms(Write_Delay_ms);
        }        
        
        static void PacketHandler(i2c::Status_t Status)
        {
          if(Status == i2c::I2C_XFER_DONE)
          {
            delay_ms(Write_Delay_ms);
          }
        }
    };
    
    template <typename I2c, uint8_t Device_Address, uint32_t size, uint8_t PageSize>
    i2c::MasterTxn_t AT24Cxx<I2c,Device_Address,size,PageSize>::Transaction;

}

#endif // AT24Cxx_H