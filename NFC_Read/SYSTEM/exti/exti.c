#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "nfc.h"

//�ⲿ�ж�0��ʼ��
void exti0_init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);//PA0 ���ӵ��ж���0
	
	 /* ����EXTI_Line0 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0; //LINE0
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE; //ʹ��LINE0
  EXTI_Init(&EXTI_InitStructure); //����
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; //�ⲿ�ж�0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //�����ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure); //����
}



//�ⲿ�ж�0�������
void EXTI0_IRQHandler(void)
{
	delay_ms(10);	//����
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 1)
	{
		LED2_Tiggle();
		
		if (read_write_state == NFC_Read)
		{
			read_write_state = NFC_Write;
		}
		else
		{
			read_write_state = NFC_Read;
		}
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line0); //���LINE0�ϵ��жϱ�־λ 
}

