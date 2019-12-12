#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "HAL_conf.h"


#if defined(MS1793_EVBOARD)
  #define MOTOR_BI_PIN GPIO_Pin_2
  #define MOTOR_BI_PIN_SOURCE GPIO_PinSource2
  #define MOTOR_FI_PIN GPIO_Pin_3
  #define MOTOR_FI_PIN_SOURCE GPIO_PinSource3
  #define MOTOR_PORT GPIOD
#elif defined(MS1791_EVBOARD)
  #define MOTOR_BI_PIN GPIO_Pin_8
  #define MOTOR_BI_PIN_SOURCE GPIO_PinSource8
  #define MOTOR_FI_PIN GPIO_Pin_9
  #define MOTOR_FI_PIN_SOURCE GPIO_PinSource9
  #define MOTOR_PORT GPIOB
#else //default
  #define MOTOR_BI_PIN GPIO_Pin_2
  #define MOTOR_BI_PIN_SOURCE GPIO_PinSource2
  #define MOTOR_FI_PIN GPIO_Pin_3
  #define MOTOR_FI_PIN_SOURCE GPIO_PinSource3
  #define MOTOR_PORT GPIOD
#endif


#define DIR_CLOCKWISE          1
#define DIR_COUNTER_CLOCKWISE  2


void MOTOR_Init(void);
void MOTOR_Proc(void);

void MOTOR_Stop(void);
void MOTOR_Run(unsigned char direction);


#endif
