#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "stm32f10x.h"
#include "sys.h"



#define LED_R_GPIO_PIN          GPIO_Pin_5
#define LED_R_GPIO_PORT			GPIOB
#define LED_R_GPIO_CLK	        RCC_APB2Periph_GPIOB
#define LED_R					BIT_ADDR(GPIOB_ODR_Addr,5)

#define LED_G_GPIO_PIN          GPIO_Pin_5
#define LED_G_GPIO_PORT			GPIOE
#define LED_G_GPIO_CLK	        RCC_APB2Periph_GPIOE
#define LED_G					BIT_ADDR(GPIOE_ODR_Addr,5)

#define ON		1
#define OFF		0

#define LEDR(a)					if(a) GPIO_ResetBits(LED_R_GPIO_PORT, LED_R_GPIO_PIN); else GPIO_SetBits(LED_R_GPIO_PORT, LED_R_GPIO_PIN);
#define LEDG(a)					if(a) GPIO_ResetBits(LED_G_GPIO_PORT, LED_G_GPIO_PIN); else GPIO_SetBits(LED_G_GPIO_PORT, LED_G_GPIO_PIN);

// ^ 异或 A异或1=A非，A异或0=A
#define LED_R_TOGGLE			{LED_R_GPIO_PORT->ODR ^=LED_R_GPIO_PIN;}
#define LED_G_TOGGLE			{LED_G_GPIO_PORT->ODR ^=LED_G_GPIO_PIN;}

void LED_GPIO_Config(void);
#endif
