#include "timer.h"
#include "usart.h"
#include "led.h"
#include "beep.h"

// 定时器2初始化
void timer2_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//使能TIM2时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = 500 -1;
	TIM_TimeBaseStructure.TIM_Prescaler = 8400 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; //TIM2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //响应优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器
}

//定时器2中断服务函数
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET) //溢出中断
	{
		TIM_Cmd(TIM2, DISABLE); //关闭TIM2
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
		
		USART1_RX_STA |= 0x8000;
	}
}




// 定时器3初始化
void timer3_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//使能TIM3时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = 3000 -1;
	TIM_TimeBaseStructure.TIM_Prescaler = 8400 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //TIM3中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 3; //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //响应优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器
	
	//TIM3 使能
	TIM_Cmd(TIM3, ENABLE);
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET) //溢出中断
	{
		//TIM_Cmd(TIM3, DISABLE); //关闭TIM3
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
		
		//LED状态取反
		LED_Tiggle();
		BEEP_Tiggle();
		
	}
}



