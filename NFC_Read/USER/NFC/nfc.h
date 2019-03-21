#ifndef __NFC_H
#define __NFC_H

#include "stdio.h"
#include "sys.h"

// �¼�״̬ö��
typedef enum {Success = 1, Failure = !Success} NFCWakeUpStatus, NFCInListStatus;

// ��д״̬ö��
typedef enum {NFC_Read = 1, NFC_Write = !NFC_Read} NFCReadWriteStatus;

extern NFCReadWriteStatus read_write_state;

/**
  * @brief  ����
	* @retval NFCWakeUpStatus: ����״̬,�ɹ�ʱ����Success
  */
NFCWakeUpStatus nfc_WakeUp(void);

/**
  * @brief  Ѱ��
	* @retval NFCInListStatus: Ѱ��״̬,�ɹ�ʱ����Success
  */
NFCInListStatus nfc_InListPassiveTarget(void);

/**
  * @brief  ��֤����
  * @param  blockNum: Ҫ��֤�Ŀ��
  * @retval None
  */
void nfc_PsdVerifyKeyB(u8 blockNum);

/**
  * @brief  ��ȡ����
  * @param  blockNum: Ҫ��ȡ�Ŀ��
  * @retval None
  */
void nfc_read(u8 blockNum);

/**
  * @brief  д������
  * @param  blockNum: Ҫд��Ŀ��
  * @param  writeData[16]: Ҫд�������(16���ֽ�)
  * @retval None
  */
void nfc_write(u8 blockNum, u8 writeData[16]);

#endif

