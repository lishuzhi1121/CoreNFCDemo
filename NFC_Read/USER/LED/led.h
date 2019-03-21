#ifndef __LED_H
#define __LED_H
#include "sys.h"

// LED״̬ö��
typedef enum {LED_ON = 1, LED_OFF = !LED_ON} LEDStatus;

//LED��ʼ��
void LED_Config(void);

//LED����
void LED_Switch(LEDStatus state);

//LED״̬ȡ��
void LED_Tiggle(void);

//LED2����
void LED2_Switch(LEDStatus state);

//LED2״̬ȡ��
void LED2_Tiggle(void);

#endif

