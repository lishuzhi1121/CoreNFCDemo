#ifndef __BEEP_H
#define __BEEP_H
#include "sys.h"

// ������״̬ö��
typedef enum {BEEP_ON = 1, BEEP_OFF = !BEEP_ON} BEEPStatus;

//��������ʼ��
void BEEP_Config(void);

//����������
void BEEP_Switch(BEEPStatus state);

//������״̬ȡ��
void BEEP_Tiggle(void);

#endif

