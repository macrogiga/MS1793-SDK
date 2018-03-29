#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "HAL_conf.h"



#define DIR_CLOCKWISE          1
#define DIR_COUNTER_CLOCKWISE  2


void MOTOR_Init(void);
void MOTOR_Proc(void);

void MOTOR_Stop(void);
void MOTOR_Run(unsigned char direction);


#endif
