#include "stm32f4xx.h"
#include "nfc.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "beep.h"

NFCReadWriteStatus read_write_state = NFC_Read;
u8 UID[4]; //�洢UID
u8 ReadedData[16]; //�洢��ǰ��ȡ��������

/**
  * @brief  ����
	* @retval NFCWakeUpStatus: ����״̬,�ɹ�ʱ����Success
  */
NFCWakeUpStatus nfc_WakeUp(void)
{
	u8 data[24]; //��������BUFF
	u8 checkCode = 0; //����У����
	u8 temp = 0; //����У���
	u8 len = 0; //�������ݳ���
	u8 i = 0; //ѭ������
	
	data[0]  = 0x55; //��������
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
	
	data[14] = 0x00; //����ͷ
  data[15] = 0x00;
	data[16] = 0xFF;
  data[17] = 0x03; //������
  data[18] = 0xFD; //������У��
	
  data[19] = 0xD4; //���ݴ��䷽��
  data[20] = 0x14; //�����������
  data[21] = 0x01;
  data[22] = 0x17; //DCS ����У����
  data[23] = 0x00;
	
	printf("\r\n Start WakeUp PN532 ...\r\n");
	
	while(1)
	{
		printf("\r\n Send: ");
		for (i = 0; i< 24; i++)
		{
			printf("%02X ", data[i]);
		}
		printf("\r\n\r\n"); //���뻻��
		usart1_send(data, 24); //���͵�����1
		
		delay_ms(200); //��ʱ�ȴ�PN532��Ӧ
		
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
					// ���ѳɹ�
					printf("\r\n Recived: ");
					for (i = 0; i < len; i++)
					{
						printf("%02X ", USART1_RX_BUF[i]);
					}
					printf("\r\n\r\n"); //���뻻��
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
  * @brief  Ѱ��
	* @retval NFCInListStatus: Ѱ��״̬,�ɹ�ʱ����Success
  */
NFCInListStatus  nfc_InListPassiveTarget(void)
{
	u8 data[11]; //��������BUFF
	u8 checkCode = 0; //����У����
	u8 temp = 0; //����У���
	u8 len = 0; //�������ݳ���
	u8 i = 0; //ѭ������
	
	data[0]  = 0x00; //����ͷ
	data[1]  = 0x00;
	data[2]  = 0xFF;
	data[3]  = 0x04;
	data[4]  = 0xFC;
	
	data[5]  = 0xD4; //���ݴ��䷽��
	data[6]  = 0x4A; //Ѱ���������
	data[7]  = 0x01; //Ѱ������,һ��Ϊ1,���Ϊ2
	data[8]  = 0x00;
	data[9]  = 0xE1; //DCS ����У����
	data[10] = 0x00;
	
	printf("\r\n Start InListPassiveTarget ...\r\n");
		
	while(1)
	{
		printf("\r\n Send: ");
		for (i = 0; i< 11; i++)
		{
			printf("%02X ", data[i]);
		}
		printf("\r\n"); //���뻻��
	
		usart1_send(data, 24); //���͵�����1
		
		delay_ms(200); //��ʱ�ȴ�PN532��Ӧ
		
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
					//Ѱ���ɹ�
					printf("\r\n Recived: ");
					for (i = 0; i < len; i++)
					{
						printf("%02X ", USART1_RX_BUF[i]);
					}
					printf("\r\n\r\n"); //���뻻��
					printf("\r\n InListPassiveTarget Success!!! \r\n");
					
					//����UID
					UID[0] = USART1_RX_BUF[19];
					UID[1] = USART1_RX_BUF[20];
					UID[2] = USART1_RX_BUF[21];
					UID[3] = USART1_RX_BUF[22];
					printf("\r\n Target UID: ");
					for (i = 0; i < 4; i++)
					{
						printf("%02X ", UID[i]);
					}
					printf("\r\n\r\n"); //���뻻��
					
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
				printf("\r\n\r\n"); //���뻻��
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
  * @brief  ��֤����
  * @param  blockNum: Ҫ��֤�Ŀ��
  * @retval None
  */
void nfc_PsdVerifyKeyB(u8 blockNum)
{
	u8 data[22]; //��������BUFF
	u8 checkCode = 0; //����У����
	u8 temp = 0; //����У���
	u8 len = 0; //�������ݳ���
	u8 i = 0; //ѭ������
	
	data[0]  = 0x00; //����ͷ
  data[1]  = 0x00;
  data[2]  = 0xFF;
  data[3]  = 0x0F;
  data[4]  = 0xF1;
	
  data[5]  = 0xD4; //���ݴ��䷽��
  data[6]  = 0x40; //�������
  data[7]  = 0x01;
  data[8]  = 0x60; //������֤����
  data[9]  = blockNum; //Ҫ��֤�Ŀ��
	
  data[10] = 0xFF; //����
  data[11] = 0xFF;
  data[12] = 0xFF;
  data[13] = 0xFF;
  data[14] = 0xFF;
  data[15] = 0xFF;
	
	data[16] = UID[0]; //UID
  data[17] = UID[1];
  data[18] = UID[2];
  data[19] = UID[3];
	
	// ����У����
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
	printf("\r\n"); //���뻻��
		
	usart1_send(data, 22); //���͵�����1
		
	delay_ms(200); //��ʱ�ȴ�PN532��Ӧ
	
	if (USART1_RX_STA & 0x8000)
	{
		temp = 0;
		len = USART1_RX_STA & 0x3FFF;
		
		printf("\r\n Recived: ");
		for (i = 0; i < len; i++)
		{
			printf("%02X ", USART1_RX_BUF[i]);
		}
		printf("\r\n"); //���뻻��
		
		if (len > 15)
		{
			for (i = 11; i < len - 2; i++)
			{
				temp += USART1_RX_BUF[i];
			}
			checkCode = 0x100 - temp;
			if (checkCode == USART1_RX_BUF[len - 2] && USART1_RX_BUF[13] == 0x00)
			{
				//��֤�ɹ�
				printf("\r\n PsdVerifyKeyB Success!!! \r\n");
				USART1_RX_STA = 0;
				clean_usart1_rx_buff(len);
				
				// ���ݵ�ǰ״̬����
				if (read_write_state == NFC_Read)
				{
					//��ȡ����
					nfc_read(blockNum);
				}
				else
				{
					//д������
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
  * @brief  ��ȡ����
  * @param  blockNum: Ҫ��ȡ�Ŀ��
  * @retval None
  */
void nfc_read(u8 blockNum)
{
	u8 data[12]; //��������BUFF
	u8 checkCode = 0; //����У����
	u8 temp = 0; //����У���
	u8 len = 0; //�������ݳ���
	u8 i = 0; //ѭ������
	
	data[0]  = 0x00; //����ͷ
	data[1]  = 0x00;
	data[2]  = 0xFF;
	data[3]  = 0x05;
	data[4]  = 0xFB;
	
	data[5]  = 0xD4; //���ݴ��䷽��
	data[6]  = 0x40; //�������
	data[7]  = 0x01;
	data[8]  = 0x30; //��ȡ�������
	data[9]  = blockNum; //���ڼ����16���ֽ�����
	
	// ����У����
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
		printf("\r\n"); //���뻻��
		
		usart1_send(data, 12); //���͵�����1
		
		delay_ms(200); //��ʱ�ȴ�PN532��Ӧ
		
		if (USART1_RX_STA & 0x8000)
		{
			temp = 0;
			len = USART1_RX_STA & 0x3FFF;
			
			printf("\r\n Recived: ");
			for (i = 0; i < len; i++)
			{
				printf("%02X ", USART1_RX_BUF[i]);
			}
			printf("\r\n"); //���뻻��
			
			if (len > 31)
			{
				for (i = 11; i < len - 2; i++)
				{
					temp += USART1_RX_BUF[i];
				}
				checkCode = 0x100 - temp;
				if (checkCode == USART1_RX_BUF[len - 2] && USART1_RX_BUF[13] == 0x00)
				{
					//��ȡ���ݳɹ�
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
					
					TIM_Cmd(TIM3, DISABLE); //�ر�TIM3
					LED_Switch(LED_ON); //����LED
					BEEP_Switch(BEEP_OFF); //�رշ�����
					USART1_RX_STA = 0;
					clean_usart1_rx_buff(len);
					break;
				}
			}
		}
	}
}

/**
  * @brief  д������
  * @param  blockNum: Ҫд��Ŀ��
  * @param  writeData[16]: Ҫд�������(16���ֽ�)
  * @retval None
  */
void nfc_write(u8 blockNum, u8 writeData[16])
{
	u8 data[28]; //��������BUFF
	u8 checkCode = 0; //����У����
	u8 temp = 0; //����У���
	u8 len = 0; //�������ݳ���
	u8 i = 0; //ѭ������
	
	data[0]  = 0x00; //����ͷ
	data[1]  = 0x00;
	data[2]  = 0xFF;
	data[3]  = 0x15;
	data[4]  = 0xEB;
	
	data[5]  = 0xD4; //���ݴ��䷽��
	data[6]  = 0x40; //�������
	data[7]  = 0x01;
	data[8]  = 0xA0; //д���������
	data[9]  = blockNum; //д�ڼ����16���ֽ�����
	
	//���Ҫд�������
	for (i = 0; i < 16; i++)
	{
		data[i + 10] = writeData[i];
	}
	
	// ����У����
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
		printf("\r\n"); //���뻻��
		
		usart1_send(data, 28); //���͵�����1
		
		delay_ms(200); //��ʱ�ȴ�PN532��Ӧ
		
		if (USART1_RX_STA & 0x8000)
		{
			temp = 0;
			len = USART1_RX_STA & 0x3FFF;
			
			printf("\r\n Recived: ");
			for (i = 0; i < len; i++)
			{
				printf("%02X ", USART1_RX_BUF[i]);
			}
			printf("\r\n"); //���뻻��
			
			if (len > 15)
			{
				for (i = 11; i < len - 2; i++)
				{
					temp += USART1_RX_BUF[i];
				}
				checkCode = 0x100 - temp;
				if (checkCode == USART1_RX_BUF[len - 2] && USART1_RX_BUF[13] == 0x00)
				{
					//д�����ݳɹ�
					printf("\r\n Write Data Success!!! \r\n");
					
					TIM_Cmd(TIM3, DISABLE); //�ر�TIM3
					LED_Switch(LED_ON); //����LED
					BEEP_Switch(BEEP_OFF); //�رշ�����
					USART1_RX_STA = 0;
					clean_usart1_rx_buff(len);
					break;
				}
			}
		}
	}
}


