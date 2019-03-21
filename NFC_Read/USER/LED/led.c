#include "stm32f4xx.h"
#include "led.h"

void LED_Config(void)
{
	// D3 ---> PB0
	GPIO_InitTypeDef  GPIO_InitStructure;
	// 开启时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	// 初始化PB0口
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	GPIO_SetBits(GPIOB,GPIO_Pin_9);
}

void LED_Switch(LEDStatus state)
{
	if(state == LED_ON)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	}
	else
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_0);
	}
}


void LED2_Switch(LEDStatus state)
{
	if(state == LED_ON)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_9);
	}
	else
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_9);
	}
}


void LED_Tiggle(void)
{
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) == 0)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_0);
	}
	else
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	}
}


void LED2_Tiggle(void)
{
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9) == 0)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_9);
	}
	else
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_9);
	}
}

