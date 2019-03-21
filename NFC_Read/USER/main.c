#include "stm32f4xx.h"
#include "sys.h"
#include "stdio.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "beep.h"
#include "timer.h"
#include "nfc.h"
#include "exti.h"


// main函数
int main(void)
{
	//设置中断优先级分组为2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	delay_init(72); //Systick初始化
	
	usart1_init(115200); //串口1初始化
	usart2_init(115200); //串口2初始化
	
	timer2_init(); //TIM2初始化
	timer3_init(); //TIM3初始化
	
	exti0_init(); //外部中断0初始化
	
	LED_Config(); //LED初始化
	KEY_Config();
	BEEP_Config(); //BEEP初始化
	
	NFCWakeUpStatus nfc_res = nfc_WakeUp();
	while (1)
	{
		if (nfc_res == Success)
		{
			while (1)
			{
				NFCInListStatus nfc_inlist_res = nfc_InListPassiveTarget();
				if (nfc_inlist_res == Success)
				{
					nfc_PsdVerifyKeyB(2);
					break;
				}
			}
			delay_ms(5000);
		}
	}
}


