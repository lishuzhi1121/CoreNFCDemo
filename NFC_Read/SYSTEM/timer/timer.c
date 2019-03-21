#include "timer.h"
#include "usart.h"
#include "led.h"
#include "beep.h"

// ��ʱ��2��ʼ��
void timer2_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//ʹ��TIM2ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = 500 -1;
	TIM_TimeBaseStructure.TIM_Prescaler = 8400 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; //TIM2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //��Ӧ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
}

//��ʱ��2�жϷ�����
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET) //����ж�
	{
		TIM_Cmd(TIM2, DISABLE); //�ر�TIM2
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����жϱ�־λ
		
		USART1_RX_STA |= 0x8000;
	}
}




// ��ʱ��3��ʼ��
void timer3_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//ʹ��TIM3ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = 3000 -1;
	TIM_TimeBaseStructure.TIM_Prescaler = 8400 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //TIM3�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 3; //��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //��Ӧ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
	
	//TIM3 ʹ��
	TIM_Cmd(TIM3, ENABLE);
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET) //����ж�
	{
		//TIM_Cmd(TIM3, DISABLE); //�ر�TIM3
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
		
		//LED״̬ȡ��
		LED_Tiggle();
		BEEP_Tiggle();
		
	}
}



