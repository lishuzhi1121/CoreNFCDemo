#include "stm32f4xx.h"
#include "delay.h"
#include "key.h"

static int key_up = 1;//按键松开状态标识

void KEY_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	//使能GPIOA,GPIOA时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //KEY 对应引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA
}

int KEY_Scan(int mode)
{
	if(mode)
	{
		key_up = 1;
	}
	if(key_up && (KEY0 == 0))
	{
		delay_ms(5); // 去抖动
		key_up = 0;
		if(KEY0 == 0) 
		{
			return 999;
		}
	}
	else
	{
		key_up = 1;
	}
	return -1; // 无按键按下
}
