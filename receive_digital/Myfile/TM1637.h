/******************************Copyright (c)***********************************
*                Jiangsu Zhihai Electronic Technology Co., Ltd.
*                      Research & Development Department
*
*													www.smarthealth-tech.com
*
-------------------------------------------------------------------------------
* @file    TM1637.h
* @author  GU DONGDONG
* @date    2015-11-25  
*******************************************************************************/
#ifndef TM1637_H
#define TM1637_H

#include "sys.h"

#define TM1637_CLK_PIN  GPIO_Pin_6
#define TM1637_DIO_PIN  GPIO_Pin_7

#define TM1637_CLK(x)   x>0? GPIO_SetBits(GPIOB, TM1637_CLK_PIN):GPIO_ResetBits(GPIOB, TM1637_CLK_PIN)
#define TM1637_DIO(x)   x>0? GPIO_SetBits(GPIOB, TM1637_DIO_PIN):GPIO_ResetBits(GPIOB, TM1637_DIO_PIN)

#define TM1637_READ_DIO    GPIO_ReadInputDataBit(GPIOB, TM1637_DIO_PIN)

//IO·½ÏòÉèÖÃ
#define TM1637_DIO_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
#define TM1637_DIO_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}

#define zero 0x3F
#define one 0x06
#define two 0x5B
#define three 0x4F
#define four 0x66
#define five 0x6D
#define six 0x7D
#define seven 0x07
#define eight 0x7F
#define nine 0X6F
/** 
  * @brief  Configuration Dots's Mode enumeration  
  */
typedef enum
{ 
    DulDot = 0x00,
    LowDot= 0x01,
    HighDot= 0x2,
}DisDotSetting;
typedef enum
{ 
    sty = 0x00,
    Jep = 0x01, 
}DisDotMode;

//extern unsigned char NumDis[];
void TM1637_Delay_us(unsigned  int Nus); 
void TM1637_Start(void);
void TM1637_Ack(void);
void TM1637_Stop(void);
void TM1637_WriteByte(unsigned char oneByte);
unsigned char TM1637_ScanKey(void);
void TM1637_NixieTubeDisplay(void);
void TM1637_DisplayValue(unsigned int value);
void TM1637_NixieTubeDisplayChar(unsigned char ch,unsigned char p);
void TM1637_NixieTubeDisplayNum(short Num,DisDotSetting Set,DisDotMode Mode);
void TM1637_Set_Bright(uint8_t state,uint8_t bright);
void TM1637_Init(void);
void TM1637LED_Init(void);
 void TM1637_DisplayOff(void);
#endif
