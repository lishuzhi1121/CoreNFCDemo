#include "stm32f4xx.h"
#include "nfc.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "beep.h"

NFCReadWriteStatus read_write_state = NFC_Read;
u8 UID[4]; //存储UID
u8 ReadedData[16]; //存储当前读取到的数据

/**
  * @brief  唤醒
	* @retval NFCWakeUpStatus: 唤醒状态,成功时返回Success
  */
NFCWakeUpStatus nfc_WakeUp(void)
{
	u8 data[24]; //发送数据BUFF
	u8 checkCode = 0; //数据校验码
	u8 temp = 0; //数据校验和
	u8 len = 0; //接收数据长度
	u8 i = 0; //循环变量
	
	data[0]  = 0x55; //唤醒命令
  data[1]  = 0x55;
  data[2]  = 0x00;
  data[3]  = 0x00;
  data[4]  = 0x00;
  data[5]  = 0x00;
  data[6]  = 0x00;
  data[7]  = 0x00;
  data[8]  = 0x00;
  data[9]  = 0x00;
  data[10] = 0x00;
  data[11] = 0x00;
  data[12] = 0x00;
  data[13] = 0x00;
	
	data[14] = 0x00; //命令头
  data[15] = 0x00;
	data[16] = 0xFF;
  data[17] = 0x03; //包长度
  data[18] = 0xFD; //包长度校验
	
  data[19] = 0xD4; //数据传输方向
  data[20] = 0x14; //唤醒命令代码
  data[21] = 0x01;
  data[22] = 0x17; //DCS 数据校验码
  data[23] = 0x00;
	
	printf("\r\n Start WakeUp PN532 ...\r\n");
	
	while(1)
	{
		printf("\r\n Send: ");
		for (i = 0; i< 24; i++)
		{
			printf("%02X ", data[i]);
		}
		printf("\r\n\r\n"); //插入换行
		usart1_send(data, 24); //发送到串口1
		
		delay_ms(200); //延时等待PN532响应
		
		if (USART1_RX_STA & 0x8000)
		{
			len = USART1_RX_STA & 0x3FFF;
			if (len > 14)
			{
				for (i = 11; i < 13; i++)
				{
					temp += USART1_RX_BUF[i];
				}
				checkCode = 0x100 - temp;
				if (checkCode == USART1_RX_BUF[13])
				{
					// 唤醒成功
					printf("\r\n Recived: ");
					for (i = 0; i < len; i++)
					{
						printf("%02X ", USART1_RX_BUF[i]);
					}
					printf("\r\n\r\n"); //插入换行
					printf("\r\n WakeUp Success!!! \r\n");
					USART1_RX_STA = 0;
					clean_usart1_rx_buff(len);
					break;
				}
			}
		}
	}
	return Success;
}


/**
  * @brief  寻卡
	* @retval NFCInListStatus: 寻卡状态,成功时返回Success
  */
NFCInListStatus  nfc_InListPassiveTarget(void)
{
	u8 data[11]; //发送数据BUFF
	u8 checkCode = 0; //数据校验码
	u8 temp = 0; //数据校验和
	u8 len = 0; //接收数据长度
	u8 i = 0; //循环变量
	
	data[0]  = 0x00; //命令头
	data[1]  = 0x00;
	data[2]  = 0xFF;
	data[3]  = 0x04;
	data[4]  = 0xFC;
	
	data[5]  = 0xD4; //数据传输方向
	data[6]  = 0x4A; //寻卡命令代码
	data[7]  = 0x01; //寻卡数量,一般为1,最大为2
	data[8]  = 0x00;
	data[9]  = 0xE1; //DCS 数据校验码
	data[10] = 0x00;
	
	printf("\r\n Start InListPassiveTarget ...\r\n");
		
	while(1)
	{
		printf("\r\n Send: ");
		for (i = 0; i< 11; i++)
		{
			printf("%02X ", data[i]);
		}
		printf("\r\n"); //插入换行
	
		usart1_send(data, 24); //发送到串口1
		
		delay_ms(200); //延时等待PN532响应
		
		if (USART1_RX_STA & 0x8000)
		{
			len = USART1_RX_STA & 0x3FFF;
			if (len > 24)
			{
				for (i = 11; i < len - 2; i++)
				{
					temp += USART1_RX_BUF[i];
				}
				checkCode = 0x100 - temp;
				if (checkCode == USART1_RX_BUF[len - 2])
				{
					//寻卡成功
					printf("\r\n Recived: ");
					for (i = 0; i < len; i++)
					{
						printf("%02X ", USART1_RX_BUF[i]);
					}
					printf("\r\n\r\n"); //插入换行
					printf("\r\n InListPassiveTarget Success!!! \r\n");
					
					//保存UID
					UID[0] = USART1_RX_BUF[19];
					UID[1] = USART1_RX_BUF[20];
					UID[2] = USART1_RX_BUF[21];
					UID[3] = USART1_RX_BUF[22];
					printf("\r\n Target UID: ");
					for (i = 0; i < 4; i++)
					{
						printf("%02X ", UID[i]);
					}
					printf("\r\n\r\n"); //插入换行
					
					USART1_RX_STA = 0;
					clean_usart1_rx_buff(len);
					break;
				}
			}
			else
			{
				printf("\r\n Recived: ");
				for (i = 0; i < len; i++)
				{
					printf("%02X ", USART1_RX_BUF[i]);
				}
				printf("\r\n\r\n"); //插入换行
				printf("\r\n Waiting For Target ... \r\n");
				USART1_RX_STA = 0;
				clean_usart1_rx_buff(len);
				delay_ms(500);
			}
		}
	}
	return Success;
}


/**
  * @brief  验证密码
  * @param  blockNum: 要验证的块号
  * @retval None
  */
void nfc_PsdVerifyKeyB(u8 blockNum)
{
	u8 data[22]; //发送数据BUFF
	u8 checkCode = 0; //数据校验码
	u8 temp = 0; //数据校验和
	u8 len = 0; //接收数据长度
	u8 i = 0; //循环变量
	
	data[0]  = 0x00; //命令头
  data[1]  = 0x00;
  data[2]  = 0xFF;
  data[3]  = 0x0F;
  data[4]  = 0xF1;
	
  data[5]  = 0xD4; //数据传输方向
  data[6]  = 0x40; //命令代码
  data[7]  = 0x01;
  data[8]  = 0x60; //密码验证命令
  data[9]  = blockNum; //要验证的块号
	
  data[10] = 0xFF; //密码
  data[11] = 0xFF;
  data[12] = 0xFF;
  data[13] = 0xFF;
  data[14] = 0xFF;
  data[15] = 0xFF;
	
	data[16] = UID[0]; //UID
  data[17] = UID[1];
  data[18] = UID[2];
  data[19] = UID[3];
	
	// 计算校验码
	for (i = 5; i < 20; i++)
	{
		temp += data[i];
	}
  data[20] = 0x100 - temp;
  data[21] = 0x00;
	
	printf("\r\n Start PsdVerifyKeyB ...\r\n");
	printf("\r\n Send: ");
	for (i = 0; i< 22; i++)
	{
		printf("%02X ", data[i]);
	}
	printf("\r\n"); //插入换行
		
	usart1_send(data, 22); //发送到串口1
		
	delay_ms(200); //延时等待PN532响应
	
	if (USART1_RX_STA & 0x8000)
	{
		temp = 0;
		len = USART1_RX_STA & 0x3FFF;
		
		printf("\r\n Recived: ");
		for (i = 0; i < len; i++)
		{
			printf("%02X ", USART1_RX_BUF[i]);
		}
		printf("\r\n"); //插入换行
		
		if (len > 15)
		{
			for (i = 11; i < len - 2; i++)
			{
				temp += USART1_RX_BUF[i];
			}
			checkCode = 0x100 - temp;
			if (checkCode == USART1_RX_BUF[len - 2] && USART1_RX_BUF[13] == 0x00)
			{
				//验证成功
				printf("\r\n PsdVerifyKeyB Success!!! \r\n");
				USART1_RX_STA = 0;
				clean_usart1_rx_buff(len);
				
				// 根据当前状态操作
				if (read_write_state == NFC_Read)
				{
					//读取数据
					nfc_read(blockNum);
				}
				else
				{
					//写入数据
					u8 writeData[16];
					writeData[0] = 0x0F;
					for (i = 1; i < 16; i++)
					{
						writeData[i] = 0x00;
					}
					nfc_write(blockNum, writeData);
				}
			}
		}
	}
}


/**
  * @brief  读取数据
  * @param  blockNum: 要读取的块号
  * @retval None
  */
void nfc_read(u8 blockNum)
{
	u8 data[12]; //发送数据BUFF
	u8 checkCode = 0; //数据校验码
	u8 temp = 0; //数据校验和
	u8 len = 0; //接收数据长度
	u8 i = 0; //循环变量
	
	data[0]  = 0x00; //命令头
	data[1]  = 0x00;
	data[2]  = 0xFF;
	data[3]  = 0x05;
	data[4]  = 0xFB;
	
	data[5]  = 0xD4; //数据传输方向
	data[6]  = 0x40; //命令代码
	data[7]  = 0x01;
	data[8]  = 0x30; //读取命令代码
	data[9]  = blockNum; //读第几块的16个字节数据
	
	// 计算校验码
	for (i = 5; i < 10; i++)
	{
		temp += data[i];
	}
	data[10] = 0x100 - temp;
	data[11] = 0x00;
	
	printf("\r\n Start Read Data ...\r\n");
	
	while (1)
	{
		printf("\r\n Send: ");
		for (i = 0; i< 12; i++)
		{
			printf("%02X ", data[i]);
		}
		printf("\r\n"); //插入换行
		
		usart1_send(data, 12); //发送到串口1
		
		delay_ms(200); //延时等待PN532响应
		
		if (USART1_RX_STA & 0x8000)
		{
			temp = 0;
			len = USART1_RX_STA & 0x3FFF;
			
			printf("\r\n Recived: ");
			for (i = 0; i < len; i++)
			{
				printf("%02X ", USART1_RX_BUF[i]);
			}
			printf("\r\n"); //插入换行
			
			if (len > 31)
			{
				for (i = 11; i < len - 2; i++)
				{
					temp += USART1_RX_BUF[i];
				}
				checkCode = 0x100 - temp;
				if (checkCode == USART1_RX_BUF[len - 2] && USART1_RX_BUF[13] == 0x00)
				{
					//读取数据成功
					printf("\r\n Read Data Success!!! \r\n");
					for (i = 14; i < 30; i++)
					{
						ReadedData[i - 14] = USART1_RX_BUF[i];
					}
					printf("\r\n ReadedData: ");
					for (i = 0; i < 16; i++)
					{
						printf("%02X ", ReadedData[i]);
					}
					//////////////////
					
					TIM_Cmd(TIM3, DISABLE); //关闭TIM3
					LED_Switch(LED_ON); //点亮LED
					BEEP_Switch(BEEP_OFF); //关闭蜂鸣器
					USART1_RX_STA = 0;
					clean_usart1_rx_buff(len);
					break;
				}
			}
		}
	}
}

/**
  * @brief  写入数据
  * @param  blockNum: 要写入的块号
  * @param  writeData[16]: 要写入的数据(16个字节)
  * @retval None
  */
void nfc_write(u8 blockNum, u8 writeData[16])
{
	u8 data[28]; //发送数据BUFF
	u8 checkCode = 0; //数据校验码
	u8 temp = 0; //数据校验和
	u8 len = 0; //接收数据长度
	u8 i = 0; //循环变量
	
	data[0]  = 0x00; //命令头
	data[1]  = 0x00;
	data[2]  = 0xFF;
	data[3]  = 0x15;
	data[4]  = 0xEB;
	
	data[5]  = 0xD4; //数据传输方向
	data[6]  = 0x40; //命令代码
	data[7]  = 0x01;
	data[8]  = 0xA0; //写入命令代码
	data[9]  = blockNum; //写第几块的16个字节数据
	
	//填充要写入的数据
	for (i = 0; i < 16; i++)
	{
		data[i + 10] = writeData[i];
	}
	
	// 计算校验码
	for (i = 5; i < 26; i++)
	{
		temp += data[i];
	}
	data[26] = 0x100 - temp;
	data[27] = 0x00;
	
	printf("\r\n Start Write Data ...\r\n");
	
	while (1)
	{
		printf("\r\n Send: ");
		for (i = 0; i< 12; i++)
		{
			printf("%02X ", data[i]);
		}
		printf("\r\n"); //插入换行
		
		usart1_send(data, 28); //发送到串口1
		
		delay_ms(200); //延时等待PN532响应
		
		if (USART1_RX_STA & 0x8000)
		{
			temp = 0;
			len = USART1_RX_STA & 0x3FFF;
			
			printf("\r\n Recived: ");
			for (i = 0; i < len; i++)
			{
				printf("%02X ", USART1_RX_BUF[i]);
			}
			printf("\r\n"); //插入换行
			
			if (len > 15)
			{
				for (i = 11; i < len - 2; i++)
				{
					temp += USART1_RX_BUF[i];
				}
				checkCode = 0x100 - temp;
				if (checkCode == USART1_RX_BUF[len - 2] && USART1_RX_BUF[13] == 0x00)
				{
					//写入数据成功
					printf("\r\n Write Data Success!!! \r\n");
					
					TIM_Cmd(TIM3, DISABLE); //关闭TIM3
					LED_Switch(LED_ON); //点亮LED
					BEEP_Switch(BEEP_OFF); //关闭蜂鸣器
					USART1_RX_STA = 0;
					clean_usart1_rx_buff(len);
					break;
				}
			}
		}
	}
}


