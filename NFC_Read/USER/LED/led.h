#ifndef __LED_H
#define __LED_H
#include "sys.h"

// LED状态枚举
typedef enum {LED_ON = 1, LED_OFF = !LED_ON} LEDStatus;

//LED初始化
void LED_Config(void);

//LED开关
void LED_Switch(LEDStatus state);

//LED状态取反
void LED_Tiggle(void);

//LED2开关
void LED2_Switch(LEDStatus state);

//LED2状态取反
void LED2_Tiggle(void);

#endif

