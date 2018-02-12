#ifndef _OLED_H_
#define _OLED_H_

#include "HAL_conf.h"





void I2C_Send(u8 *pdat, u8 len);


void CaptureInit(void);
void CaptureReset(void);
void CaptureStart(void);
void CaptureStop(void);
u8 CaptureDataReady(void);
u8 CaptureDataGet(u16* humi, u16* temp);
u8 CaptureDataMon(u16* humi, u16* temp);
void ReadDHT(void);

void OLED_Init(void);
void OLED_Clear(void);
void OLED_DispStr(unsigned char x, unsigned char y, char *ch);

#endif
