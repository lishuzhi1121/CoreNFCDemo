#include "usart.h"

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
	int handle;
};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
	x = x;
}

//�ض���fputc����
int fputc(int ch, FILE *f)
{
	while((USART2->SR&0X40)==0); //ѭ������,ֱ���������
	USART2->DR = (u8) ch;
	return ch;
}
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//��ʼ��IO ����1
//bound:������
void usart1_init(u32 baudrate)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//ʹ��GPIOA��USART1ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

  //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = baudrate; //����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No; //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //��Ӧ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��NVIC�Ĵ���

#endif
}

// �򴮿�1����length���ȵ�����data
void usart1_send(u8 *data, u8 length)
{
	for (u8 i = 0; i < length; i++)
			{
				USART_SendData(USART1, data[i]);
				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); // �ȴ��������
			}
}

//���㴮��1���ջ�����ǰ num ���ֽ�
void clean_usart1_rx_buff(u16 num)
{
	for (u16 i = 0; i < num; i++)
	{
		USART1_RX_BUF[i] = 0x00;
	}
}

 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
u8 USART1_RX_BUF[USART1_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.

//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART1_RX_STA = 0;

//����1�жϷ������
void USART1_IRQHandler(void)
{
	u16 res;
	//�ж��Ƿ��Ǵ���1�����ж�
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		//ÿ�ν��յ�����ʱ,��ʱ������,����ʼ��ʱ
		//�����ռ��������ʱ������ֵʱ,����TIM�ж�,��ʾһ֡���ݽ������
		TIM2->CNT &= 0x0000;
		TIM_Cmd(TIM2, ENABLE);
		
		//��ȡ���յ�������
		res = USART_ReceiveData(USART1); //(USART1->DR)
		
		if((USART1_RX_STA & 0x8000) == 0)//����δ���
		{
			USART1_RX_BUF[USART1_RX_STA & 0X3FFF] = res;
			USART1_RX_STA++;
			if(USART1_RX_STA > (USART1_REC_LEN - 1))
			{
				USART1_RX_STA = 0; //������յ������ݳ����涨����,��˵���������ݴ���,���¿�ʼ����
			}
		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE); //�������1�жϱ�־λ
  }
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//��ʼ��IO ����2
//bound:������
void usart2_init(u32 baudrate)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//ʹ��GPIOA��USART2ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
 
	//����2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	//USART2�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2��GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA2��PA3

  //USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = baudrate; //����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No; //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	
  USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���2
	
	USART_ClearFlag(USART2, USART_FLAG_TC); //���������ɱ�ʶ
	
#if EN_USART2_RX

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //��������ж�
	
	//USART2 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //����2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //��Ӧ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��NVIC�Ĵ���

#endif
}



// �򴮿�2����length���ȵ�����data
void usart2_send(u8 *data, u8 length)
{
	for (u8 i = 0; i < length; i++)
			{
				USART_SendData(USART2, data[i]);
				while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); // �ȴ��������
			}
}

 
#if EN_USART2_RX   //���ʹ���˽���
//����2�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.

//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART2_RX_STA = 0;

//����2�жϷ������
void USART2_IRQHandler(void)
{
	u8 Res;
	
	//�ж��Ƿ��ǽ����ж�
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		//��ȡ���յ�������
		Res =USART_ReceiveData(USART2); //(USART1->DR)
		
		//���յ������ݱ����ǻس�(0x0d 0x0a)��β
		if((USART2_RX_STA & 0x8000) == 0)//����δ���
		{
			//�ж�ǰһ���Ƿ���յ���0x0d
			if(USART2_RX_STA & 0x4000)
			{
				//���ǰһ�ν��յ�����0x0d�����ж���һ���Ƿ���յ���0x0a
				if(Res!=0x0a)
				{
					USART2_RX_STA = 0;//�����һ���յ��Ĳ���0x0a,��˵�����մ���,���¿�ʼ,����״̬��0
				}
				else
				{
					USART2_RX_STA |= 0x8000;	//�����һ���յ�����0x0a,��˵�����������,��״ֵ̬���λ��1
				}
			}
			else //��û�յ�0X0d
			{
				//�жϵ�ǰ���յ����ǲ���0x0d
				if(Res == 0x0d)
				{
					USART2_RX_STA |= 0x4000; //�����һ���յ�����0x0d,��״ֵ̬�θ�λ��1
				}
				else
				{
					USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = Res;
					USART2_RX_STA++;
					if(USART2_RX_STA > (USART2_REC_LEN-1))
					{
						USART2_RX_STA = 0; //������յ������ݳ����涨����,��˵���������ݴ���,���¿�ʼ����
					}
				}
			}
		}
		
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); //�������2�жϱ�־λ
  }
}

#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////



