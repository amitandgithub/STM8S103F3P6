
#ifndef TvCodes_h
#define TvCodes_h

//#include"TvCodes.h"


#define SAMSUNG_BITS          32
#define SAMSUNG_HDR_MARK    4500
#define SAMSUNG_HDR_SPACE   4500
#define SAMSUNG_BIT_MARK     560
#define SAMSUNG_ONE_SPACE   1600
#define SAMSUNG_ZERO_SPACE   560
#define SAMSUNG_RPT_SPACE   2250




/* DDC dish*/
#define DDC_POWER 0x14EB18E7

#define DDC_MUTE 0x14EBA857

#define DDC_CH_UP 0x14EBB04F
#define DDC_CH_DOWN 0x14EB7887

#define DDC_VOL_UP 0x14EBAA55
#define DDC_VOL_DOWN 0x14EB9A65


/* Worldtech camera*/
#define WT_POWER 0x20DFC837

#define WT_MUTE 0x20DFE01F

#define WT_CH_UP 0x20DF906F
#define WT_CH_DOWN 0x20DF8877

#define WT_VOL_UP 0x20DF00FF
#define WT_VOL_DOWN 0x20DF28D7



/* SAMSUNG 32 TV camera*/
#define SAMSUNG_32_TV_POWER 0xE0E040BF

#define SAMSUNG_32_TV_MUTE 0xE0E0F00F

#define SAMSUNG_32_TV_CH_UP 0xE0E048B7
#define SAMSUNG_32_TV_CH_DOWN 0xE0E008F7

#define SAMSUNG_32_TV_VOL_UP 0xE0E0E01F
#define SAMSUNG_32_TV_VOL_DOWN 0xE0E0D02F



















#endif //TvCodes_h