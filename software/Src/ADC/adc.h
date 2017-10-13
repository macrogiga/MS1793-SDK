#ifndef __ADC_H_
#define __ADC_H_
#include "HAL_device.h" 
#include "HAL_conf.h"

#define DISABLE_ALL_CHANNEL     9


void ADC1_SingleChannel(uint8_t ADC_Channel_x);
u16 ADC1_SingleChannel_Get(uint8_t ADC_Channel_x);
u16 Get_Adc_Average(uint8_t ADC_Channel_x,uint8_t times);

#endif
