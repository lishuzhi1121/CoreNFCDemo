#ifndef __BEEP_H
#define __BEEP_H
#include "sys.h"

// 蜂鸣器状态枚举
typedef enum {BEEP_ON = 1, BEEP_OFF = !BEEP_ON} BEEPStatus;

//蜂鸣器初始化
void BEEP_Config(void);

//蜂鸣器开关
void BEEP_Switch(BEEPStatus state);

//蜂鸣器状态取反
void BEEP_Tiggle(void);

#endif

