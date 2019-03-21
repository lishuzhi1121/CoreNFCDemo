#include "stm32f4xx.h"
#include "delay.h"
#include "key.h"

static int key_up = 1;//�����ɿ�״̬��ʶ

void KEY_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	//ʹ��GPIOA,GPIOAʱ��
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //KEY ��Ӧ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA
}

int KEY_Scan(int mode)
{
	if(mode)
	{
		key_up = 1;
	}
	if(key_up && (KEY0 == 0))
	{
		delay_ms(5); // ȥ����
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
	return -1; // �ް�������
}
