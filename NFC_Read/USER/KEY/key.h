#ifndef __KEY_H
#define __KEY_H
#include "sys.h"

// °´¼üÖµ
#define KEY0 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) // PA0

void KEY_Config(void);

int KEY_Scan(int mode);

#endif
