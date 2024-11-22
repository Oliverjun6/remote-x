/*通过定时器设置采样频率，DMA对采样到的数据进行传输
使用FFT对采样到的波形数据进行傅里叶变换，
得到傅里叶序列，对其进行计算得到幅频特性序列，
当幅值最大时对应的序列就是该未知波形的频率（序列需要乘采样频率再除采样点数）。
采样定理规定采样频率必须要大于所采波形频率的2倍*/

#include "stm32f10x.h"                  // Device header
#include "stm32_dsp.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "lcd.h"
#include "bsp_led.h" 
#include "stdbool.h"
#include "stm32_dsp.h"
#include "math.h"
#include "TM1637.h"
#include "timer.h"
#define SAMPLS_NUM 48
#define PACKET_LENGTH 24 // 每个数据包的长度
#define DATA_LENGTH 48  // Receive_Data长度
#define HEADER 0xF       // 包头 (1111)
#define FOOTER 0xE       // 包尾 (1110)
// 存放解析出的数据包
uint8_t Parsed_Data[4]; // 最多保存两个数据包，每包有4个数字
u16 ADC_Data[SAMPLS_NUM];
u8 Receive_Data[SAMPLS_NUM];
u32 FFT_Data[SAMPLS_NUM];
u32 FFT_OutData[SAMPLS_NUM/2];
u32 FFT_Mag[SAMPLS_NUM/2] ; //幅频特性序列（序号代表频率分量，值代表幅值大小。由于FFT的频谱结果是关于奈奎斯特频率对称的，所以只计算一半的点即可）
int Parse_One_Packet(uint8_t *Receive_Data, uint8_t data_length, uint8_t Parsed_Data[4]);
int E_flag = 1;
//自定义结构体
struct GlobalStruct {
    bool adc_finish_fg;
    // 其他成员
};
struct GlobalStruct global;  // 全局变量

// 或者只是定义一个全局变量
bool adc_flag = false;  // 初始化为false
unsigned short maxMagIndex = 0;

void my_dma_init(void);
u8 Bin_switch(u16 adc_num);
uint8_t Extract_Number(uint8_t *data, uint8_t start_bit) ;
void my_DMA_NVIC_Init(void);
void Get_PowreMag(void);
int peak_find(unsigned int *a);
void Display_Num(void);
void adcall_int()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	uart_init(9600);
	
	TM1637_Init();
	TM1637_DisplayOff();
	
	delay_init();
	
	ADC_GPIO_Init();

	LED_GPIO_Config();
	
	adc_TIM3_Int_Init(3600,1000);
	
	adc_Init();
	
	my_dma_init();
	
	LCD_Init();  

}

int main(void)
{	
	u16 j;
	int peak;
	int result;
	adcall_int();
	while(1)
	{
		//TM1637_NixieTubeDisplay();
		if(adc_flag)
        {
//			cr4_fft_256_stm32( FFT_OutData, FFT_Data, SAMPLS_NUM);//注意FFT输出的时一个复数数组
//			Get_PowreMag();
			adc_flag = false;
        }
		for(j=0;j<SAMPLS_NUM;j++)
		{
			Receive_Data[j] =Bin_switch(ADC_Data[j]) ;
			//printf("%u\n", Bin_switch(ADC_Data[j]));
		}
		result = Parse_One_Packet(Receive_Data, DATA_LENGTH, Parsed_Data);
		if (result) 
		{
			printf("Found one packet: %d %d %d %d\n",
			Parsed_Data[0], Parsed_Data[1], Parsed_Data[2], Parsed_Data[3]);
			if(E_flag)
			{
				Display_Num();
			}
			if(Parsed_Data[0] == 10 | Parsed_Data[1] == 10 | Parsed_Data[2] == 10 | Parsed_Data[3] == 10)
			{
				LEDR(0);
				//E_flag = 0;
				//delay_ms(1000);
				TM1637_DisplayOff();
				//time_Nixie_init();
				
			}
		} 
		else 
		{
			printf("No valid packet found.\n");
		}
		
		
	
	}
	
}



void my_DMA_NVIC_Init()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel= DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority	= 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd   = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DMA_ClearITPendingBit(DMA1_IT_TC1);//清除DMA1通道1的传输完成中断（TC1）挂起位
	DMA_ITConfig(DMA1_Channel1,DMA1_IT_TC1,ENABLE);//配置DMA1通道1的中断
}



void my_dma_init()
{
	
	DMA_InitTypeDef DMA_InitStruct;
	            
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	DMA_DeInit(DMA1_Channel1);
	
	DMA_InitStruct.DMA_PeripheralBaseAddr=(u32)&ADC1->DR;//ADC规则数据寄存器(ADC_DR) (u32)ADC1_BASE+0x4C
	DMA_InitStruct.DMA_MemoryBaseAddr    =(u32)ADC_Data;//储存数据的，把ADC那里采集的数据存在这里
	DMA_InitStruct.DMA_DIR= DMA_DIR_PeripheralSRC;//方向，外设作为源，外设到内存，数据将从ADC（外设）传输到ADC_SourceData（内存）
	DMA_InitStruct.DMA_BufferSize= SAMPLS_NUM;//采样点
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//外设地址固定
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable ;		//内存地址自增
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//16位
	DMA_InitStruct.DMA_MemoryDataSize= DMA_MemoryDataSize_HalfWord ;
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority=DMA_Priority_High;
	DMA_InitStruct.DMA_M2M= DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1,  &DMA_InitStruct);
	
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	my_DMA_NVIC_Init();
}

void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)//中断出现
	{
		adc_flag = true;
		DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
	
	//让DMA不断响应工作
}

void Get_PowreMag()
{
	signed short realPart, imagPart;
	float Re, Im, Mag;
	
	unsigned short i;
	unsigned long maxMag = 0;
    unsigned long secondMaxMag = 0;
    unsigned short maxIndex = 0;
    unsigned short secondMaxIndex = 0;
	

	//printf("波形频率:%u\nKHz", secondMaxIndex);
//	LCD_ShowString(15,40,100,16,16,"CurrentFre:");
	LCD_ShowxNum(100, 40,secondMaxIndex, 5, 16, 0);
//	LCD_ShowString(180,40,40,16,16,"HKz");
}



int peak_find(unsigned int *a)
{
	int i , j, count=0;
	unsigned int *peak = a;
	unsigned int aMax = 0;
	for(i = 1 ; i < 150 ; i++)
	{
		if(a[i] > aMax)
		{
			aMax = a[i];
		}	
		if((a[i] > a[i+1]) && (a[i] < a[i-1]) && a[i]>80)
		{
			count++;
	
		}
		peak++;
	}	
	
		if(aMax <200)
			count = 0;
		if(aMax >1500)
			count = 1;

		
	return count;
}

u8 Bin_switch(u16 adc_num)
{
	u8 Bin_num;
	if(adc_num>2000)
	{
		Bin_num = 1;	
	}
	else
		Bin_num = 0;
	return Bin_num;
}



// 提取4位数据为数字
uint8_t Extract_Number(uint8_t *data, uint8_t start_bit) 
{
    uint8_t number = 0;
	int i;
    for (i = 0; i < 4; i++) 
	{
        number = (number << 1) | data[start_bit + i];
    }
    return number;
}
int Parse_One_Packet(uint8_t *Receive_Data, uint8_t data_length, uint8_t Parsed_Data[4]) 
{
	int i , j ;
    for (i = 0; i <= data_length - PACKET_LENGTH; i++) 
	{
        // 检查包头
        uint8_t header = Extract_Number(Receive_Data, i);
        if (header == HEADER) 
		{
			
            // 检查包尾
            uint8_t footer = Extract_Number(Receive_Data, i + PACKET_LENGTH - 4);
            if (footer == FOOTER) 
			{
                // 提取数据部分
                for (j = 0; j < 4; j++) 
				{
                    uint8_t number = Extract_Number(Receive_Data, i + 4 + j * 4);
                    if (number > 11) 
					{
                        return 0; // 数据异常，返回失败
                    }
                    Parsed_Data[j] = number;
                }
                return 1; // 成功找到一个完整的数据包
            }
        }
    }
    return 0; // 未找到完整数据包
}
void Display_Num()
{
		int j;
		TM1637_Start();
		TM1637_WriteByte(0x40);//0x40则采用显示地址自加1模式
		TM1637_Ack();
		TM1637_Stop();
		TM1637_Start();
		TM1637_WriteByte(0xC0);//0X00地址开始显示
		TM1637_Ack();
		

		
		for(j = 0;j < 4; j++)
			{
				
				if(Parsed_Data[j] == 0)
				{
					TM1637_WriteByte(zero);
					TM1637_Ack();
				}
				if(Parsed_Data[j] == 1)
				{
					TM1637_WriteByte(one);
					TM1637_Ack();
				}
				if(Parsed_Data[j] == 2)
				{
					TM1637_WriteByte(two);
					TM1637_Ack();
				}
				if(Parsed_Data[j] == 3)
				{
					TM1637_WriteByte(three);
					TM1637_Ack();
				}
				if(Parsed_Data[j] == 4)
				{
					TM1637_WriteByte(four);
					TM1637_Ack();
				}
				if(Parsed_Data[j] == 5)
				{
					TM1637_WriteByte(five);
					TM1637_Ack();
				}
				if(Parsed_Data[j] == 6)
				{
					TM1637_WriteByte(six);
					TM1637_Ack();
				}
				if(Parsed_Data[j] == 7)
				{
					TM1637_WriteByte(seven);
					TM1637_Ack();
				}
				if(Parsed_Data[j] == 8)
				{
					TM1637_WriteByte(eight);
					TM1637_Ack();
				}
				if(Parsed_Data[j] == 9)
				{
					TM1637_WriteByte(nine);
					TM1637_Ack();
				}
					if(Parsed_Data[j] == 0x49)
				{
					TM1637_WriteByte(0x49);
					TM1637_Ack();
				}
			}
			
		TM1637_Stop();
		TM1637_Start();
		TM1637_WriteByte(0x8f);
		TM1637_Ack();
		TM1637_Stop();

	
}

//	for(i = 0 ; i < SAMPLS_NUM / 2 ; i++)
//	{
//		//  FFT_OutData[i] 是一个32位整数，其中包含第i个频率分量的复数的实部和虚部
//		realPart = (FFT_OutData[i] & 0x0000FFFF); 			// 提取实部
//		imagPart = ((FFT_OutData[i] >> 16) & 0x0000FFFF);   // 提取虚部
//		//手动去浮点数,归一化处理，十六位的数，一个符号位，剩下15位表示0-32768的数
//		//这些浮点数乘以 SAMPLS_NUM，再除以32768，相当于将原始的16位整数值缩放到0到1之间。
//		Re = SAMPLS_NUM * ((float)realPart) / 32768;
//        Im = SAMPLS_NUM * ((float)imagPart) / 32768;
//		//Mag 计算为功率幅度
//		Mag = sqrt(Re*Re+Im*Im) / SAMPLS_NUM;
//		
//		FFT_Mag[i] = (unsigned long)(Mag * 65536);//mag通过计算是个浮点数，这一步对mag进行放大，增加精准度
//	
//		if (FFT_Mag[i] > maxMag) //遍历找最大和第二大的功率幅度
//		{
//            secondMaxMag = maxMag;
//            secondMaxIndex = maxIndex;
//            maxMag = FFT_Mag[i];
//            maxIndex = i;
//        } 
//		else if (FFT_Mag[i] > secondMaxMag) 
//		{
//            secondMaxMag = FFT_Mag[i];
//            secondMaxIndex = i;
//        }
//	}
	


