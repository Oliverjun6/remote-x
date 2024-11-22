#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include "stm32f10x.h"
#include "sys.h"



#define KEY_0_GPIO_PIN          GPIO_Pin_4
#define KEY_0_GPIO_PORT			GPIOE
#define KEY_0_GPIO_CLK	        RCC_APB2Periph_GPIOE//复制的时候看好引脚改了没有



#define KEY_1_GPIO_PIN          GPIO_Pin_3
#define KEY_1_GPIO_PORT			GPIOE
#define KEY_1_GPIO_CLK	        RCC_APB2Periph_GPIOE

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
#define KEY_ON		0
#define KEY_OFF		1


void KEY_GPIO_Config(void);
uint8_t KEY_Scan(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#endif
