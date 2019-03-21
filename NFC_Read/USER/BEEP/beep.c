#include "stm32f4xx.h"
#include "delay.h"
#include "beep.h"

// beep ---> PG0
void BEEP_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	//使能GPIOG时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //BEEP 对应引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //普通输入模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	
  GPIO_Init(GPIOG, &GPIO_InitStructure); //初始化GPIOG
	
	GPIO_ResetBits(GPIOG,GPIO_Pin_0); //默认不响
}


void BEEP_Switch(BEEPStatus state)
{
	if(state == BEEP_ON)
	{
		GPIO_SetBits(GPIOG,GPIO_Pin_0);
	}
	else
	{
		GPIO_ResetBits(GPIOG,GPIO_Pin_0);
	}
}

void BEEP_Tiggle(void)
{
	if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_0) == 0)
	{
		GPIO_SetBits(GPIOG,GPIO_Pin_0);
	}
	else
	{
		GPIO_ResetBits(GPIOG,GPIO_Pin_0);
	}
}

