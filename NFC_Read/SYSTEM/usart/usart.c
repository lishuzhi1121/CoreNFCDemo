#include "usart.h"

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
	int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
	x = x;
}

//重定义fputc函数
int fputc(int ch, FILE *f)
{
	while((USART2->SR&0X40)==0); //循环发送,直到发送完毕
	USART2->DR = (u8) ch;
	return ch;
}
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//初始化IO 串口1
//bound:波特率
void usart1_init(u32 baudrate)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//使能GPIOA、USART1时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

  //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = baudrate; //波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No; //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口1
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //响应优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器

#endif
}

// 向串口1发送length长度的数据data
void usart1_send(u8 *data, u8 length)
{
	for (u8 i = 0; i < length; i++)
			{
				USART_SendData(USART1, data[i]);
				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); // 等待发送完成
			}
}

//清零串口1接收缓冲区前 num 个字节
void clean_usart1_rx_buff(u16 num)
{
	for (u16 i = 0; i < num; i++)
	{
		USART1_RX_BUF[i] = 0x00;
	}
}

 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
u8 USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART1_RX_STA = 0;

//串口1中断服务程序
void USART1_IRQHandler(void)
{
	u16 res;
	//判断是否是串口1接收中断
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		//每次接收到数据时,定时器清零,并开始计时
		//当接收间隔超过定时器定的值时,进入TIM中断,表示一帧数据接收完成
		TIM2->CNT &= 0x0000;
		TIM_Cmd(TIM2, ENABLE);
		
		//读取接收到的数据
		res = USART_ReceiveData(USART1); //(USART1->DR)
		
		if((USART1_RX_STA & 0x8000) == 0)//接收未完成
		{
			USART1_RX_BUF[USART1_RX_STA & 0X3FFF] = res;
			USART1_RX_STA++;
			if(USART1_RX_STA > (USART1_REC_LEN - 1))
			{
				USART1_RX_STA = 0; //如果接收到的数据超出规定长度,则说明接收数据错误,重新开始接收
			}
		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE); //清除串口1中断标志位
  }
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//初始化IO 串口2
//bound:波特率
void usart2_init(u32 baudrate)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//使能GPIOA、USART2时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
 
	//串口2对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	//USART2端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2与GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA2，PA3

  //USART2 初始化设置
	USART_InitStructure.USART_BaudRate = baudrate; //波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No; //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART2, &USART_InitStructure); //初始化串口2
	
  USART_Cmd(USART2, ENABLE);  //使能串口2
	
	USART_ClearFlag(USART2, USART_FLAG_TC); //清除发送完成标识
	
#if EN_USART2_RX

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //开启相关中断
	
	//USART2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //响应优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器

#endif
}



// 向串口2发送length长度的数据data
void usart2_send(u8 *data, u8 length)
{
	for (u8 i = 0; i < length; i++)
			{
				USART_SendData(USART2, data[i]);
				while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); // 等待发送完成
			}
}

 
#if EN_USART2_RX   //如果使能了接收
//串口2中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART2_RX_STA = 0;

//串口2中断服务程序
void USART2_IRQHandler(void)
{
	u8 Res;
	
	//判断是否是接收中断
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		//读取接收到的数据
		Res =USART_ReceiveData(USART2); //(USART1->DR)
		
		//接收到的数据必须是回车(0x0d 0x0a)结尾
		if((USART2_RX_STA & 0x8000) == 0)//接收未完成
		{
			//判断前一次是否接收到了0x0d
			if(USART2_RX_STA & 0x4000)
			{
				//如果前一次接收到的是0x0d，则判断这一次是否接收到了0x0a
				if(Res!=0x0a)
				{
					USART2_RX_STA = 0;//如果这一次收到的不是0x0a,则说明接收错误,重新开始,接收状态置0
				}
				else
				{
					USART2_RX_STA |= 0x8000;	//如果这一次收到的是0x0a,则说明接收完成了,将状态值最高位置1
				}
			}
			else //还没收到0X0d
			{
				//判断当前接收到的是不是0x0d
				if(Res == 0x0d)
				{
					USART2_RX_STA |= 0x4000; //如果这一次收到的是0x0d,则将状态值次高位置1
				}
				else
				{
					USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = Res;
					USART2_RX_STA++;
					if(USART2_RX_STA > (USART2_REC_LEN-1))
					{
						USART2_RX_STA = 0; //如果接收到的数据超出规定长度,则说明接收数据错误,重新开始接收
					}
				}
			}
		}
		
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); //清除串口2中断标志位
  }
}

#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////



