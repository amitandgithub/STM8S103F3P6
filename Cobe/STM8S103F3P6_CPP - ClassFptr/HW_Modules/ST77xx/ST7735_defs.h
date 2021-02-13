

#ifndef ST7735_defs
#define ST7735_defs



//#include "fonts.h"
#include <stdint.h>

#define DELAY 0x80

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

typedef struct 
{
  const uint8_t width;
  uint8_t height;
  const uint16_t *data;
} FontDef;


extern FontDef st7735_Font_7x10 ;
extern FontDef st7735_Font_11x18;
extern FontDef st7735_Font_16x26 ;



// mini 160x80, rotate left - Working one
/*
#define IS_160X80 1
#define XSTART 0
#define YSTART 24
#define WIDTH  160
#define HEIGHT 80
#define ROTATION (MADCTL_MX | MADCTL_MV | MADCTL_BGR)
*/



/*
#define IS_160X80 1
#define XSTART 24
#define YSTART 0
#define WIDTH  80
#define HEIGHT 160 
#define ROTATION (MADCTL_BGR) //(MADCTL_MX | MADCTL_MY | MADCTL_BGR)
  */

// mini 160x80 display (it's unlikely you want the default orientation)
/*
#define IS_160X80 1
#define XSTART 26
#define YSTART 1
#define WIDTH  80
#define HEIGHT 160 
#define ROTATION (MADCTL_MX | MADCTL_MY | MADCTL_BGR)
*/



// mini 160x80, rotate right 
/*
#define IS_160X80 1
#define XSTART 1
#define YSTART 26
#define WIDTH  160
#define HEIGHT 80
#define ROTATION (MADCTL_MY | MADCTL_MV | MADCTL_BGR)
*/

// AliExpress/eBay 1.8" display, default orientation
/*
#define IS_160X128 1
#define WIDTH  128
#define HEIGHT 160
#define XSTART 0
#define YSTART 0
#define ROTATION (MADCTL_MX | MADCTL_MY)
*/

// AliExpress/eBay 1.8" display, rotate right
#if 0

#define IS_160X128 1
#define WIDTH  160
#define HEIGHT 128
#define XSTART 0
#define YSTART 0
#define ROTATION (MADCTL_MY | MADCTL_MV)

#else
//0
#define IS_160X80 1
#define WIDTH  80
#define HEIGHT 160
#define XSTART 24
#define YSTART 0
#define ROTATION (MADCTL_MX | MADCTL_MY | MADCTL_BGR)

//1
//#define IS_160X80 1
//#define WIDTH  160
//#define HEIGHT 80
//#define XSTART 0
//#define YSTART 24
//#define ROTATION (MADCTL_MY | MADCTL_MV | MADCTL_BGR)
//    
//        //2
//#define IS_160X80 1
//#define WIDTH  80
//#define HEIGHT 160
//#define XSTART 24
//#define YSTART 0
//#define ROTATION (MADCTL_BGR)
//    
//        //3
//#define IS_160X80 1
//#define WIDTH  160
//#define HEIGHT 80
//#define XSTART 0
//#define YSTART 24
//#define ROTATION (MADCTL_MX | MADCTL_MV | MADCTL_BGR)

#endif

/*
#define IS_160X128 1
#define WIDTH  160
#define HEIGHT 128
#define XSTART 0
#define YSTART 0
#define ROTATION (MADCTL_MY | MADCTL_MV)
*/
// AliExpress/eBay 1.8" display, rotate left
/*
#define IS_160X128 1
#define WIDTH  160
#define HEIGHT 128
#define XSTART 0
#define YSTART 0
#define ROTATION (MADCTL_MX | MADCTL_MV)
*/

// AliExpress/eBay 1.8" display, upside down
/*
#define IS_160X128 1
#define WIDTH  128
#define HEIGHT 160
#define XSTART 0
#define YSTART 0
#define ROTATION (0)
*/

// WaveShare ST7735S-based 1.8" display, default orientation
/*
#define IS_160X128 1
#define WIDTH  128
#define HEIGHT 160
#define XSTART 2
#define YSTART 1
#define ROTATION (MADCTL_MX | MADCTL_MY | MADCTL_RGB)
*/

// WaveShare ST7735S-based 1.8" display, rotate right
/*
#define IS_160X128 1
#define WIDTH  160
#define HEIGHT 128
#define XSTART 1
#define YSTART 2
#define ROTATION (MADCTL_MY | MADCTL_MV | MADCTL_RGB)
*/

// WaveShare ST7735S-based 1.8" display, rotate left
/*
#define IS_160X128 1
#define WIDTH  160
#define HEIGHT 128
#define XSTART 1
#define YSTART 2
#define ROTATION (MADCTL_MX | MADCTL_MV | MADCTL_RGB)
*/

// WaveShare ST7735S-based 1.8" display, upside down
/*
#define IS_160X128 1
#define WIDTH  128
#define HEIGHT 160
#define XSTART 2
#define YSTART 1
#define ROTATION (MADCTL_RGB)
*/

// 1.44" display, default orientation
/*
#define IS_128X128 1
#define WIDTH  128
#define HEIGHT 128
#define XSTART 2
#define YSTART 3
#define ROTATION (MADCTL_MX | MADCTL_MY | MADCTL_BGR)
*/
// 1.44" display, rotate right
/*
#define IS_128X128 1
#define WIDTH  128
#define HEIGHT 128
#define XSTART 3
#define YSTART 2
#define ROTATION (MADCTL_MY | MADCTL_MV | MADCTL_BGR)
*/

// 1.44" display, rotate left
/*
#define IS_128X128 1
#define WIDTH  128
#define HEIGHT 128
#define XSTART 1
#define YSTART 2
#define ROTATION (MADCTL_MX | MADCTL_MV | MADCTL_BGR)
*/

// 1.44" display, upside down
/*
#define IS_128X128 1
#define WIDTH  128
#define HEIGHT 128
#define XSTART 2
#define YSTART 1
#define ROTATION (MADCTL_BGR)
*/


/****************************/

#define NOP     0x00
#define SWRESET 0x01
#define RDDID   0x04
#define RDDST   0x09

#define SLPIN   0x10
#define SLPOUT  0x11
#define PTLON   0x12
#define NORON   0x13

#define INVOFF  0x20
#define INVON   0x21
#define DISPOFF 0x28
#define DISPON  0x29
#define CASET   0x2A
#define RASET   0x2B
#define RAMWR   0x2C
#define RAMRD   0x2E

#define PTLAR   0x30
#define COLMOD  0x3A
#define MADCTL  0x36

#define FRMCTR1 0xB1
#define FRMCTR2 0xB2
#define FRMCTR3 0xB3
#define INVCTR  0xB4
#define DISSET5 0xB6

#define PWCTR1  0xC0
#define PWCTR2  0xC1
#define PWCTR3  0xC2
#define PWCTR4  0xC3
#define PWCTR5  0xC4
#define VMCTR1  0xC5

#define RDID1   0xDA
#define RDID2   0xDB
#define RDID3   0xDC
#define RDID4   0xDD

#define PWCTR6  0xFC

#define GMCTRP1 0xE0
#define GMCTRN1 0xE1

// Color definitions
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))





// based on Adafruit ST7735 library for Arduino
static const uint8_t init_cmds1[] = {            // Init for 7735R, part 1 (red or green tab)
  15,                       // 15 commands in list:
  SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
  150,                    //     150 ms delay
  SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
  255,                    //     500 ms delay
  FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
  0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
  FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
  0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
  FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
  0x01, 0x2C, 0x2D,       //     Dot inversion mode
  0x01, 0x2C, 0x2D,       //     Line inversion mode
  INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
  0x07,                   //     No inversion
  PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
  0xA2,
  0x02,                   //     -4.6V
  0x84,                   //     AUTO mode
  PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
  0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
  PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
  0x0A,                   //     Opamp current small
  0x00,                   //     Boost frequency
  PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
  0x8A,                   //     BCLK/2, Opamp current small & Medium low
  0x2A,  
  PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
  0x8A, 0xEE,
  VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
  0x0E,
  INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
  MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
  ROTATION,        //     row addr/col addr, bottom to top refresh
  COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
  0x05 },                 //     16-bit color
  
  init_cmds2_IS_128X160[] = {            // Init for 7735R, part 2 (1.44" display) 
    2,                        //  2 commands in list:
    CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
    0x00, 0x00,             //     XSTART = 0
    0x00, 0x7F+0x02,             //     XEND = 127
    RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
    0x00, 0x00,             //     XSTART = 0
    0x00, 0x9F+0x01 },           //     XEND = 127
    
    init_cmds2_160X80[] = {            // Init for 7735S, part 2 (160x80 display)
      3,                        //  3 commands in list:
      CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x4F,             //     XEND = 79
      RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F /* ,            //     XEND = 159
        INVON, 0*/ },        //  3: Invert colors
        
        init_cmds3[] = {            // Init for 7735R, part 3 (red or green tab)
          4,                        //  4 commands in list:
          GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
          0x02, 0x1c, 0x07, 0x12,
          0x37, 0x32, 0x29, 0x2d,
          0x29, 0x25, 0x2B, 0x39,
          0x00, 0x01, 0x03, 0x10,
          GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
          0x03, 0x1d, 0x07, 0x06,
          0x2E, 0x2C, 0x29, 0x2D,
          0x2E, 0x2E, 0x37, 0x3F,
          0x00, 0x00, 0x02, 0x10,
          NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
          10,                     //     10 ms delay
          DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
          100 }; 

static const uint8_t 
generic_st7789[] =  {                // Init commands for 7789 screens
  9,                              //  9 commands in list:
  SWRESET,   DELAY, //  1: Software reset, no args, w/delay
  150,                          //     ~150 ms delay
  SLPOUT ,   DELAY, //  2: Out of sleep mode, no args, w/delay
  10,                          //      10 ms delay
  COLMOD , 1+DELAY, //  3: Set color mode, 1 arg + delay:
  0x55,                         //     16-bit color
  10,                           //     10 ms delay
  MADCTL , 1,              //  4: Mem access ctrl (directions), 1 arg:
  0x08,                         //     Row/col addr, bottom-top refresh
  CASET  , 4,              //  5: Column addr set, 4 args, no delay:
  0x00,
  0,        //     XSTART = 0
  0,
  240,  //     XEND = 240
  RASET  , 4,              //  6: Row addr set, 4 args, no delay:
  0x00,
  0,             //     YSTART = 0
  320>>8,
  320&0xFF,  //     YEND = 320
  INVON  ,   DELAY,  //  7: hack
  10,
  NORON  ,   DELAY, //  8: Normal display on, no args, w/delay
  10,                           //     10 ms delay
  DISPON ,   DELAY, //  9: Main screen turn on, no args, delay
  10 };   

#endif //ST7735_defs