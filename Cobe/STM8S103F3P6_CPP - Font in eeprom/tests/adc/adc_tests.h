

#include"system.h"
#include"DigitalOut.h"
#include"DigitalIn.h"

#include"adc.h"

typedef adc<ADC_Mode_Sngle,ADC_CH_5> adc_t; 
//typedef adc<ADC_Mode_Single_Scan,ADC_CH_6> adc_t;

extern adc_t Analog5;

void adc_tests(void);

void adc_test_Single(void);

void adc_test_SingleScan(void);

void adc_test_ContinousScan(void);

void Adc_AWD_Callback(void);