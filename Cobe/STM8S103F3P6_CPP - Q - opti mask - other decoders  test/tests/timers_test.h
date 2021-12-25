
#ifndef TIMERS_TEST_H
#define TIMERS_TEST_H

#include"system.h"
#include"gpt1.h"
#include"gpt2.h"
#include"gpt4.h"
#include"pulseIn1.h"
#include"pulseIn2.h"
#include"IrDecoder1.h" 
#include"IrDecoder2.h"
#include"pulseOut1.h"
#include"pulseOut2.h"
#include"IrEncoder1.h" 
#include"IrEncoder2.h" 
#include"TvCodes.h"

void timer_tests(void);
void gpt_test(void);
void gpt4_test(void);
void pulseOut_test(void);
void pulseIn_test(void);
void IrDecoder_test(void);
void IR_Transmitter(void); 
void IR_Receiver(void);
void IR_Extender(void);
void IR_Loopback(void);
void WT_to_DDC(uint32_t code);
void WT_to_Samsung(uint32_t code);

typedef
//IrDecoder2<TIM2_CH1_C5_RMP,
IrDecoder1<TIM1_CH1_C6_RMP,
SAMSUNG_BITS,
SAMSUNG_HDR_MARK,
SAMSUNG_HDR_SPACE,
SAMSUNG_BIT_MARK,
SAMSUNG_ONE_SPACE,
SAMSUNG_ZERO_SPACE,
SAMSUNG_RPT_SPACE> IR_Decoder_t;

typedef
//IrEncoder1<TIM1_CH3_C3,
IrEncoder2<TIM2_CH3_A3,
SAMSUNG_BITS,
SAMSUNG_HDR_MARK,
SAMSUNG_HDR_SPACE,
SAMSUNG_BIT_MARK,
SAMSUNG_ONE_SPACE,
SAMSUNG_ZERO_SPACE,
SAMSUNG_RPT_SPACE> Ir_Encoder_t;

extern Ir_Encoder_t Ir_Encoder;
extern IR_Decoder_t Ir_Decoder;







#endif



