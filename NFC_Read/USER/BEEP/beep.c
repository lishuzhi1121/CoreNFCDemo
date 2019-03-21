#include "stm32f4xx.h"
#include "delay.h"
#include "beep.h"

// beep ---> PG0
void BEEP_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	//ʹ��GPIOGʱ��
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //BEEP ��Ӧ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //��ͨ����ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	
  GPIO_Init(GPIOG, &GPIO_InitStructure); //��ʼ��GPIOG
	
	GPIO_ResetBits(GPIOG,GPIO_Pin_0); //Ĭ�ϲ���
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

