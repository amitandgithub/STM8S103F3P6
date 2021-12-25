/******************
** FILE: ST7735.cpp
**
** DESCRIPTION:
**  ST7735 implementation
**
** CREATED: 28/12/2019, by Amit Chaudhary
******************/

#include "ST7735.h"

namespace BSP
{
  
  ST7735::ST7735(ST77xx_t ST7735t, HAL::Spi* pSpiDriverLCD, Port_t CSPort, PIN_t CSPin,Port_t DCPort, PIN_t DCPin, Port_t RstPort, PIN_t RstPin, Port_t BklPort, PIN_t BklPin): m_ST77xx(ST7735t),m_Spi(pSpiDriverLCD),m_CSPin(CSPort,CSPin),m_DCPin(DCPort,DCPin),m_RstPin(RstPort,RstPin),m_BklPin(BklPort,BklPin)
  {
    
  }
  void ST7735::HwInit()
  {
    m_Spi->HwInit(); 
    if( (m_ST77xx == ST7789_135_x_240) || (m_ST77xx == ST7789_240_x_240) ) m_Spi->SetMode(3);
    m_Spi->SetBaudrate(HAL::Spi::SPI_BAUDRATE_DIV16);
    m_CSPin.HwInit();
    m_DCPin.HwInit();
    m_RstPin.HwInit();
    m_BklPin.HwInit();
    DisplayInit();  
    m_BklPin.High();
    
   // Rotate(ROTATE_0_DEG);
  }  
  void ST7735::Rotate(Rotation_t rot)
  {
    uint8_t Madctl;        
    if(m_ST77xx == ST7735_160_x_80)
    {
      switch(rot)
      {
      case ROTATE_0_DEG :
        m_WIDTH = 80;
        m_HEIGHT = 160;
        m_XSTART = 24;
        m_YSTART = 0;
        Madctl = (MADCTL_MX | MADCTL_MY | MADCTL_BGR);
        break;
      case ROTATE_90_DEG :
        m_WIDTH = 160;
        m_HEIGHT = 80;
        m_XSTART = 0;
        m_YSTART = 24;
        Madctl = (MADCTL_MY | MADCTL_MV | MADCTL_BGR);
        break;
      case ROTATE_180_DEG :
        m_WIDTH = 80;
        m_HEIGHT = 160;
        m_XSTART = 24;
        m_YSTART = 0;
        Madctl = (MADCTL_BGR);
        break;
      case ROTATE_270_DEG :
        m_WIDTH = 160;
        m_HEIGHT = 80;
        m_XSTART = 0;
        m_YSTART = 24;
        Madctl = (MADCTL_MX | MADCTL_MV | MADCTL_BGR);
        break;
      default: break;
      }
    }
    else if(m_ST77xx == ST7735_160_x_128)
    {
      switch(rot)
      {
      case ROTATE_0_DEG :
        m_WIDTH = 128;
        m_HEIGHT = 160;
        m_XSTART = 0;
        m_YSTART = 0;
        Madctl = (MADCTL_MX | MADCTL_MY | MADCTL_RGB);
        break;
      case ROTATE_90_DEG :
        m_WIDTH = 160;
        m_HEIGHT = 128;
        m_XSTART = 0;
        m_YSTART = 0;
        Madctl = (MADCTL_MY | MADCTL_MV | MADCTL_RGB);
        break;
      case ROTATE_180_DEG :
        m_WIDTH = 128;
        m_HEIGHT = 160;
        m_XSTART = 0;
        m_YSTART = 0;
        Madctl = (MADCTL_RGB);
        break;
      case ROTATE_270_DEG :
        m_WIDTH = 160;
        m_HEIGHT = 128;
        m_XSTART = 0;
        m_YSTART = 0;
        Madctl = (MADCTL_MX | MADCTL_MV | MADCTL_RGB);
        break;
        default: break;
      }
    }  
    else if(m_ST77xx == ST7789_240_x_240)
    {
      switch(rot)
      {
      case ROTATE_0_DEG :
        m_WIDTH = 240;
        m_HEIGHT = 240;
        m_XSTART = 0;
        m_YSTART = 80;
        Madctl = (MADCTL_MX | MADCTL_MY | MADCTL_RGB);
        break;
      case ROTATE_90_DEG :
        m_WIDTH = 240;
        m_HEIGHT = 240;
        m_XSTART = 80;
        m_YSTART = 0;
        Madctl = (MADCTL_MY | MADCTL_MV | MADCTL_RGB);
        break;
      case ROTATE_180_DEG :
        m_WIDTH = 240;
        m_HEIGHT = 240;
        m_XSTART = 0;
        m_YSTART = 0;
        Madctl = (MADCTL_RGB);
        break;
      case ROTATE_270_DEG :
        m_WIDTH = 240;
        m_HEIGHT = 240;
        m_XSTART = 0;
        m_YSTART = 0;
        Madctl = (MADCTL_MX | MADCTL_MV | MADCTL_RGB);
        break;
      default: break;
      }
    }  
    else if(m_ST77xx == ST7789_135_x_240)
    {
      switch(rot)
      {
      case ROTATE_0_DEG :
        m_WIDTH = 135;
        m_HEIGHT = 240;
        m_XSTART = 52;
        m_YSTART = 40;
        Madctl = (MADCTL_MX | MADCTL_MY | MADCTL_RGB);
        break;
      case ROTATE_90_DEG :
        m_WIDTH = 240;
        m_HEIGHT = 135;
        m_XSTART = 40;
        m_YSTART = 52;
        Madctl = (MADCTL_MY | MADCTL_MV | MADCTL_RGB);
        break;
      case ROTATE_180_DEG :
        m_WIDTH = 135;
        m_HEIGHT = 240;
        m_XSTART = 52;
        m_YSTART = 40;
        Madctl = (MADCTL_RGB);
        break;
      case ROTATE_270_DEG :
        m_WIDTH = 240;
        m_HEIGHT = 135;
        m_XSTART = 40;
        m_YSTART = 52;
        Madctl = (MADCTL_MX | MADCTL_MV | MADCTL_RGB);
        break;
      default: break;
      }
    }
    
    Select();
    WriteCommand(MADCTL);
    WriteData(Madctl);
    SetAddressWindow(m_XSTART,m_YSTART,m_WIDTH-1,m_HEIGHT-1);
    Deselect();    
  }
  
  void ST7735::DisplayInit()
  {      
    Select();
    Reset();
    if(m_ST77xx == ST7735_160_x_80)
    {        
      ExecuteCommandList(init_cmds1);
      ExecuteCommandList(init_cmds2_160X80);
      ExecuteCommandList(init_cmds3);
    }
    else if(m_ST77xx == ST7735_160_x_128)
    {
      ExecuteCommandList(init_cmds1);
      ExecuteCommandList(init_cmds2_IS_128X160);
      ExecuteCommandList(init_cmds3);
    }
    else if(m_ST77xx == ST7789_240_x_240)
    {
      ExecuteCommandList(generic_st7789);
    }
    else if(m_ST77xx == ST7789_135_x_240)
    {
      ExecuteCommandList(generic_st7789);
    }    
    Rotate(ROTATE_0_DEG);
    Deselect();
    FillScreen(BLACK);
  }  
  
  void ST7735::WriteCommand(uint8_t cmd) 
  {
    m_DCPin.Low();
    m_Spi->Tx(cmd);
  }  
  
  void ST7735::WriteData(uint8_t* buff, size_t buff_size) 
  {
    m_DCPin.High();
    m_Spi->TxPoll(buff, buff_size);
  }
  
  void ST7735::WriteData(uint8_t data) 
  {
    m_DCPin.High();
    m_Spi->Tx(data);
  }
  
  void ST7735::WriteData(uint16_t data) 
  {
    m_DCPin.High();
    m_Spi->Tx(data);
  }
  
  void ST7735::ExecuteCommandList(const uint8_t *addr) 
  {
    uint8_t numCommands, numArgs;
    uint16_t ms;
    
    numCommands = *addr++;
    while(numCommands--) {
      uint8_t cmd = *addr++;
      WriteCommand(cmd);
      
      numArgs = *addr++;
      // If high bit set, delay follows args
      ms = numArgs & DELAY;
      numArgs &= ~DELAY;
      if(numArgs) {
        WriteData((uint8_t*)addr, numArgs);
        addr += numArgs;
      }
      
      if(ms) {
        ms = *addr++;
        if(ms == 255) ms = 500;
        LL_mDelay(ms);
      }
    }
  }
  
  void ST7735::SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) 
  {    
    WriteCommand(CASET);
    WriteData( (uint16_t)(0x0000 | ((x0 + m_XSTART)<<8U)) );
    WriteData( (uint16_t)(0x0000 | ((x1 + m_XSTART)<<8U)) );
    
    // row address set
    WriteCommand(RASET);
    WriteData( (uint16_t)(0x0000 | ((y0 + m_YSTART)<<8U)) );
    WriteData( (uint16_t)(0x0000 | ((y1 + m_YSTART)<<8U)) );
    // write to RAM
    WriteCommand(RAMWR);
  }  
  
  void ST7735::DrawPixel(uint16_t x, uint16_t y, uint16_t color) 
  {
    if((x >= m_WIDTH) || (y >= m_HEIGHT))
      return;    
    Select();    
    SetAddressWindow(x, y, x+1, y+1);
    WriteData( (uint16_t)(color>>8U | color<<8U) );    
    Deselect();
  }
  
  void ST7735::WriteChar(uint16_t x, uint16_t y, char ch, FontDef* font, uint16_t color, uint16_t bgcolor) 
  {
    uint32_t i, b, j;    
    Select(); 
    SetAddressWindow(x, y, x+font->width-1, y+font->height-1);    
    for(i = 0; i < font->height; i++) {
      b = font->data[(ch - 32) * font->height + i];
      for(j = 0; j < font->width; j++) {
        if((b << j) & 0x8000)  {
          WriteData( (uint16_t)(color>>8U | color<<8U) );
        } else {
          WriteData( (uint16_t)(bgcolor>>8U | bgcolor<<8U) );
        }
      }
    }
    Deselect();
  } 
  
  void ST7735::WriteString(uint16_t x, uint16_t y, const char* str, FontDef* font, uint16_t color, uint16_t bgcolor)
  {
    //Select();    
    while(*str) {
      if(x + font->width >= m_WIDTH) {
        x = 0;
        y += font->height;
        if(y + font->height >= m_HEIGHT) {
          break;
        }        
        if(*str == ' ') {
          // skip spaces in the beginning of the new line
          str++;
          continue;
        }
      }      
      WriteChar(x, y, *str, font, color, bgcolor);
      x += font->width;
      str++;
    }    
    //Deselect();
  }
  
  void ST7735::FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h1, uint16_t color) 
  {
    // clipping
    if((x >= m_WIDTH) || (y >= m_HEIGHT)) return;
    if((x + w - 1) >= m_WIDTH) w = m_WIDTH - x;
    if((y + h1 - 1) >= m_HEIGHT) h1 = m_HEIGHT - y;
    
    Select();
    SetAddressWindow(x, y, x+w-1, y+h1-1);
    
    m_DCPin.High();
    for(y = h1; y > 0; y--) {
      for(x = w; x > 0; x--) {
        
        WriteData( (uint16_t)(color>>8U | color<<8U) );
      }
    }    
    Deselect();
  }
  
  
  void ST7735::FillScreen(uint16_t color) 
  {
    FillRectangle(0, 0, m_WIDTH, m_HEIGHT, color);
  }
  
  #define putpix(c) { lcd7735_senddata(c >> 8); lcd7735_senddata(c & 0xFF); }
  
  void ST7735::DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h1, const uint16_t* data) 
  {
    if((x >= m_WIDTH) || (y >= m_HEIGHT)) return;
    if((x + w - 1) >= m_WIDTH) return;
    if((y + h1 - 1) >= m_HEIGHT) return;
    
    Select();
    SetAddressWindow(x, y, x+w-1, y+h1-1);
    WriteData((uint8_t*)data, sizeof(uint16_t)*w*h1);    
    Deselect();
  }
  
  void ST7735::InvertColors(bool invert) 
  {
    Select();
    WriteCommand(invert ? INVON : INVOFF);
    Deselect();
  }
  
  void ST7735::DrawRctangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
  {
    DrawFastHLine(x, y, w, color); // from A to B
    DrawFastVLine(x+w, y, h, color); // from B to C
    DrawFastHLine(x,y+h,w, color); // from C to D
    DrawFastVLine(x,y,h, color); // from D to A
  }
  
  void ST7735::DrawFastVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color)
  {
    // Rudimentary clipping
    if((x >= m_WIDTH) || (y >= m_HEIGHT)) return;
    if((y+h-1) >= m_HEIGHT) h = m_HEIGHT-y;
    
    Select();
    SetAddressWindow(x, y,x, y+h-1);
    
    m_DCPin.High();
    while(h--) 
    {
      //putpix(color);
      WriteData( (uint16_t)(color>>8U | color<<8U) );
    }
    Deselect();
  }
  
  void  ST7735::DrawFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color) 
  {
    // Rudimentary clipping
    if((x >= m_WIDTH) || (y >= m_HEIGHT)) return;
    if((x+w-1) >= m_WIDTH)  w = m_WIDTH-x;
    
    Select();
    SetAddressWindow(x, y, x+w-1, y);    
    m_DCPin.High();
    while(w--) 
    {
      WriteData( (uint16_t)(color>>8U | color<<8U) );
    }
    Deselect();
  }

}