#ifndef SSD1306_H
#define SSD1306_H

#include "system.h"
#include "fonts.h"
#include "Utils.h"
#include"HAL_Objects.h"

using namespace HAL;

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

static const uint16_t* FonData = Font16x26;

namespace Devices
{
    template <typename I2c, uint8_t Device_Address, uint8_t FontWidth = 11, uint8_t FontHeight = 18, bool eeprom_font_buffer = false > // Font_11x18 -> 3420 bytes in flash

    class SSD1306  // Saved around 75b flash, 1b RAM for making static
    {
    public:
        static const uint8_t BufSize = ((FontWidth+1) * (FontHeight+1)) / 8; 

        static const uint16_t Font_11x18_start_address = 0x0000;
        static const uint16_t Font_11x18_end_address = Font_11x18_start_address + BufSize*95;
        
        static const uint16_t Font_16x26_start_address1 = Font_11x18_end_address;
        static const uint16_t Font_16x26_end_address1 = Font_16x26_start_address1 + BufSize*65;
        
        static const uint16_t Font_16x26_start_address2 = Font_16x26_end_address1;
        static const uint16_t Font_16x26_end_address2 = Font_16x26_start_address1 + BufSize*30;
        
        static const uint16_t Font_Current_address = Font_11x18_start_address;
        static const uint16_t Font_Current_end_address = Font_11x18_end_address;
   
        typedef enum
        {
            SSD1306_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
            SSD1306_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
        } SSD1306_COLOR_t;
        
        static uint8_t ScreenBuffer[BufSize+1];

        static void HwInit(void)
        {
            I2c::HwInit();
            
            delay_ms(10);

            /* Init LCD */
            Cmd(0xAE); //display off
            Cmd(0x20); //Set Memory Addressing Mode
            Cmd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
            Cmd(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
            Cmd(0xC8); //Set COM Output Scan Direction
            Cmd(0x00); //---set low column address
            Cmd(0x10); //---set high column address
            Cmd(0x40); //--set start line address
            Cmd(0x81); //--set contrast control register
            Cmd(0xFF);
            Cmd(0xA1); //--set segment re-map 0 to 127
            Cmd(0xA6); //--set normal display
            Cmd(0xA8); //--set multiplex ratio(1 to 64)
            Cmd(0x3F); //
            Cmd(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
            Cmd(0xD3); //-set display offset
            Cmd(0x00); //-not offset
            Cmd(0xD5); //--set display clock divide ratio/oscillator frequency
            Cmd(0xF0); //--set divide ratio
            Cmd(0xD9); //--set pre-charge period
            Cmd(0x22); //
            Cmd(0xDA); //--set com pins hardware configuration
            Cmd(0x12);
            Cmd(0xDB); //--set vcomh
            Cmd(0x20); //0x20,0.77xVcc
            Cmd(0x8D); //--set DC-DC enable
            Cmd(0x14); //
            Cmd(0xAF); //--turn on SSD1306 panel

            Cmd(SSD1306_DEACTIVATE_SCROLL);
            /* Clear screen */
            Fill(0);
        }

        static void SetBrightness(uint8_t level)
        {  
          Cmd(SSD1306_SETCONTRAST);
          Cmd(level);
        }

        static void UpdateScreen(uint8_t x, uint8_t y)
        {
          Cmd(SSD1306_COLUMNADDR);
          Cmd(x);                    // Column start address (0 = reset)
          Cmd( x + FontWidth); // Column end address (127 = reset)
          
          Cmd(SSD1306_PAGEADDR);
          Cmd(y>>3); // Page start address (0 = reset)
          Cmd(7); // Page end address
          
          ScreenBuffer[0] = 0x40;
          
          I2c::Tx(Device_Address, ScreenBuffer, BufSize);
        }

        static void Off(void) { Cmd(0xAE); } //display off}

        static void On(void) { Cmd(0xAF); } //--turn on SSD1306 panel}

        static void DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color) 
        {          
            /* Set color */
          if (color == SSD1306_COLOR_WHITE) {
            ScreenBuffer[x + (y / 8) * (FontWidth +1) + 1] |= 1 << (y % 8);
          } else {
            ScreenBuffer[x + (y / 8) * (FontWidth +1) + 1] &= ~(1 << (y % 8));
          }
        }
        
        static char Putc(char ch, uint8_t x, uint8_t y )
        {            
            uint16_t i, b, j;
          
            /* Go through font */
            for (i = 0; i < FontHeight; i++)
            {
                b = FonData[(ch - 32) * FontHeight + i];
                //b = FonData[(ch - 32 -66) * FontHeight + i]; // use this for updating big font address2 in eeprom
                for (j = 0; j < FontWidth; j++)
                {
                    if ((b << j) & 0x8000)
                    {
                        DrawPixel( 0 + j, (0 + i), SSD1306_COLOR_WHITE);
                    }
                    else
                    {
                        DrawPixel( 0 + j, (0 + i), SSD1306_COLOR_BLACK);
                    }
                }
            }
            UpdateScreen(x,y);
            /* Return character written */
            return ch;
        }
        
        
        static void Putc_from_eeprom(char ch, uint8_t x, uint8_t y )
        {
          uint16_t Font_Current_address;
          
            if(FontWidth == 11)
            {
              Font_Current_address = Font_11x18_start_address; 
            }
            else if(FontWidth == 16)
            {
              Font_Current_address = Font_16x26_start_address1;
            }
            else
            {
              Font_Current_address = Font_11x18_start_address;  // default font
            }
            
            AT24C128_t::Read(Font_Current_address + (ch - 32)*BufSize, ScreenBuffer,BufSize);
            
            UpdateScreen(x,y);
        }
        
        static void Update_Eeprom(void)
        {
          if(eeprom_font_buffer == true)
          {
            uint16_t address = Font_Current_address;

            for(char ch = ' '; ch <= '~'; ch++)
            {
              Putc(ch,0,0);
              AT24C128_t::Write(address, ScreenBuffer, BufSize);
              address += BufSize;
            }          
          }
        }
        
        static void Update_Eeprom_Big_Font(char start, char end, uint16_t eeprom_address)
        {
          if(eeprom_font_buffer == true)
          {
            uint16_t address = eeprom_address;

            for(char ch = start; ch <= end; ch++)
            {
              Putc(ch,0,0);
              AT24C128_t::Write(address, ScreenBuffer, BufSize);
              address += BufSize;
            }          
          }
        }
           
        static void Puts(char* Buf,uint8_t Len, uint8_t x, uint8_t y)
        {
          for(uint8_t count = 0; count<Len; count++)
          {
            if(eeprom_font_buffer == false)
            {                
              Putc(Buf[count], x, y);
            }
            else
            {
              Putc_from_eeprom(Buf[count], x, y);                
            }
            
            x =  x + FontWidth;
          }
        }
         
        
        static void I2C_Write(uint8_t control_byte, uint8_t data)
        {
            uint8_t dt[2];
            dt[0] = control_byte;
            dt[1] = data;
            I2c::Tx(Device_Address, dt, 2);
        }
        static void Cmd(uint8_t data)
        {
            I2C_Write(0x00, data);
        }

        static void Data(uint8_t data)
        {
            I2C_Write(0x40, data);
        }
        
        static void Fill(uint8_t data)
        {
          uint16_t counter;
          
          Cmd(SSD1306_COLUMNADDR);
          Cmd(0);   // Column start address (0 = reset)
          Cmd(127); // Column end address (127 = reset)
          
          Cmd(SSD1306_PAGEADDR);
          Cmd(0); // Page start address (0 = reset)
          Cmd(7); // Page end address
          
          for(counter=0;counter<128*8 ;counter++)
          {
            Data(data);
          }
        }


        static void Tests(void)
        {        
          char ch = ' ';
            HwInit();
            //Puts("0123456",8,0,0);
            //Puts("BCDEFGHIJKL",0,25);
            //Update_Eeprom();
            //Update_Eeprom_Big_Font(' ', 'a',Font_16x26_start_address1 );
            
            //Make  sure Font array adjusment done and b = FonData[(ch - 32 -66) * FontHeight + i]; 
            // use this for updating big font address2 in eeprom
           // Update_Eeprom_Big_Font('b', '~',Font_16x26_start_address2 ); 
            Puts("01234567",7,0,0);
            Puts("ABCDEFGH",7,0,32);
            Puts("abcdefgh",7,0,0);
            while(1)
            {
              Putc_from_eeprom(ch++,0,32);
              delay_ms(200);
            }
        }
    };
    template <typename I2c, uint8_t Device_Address, uint8_t FontWidth, uint8_t FontHeight, bool eeprom_font_buffer >
      uint8_t Devices::SSD1306<I2c, Device_Address, FontWidth, FontHeight, eeprom_font_buffer >::ScreenBuffer[Devices::SSD1306<I2c, Device_Address, FontWidth, FontHeight, eeprom_font_buffer >::BufSize+1];
}

#endif //SSD1306_H