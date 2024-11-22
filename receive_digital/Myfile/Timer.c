#include "bsp_led.h"
#include "bsp_Key.h"
#include "sys.h"   
#include "delay.h"
#include "TM1637.h"
#include "Timer.h"
#include "stm32f10x.h"                  // Device header
extern int E_flag;
                                 


void time_Nixie_init()
{
	TIM_TimeBaseInitTypeDef TIMER_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	TIMER_InitStruct.TIM_Period = 14399;
	TIMER_InitStruct.TIM_Prescaler = 12499;
	TIMER_InitStruct.TIM_CounterMode = TIM_CounterMode_Up ;
	TIMER_InitStruct.TIM_ClockDivision = TIM_CKD_DIV2;
	TIM_TimeBaseInit(TIM4, &TIMER_InitStruct);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x02;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM4 ,ENABLE);

}

void TIM4_IRQHandler(void) //locate in startup_103x_hd
{
	if(TIM_GetITStatus(TIM4,  TIM_IT_Update) == SET)   //SET = 1
	{
		TIM_ClearITPendingBit(TIM4 , TIM_IT_Update);  //clear
		if(E_flag)
		{
			TM1637_DisplayOff();
//			E_flag = 0;
			E_flag = 0;
			
		}

	}
}