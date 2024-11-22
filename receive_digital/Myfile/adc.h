#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f10x.h" 
#include "sys.h"

void ADC_GPIO_Init(void);
void adc_Init(void);
void adc_TIM3_Int_Init(u16 arr,u16 psc);
void outputData(void);
#endif
