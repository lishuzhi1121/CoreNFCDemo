#ifndef __NFC_H
#define __NFC_H

#include "stdio.h"
#include "sys.h"

// 事件状态枚举
typedef enum {Success = 1, Failure = !Success} NFCWakeUpStatus, NFCInListStatus;

// 读写状态枚举
typedef enum {NFC_Read = 1, NFC_Write = !NFC_Read} NFCReadWriteStatus;

extern NFCReadWriteStatus read_write_state;

/**
  * @brief  唤醒
	* @retval NFCWakeUpStatus: 唤醒状态,成功时返回Success
  */
NFCWakeUpStatus nfc_WakeUp(void);

/**
  * @brief  寻卡
	* @retval NFCInListStatus: 寻卡状态,成功时返回Success
  */
NFCInListStatus nfc_InListPassiveTarget(void);

/**
  * @brief  验证密码
  * @param  blockNum: 要验证的块号
  * @retval None
  */
void nfc_PsdVerifyKeyB(u8 blockNum);

/**
  * @brief  读取数据
  * @param  blockNum: 要读取的块号
  * @retval None
  */
void nfc_read(u8 blockNum);

/**
  * @brief  写入数据
  * @param  blockNum: 要写入的块号
  * @param  writeData[16]: 要写入的数据(16个字节)
  * @retval None
  */
void nfc_write(u8 blockNum, u8 writeData[16]);

#endif

