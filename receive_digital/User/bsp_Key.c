#include "bsp_key.h"
#include "stm32f10x.h"                  // Device header
#include "delay.h"

void KEY_GPIO_Config(void)
{
	
	GPIO_InitTypeDef GPIO_initStrust;
	
	RCC_APB2PeriphClockCmd(KEY_0_GPIO_CLK, ENABLE);//时钟使能
	
	GPIO_initStrust.GPIO_Pin  = KEY_0_GPIO_PIN | KEY_1_GPIO_PIN ;//对于同在一组的引脚，例；E3和E4，初始化时用 |  放在一起用
	
	GPIO_initStrust.GPIO_Mode = GPIO_Mode_IPU;
	                                  
	GPIO_Init(KEY_0_GPIO_PORT	, &GPIO_initStrust);
	GPIO_Init(KEY_1_GPIO_PORT	, &GPIO_initStrust);
	
//	RCC_APB2PeriphClockCmd(KEY_0_GPIO_CLK, ENABLE);
//	GPIO_initStrust.GPIO_Pin  = KEY_0_GPIO_PIN;
//	GPIO_initStrust.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_Init(KEY_0_GPIO_PORT	, &GPIO_initStrust);
//	
//	RCC_APB2PeriphClockCmd(KEY_1_GPIO_CLK, ENABLE);
//	GPIO_initStrust.GPIO_Pin  = KEY_1_GPIO_PIN;
//	GPIO_initStrust.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_Init(KEY_1_GPIO_PORT	, &GPIO_initStrust);
	
	
}

uint8_t KEY_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin)==KEY_ON)
	{
		delay_ms(20);
			if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin)==KEY_ON)
			{
				while(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin)==KEY_ON);
				return KEY_ON;
				
			}
		

	}
	else
	{
		return KEY_OFF;
	}
	
}


