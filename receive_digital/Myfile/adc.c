#include "stm32f10x.h"                  // Device header
#include "adc.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"

void ADC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);				//使能GPIOA时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;		//管脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;						//模拟输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);								//GPIO组
}

void adc_Init()
{

	ADC_InitTypeDef ADC_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//72M/6=12M，ADC不能超过14M
	
	ADC_DeInit(ADC1);//复位
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_ContinuousConvMode =DISABLE; //切换单次转换和连续转换
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;		//扫描模式切换,DIS一次转换一个通道,// 禁止扫描模式，多通道才要，单通道不需要
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right ;
	ADC_InitStruct.ADC_NbrOfChannel = 1;			//顺序进行规则转换的ADC 通道的数目
	
	ADC_Init(ADC1, &ADC_InitStruct);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_7Cycles5);//采样时间
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_1,  1, ADC_SampleTime_7Cycles5);	//AI_VS_A1
	ADC_DMACmd(ADC1, ENABLE);
	
	ADC_Cmd(ADC1, ENABLE);
	
	//R、S校准
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1));
	
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);//控制ADC的外部触发转换功能
}


void adc_TIM3_Int_Init(u16 arr,u16 psc)
{	
	TIM_TimeBaseInitTypeDef TIMER_InitStruct;	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIMER_InitStruct.TIM_Period = arr-1;
	TIMER_InitStruct.TIM_Prescaler = psc-1;
	TIMER_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIMER_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIMER_InitStruct);
	
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
	TIM_Cmd(TIM3 ,ENABLE);
	
}

//转换时间一定要小于采样周期

