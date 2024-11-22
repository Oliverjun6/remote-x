#include "bsp_led.h"
#include "stm32f10x.h"

void LED_GPIO_Config()
{
	
	GPIO_InitTypeDef GPIO_initStrust;
	
	RCC_APB2PeriphClockCmd(LED_R_GPIO_CLK, ENABLE);
	GPIO_initStrust.GPIO_Pin = LED_R_GPIO_PIN;
	GPIO_initStrust.GPIO_Mode= GPIO_Mode_Out_PP;
	GPIO_initStrust.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(LED_R_GPIO_PORT	, &GPIO_initStrust);//第一个是端口，第二个是结构体指针
	
	RCC_APB2PeriphClockCmd(LED_G_GPIO_CLK, ENABLE);
	GPIO_initStrust.GPIO_Pin = LED_G_GPIO_PIN;
	GPIO_initStrust.GPIO_Mode= GPIO_Mode_Out_PP;
	GPIO_initStrust.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(LED_G_GPIO_PORT	, &GPIO_initStrust);

	
}

