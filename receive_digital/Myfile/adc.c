#include "stm32f10x.h"                  // Device header
#include "adc.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"

void ADC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);				//ʹ��GPIOAʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;		//�ܽ�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;						//ģ������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);								//GPIO��
}

void adc_Init()
{

	ADC_InitTypeDef ADC_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//72M/6=12M��ADC���ܳ���14M
	
	ADC_DeInit(ADC1);//��λ
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_ContinuousConvMode =DISABLE; //�л�����ת��������ת��
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;		//ɨ��ģʽ�л�,DISһ��ת��һ��ͨ��,// ��ֹɨ��ģʽ����ͨ����Ҫ����ͨ������Ҫ
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right ;
	ADC_InitStruct.ADC_NbrOfChannel = 1;			//˳����й���ת����ADC ͨ������Ŀ
	
	ADC_Init(ADC1, &ADC_InitStruct);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_7Cycles5);//����ʱ��
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_1,  1, ADC_SampleTime_7Cycles5);	//AI_VS_A1
	ADC_DMACmd(ADC1, ENABLE);
	
	ADC_Cmd(ADC1, ENABLE);
	
	//R��SУ׼
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1));
	
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);//����ADC���ⲿ����ת������
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

//ת��ʱ��һ��ҪС�ڲ�������

