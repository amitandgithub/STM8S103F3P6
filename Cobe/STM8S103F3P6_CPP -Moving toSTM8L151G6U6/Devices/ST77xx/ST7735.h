/*
* ST7735.h
*
*  Created on: 28-Dec-2019
*      Author: Amit Chaudhary
*/

#ifndef ST7735_h
#define ST7735_h

#include "CPP_HAL.h" 
#include "Spi.h"
#include "GpioOutput.h"
#include "ST7735_defs.h"


namespace BSP 
{
    class ST7735 
    {
    public:
      typedef enum
      {
        ST7735_160_x_80,
        ST7735_160_x_128, 
        ST7789_240_x_240,
        ST7789_135_x_240,
      }ST77xx_t;
      
      typedef enum
      {
        ROTATE_0_DEG,
        ROTATE_90_DEG,
        ROTATE_180_DEG,
        ROTATE_270_DEG
      }Rotation_t;
      
      ST7735(ST77xx_t ST7735t, HAL::Spi* pSpiDriverLCD, Port_t CSPort, PIN_t CSPin,Port_t DCPort, PIN_t DCPin, Port_t RstPort, PIN_t RstPin, Port_t BklPort, PIN_t BklPin);
      void HwInit();
      void DisplayInit(void);
      void DrawPixel(uint16_t x, uint16_t y, uint16_t color);
      void WriteString(uint16_t x, uint16_t y, const char* str, FontDef* font, uint16_t color, uint16_t bgcolor);
      void FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h1, uint16_t color);
      void FillScreen(uint16_t color);
      void DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h1, const uint16_t* data);
      void InvertColors(bool invert);
      
      void DrawFastVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
      void DrawFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
      void DrawRctangle(uint16_t x, uint16_t y, uint16_t h, uint16_t w, uint16_t color);
        
      void Select(){m_CSPin.Low();}
      void Deselect(){m_CSPin.High();}
      void Reset(){m_RstPin.Low(); LL_mDelay(20); m_RstPin.High();LL_mDelay(30);}
      void WriteCommand(uint8_t cmd);
      void WriteData(uint8_t* buff, size_t buff_size);
      void WriteData(uint8_t data);
      void WriteData(uint16_t data);
      void ExecuteCommandList(const uint8_t *addr);
      void SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
      void WriteChar(uint16_t x, uint16_t y, char ch, FontDef* font, uint16_t color, uint16_t bgcolor);
      void setRotation(uint8_t m);
      void Rotate(Rotation_t rot);
      void drawLine(int x0, int y0, int x1, int y1,unsigned int color);
      void drawFastLine(uint16_t x, uint16_t y, unsigned char length,uint16_t color, unsigned char rotflag);
      void drawHorizontalLine(unsigned char x, unsigned char y, unsigned char length, unsigned int color);
      void drawVerticalLine(unsigned char x, unsigned char y, unsigned char length, unsigned int color);
      void fillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h,unsigned int color);
      void drawRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h,unsigned int color);
    private:
      uint16_t          m_Res_Width;
      uint16_t          m_Res_Height;
      ST77xx_t          m_ST77xx;
      uint8_t           m_WIDTH;
      uint8_t           m_HEIGHT;
      uint8_t           m_XSTART;
      uint8_t           m_YSTART;
      HAL::Spi*         m_Spi;
      HAL::GpioOutput   m_CSPin;
      HAL::GpioOutput   m_DCPin;
      HAL::GpioOutput   m_RstPin;
      HAL::GpioOutput   m_BklPin;     
    }; 
}
  
#endif /* ST7735_h */
