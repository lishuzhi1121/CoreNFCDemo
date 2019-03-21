#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "nfc.h"

//外部中断0初始化
void exti0_init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);//PA0 连接到中断线0
	
	 /* 配置EXTI_Line0 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0; //LINE0
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE; //使能LINE0
  EXTI_Init(&EXTI_InitStructure); //配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; //外部中断0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //子优先级1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
  NVIC_Init(&NVIC_InitStructure); //配置
}



//外部中断0服务程序
void EXTI0_IRQHandler(void)
{
	delay_ms(10);	//消抖
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
	 EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位 
}

