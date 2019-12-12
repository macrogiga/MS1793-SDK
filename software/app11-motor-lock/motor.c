/*
    Copyright (c) 2018 Macrogiga Electronics Co., Ltd.

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.
*/

#include <string.h>
#include "BSP.h"
#include "motor.h"


extern unsigned char LockFlag;
extern u8 Password_wr[];

void MOTOR_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    //motor module
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    GPIO_PinAFConfig(MOTOR_PORT,MOTOR_BI_PIN_SOURCE,GPIO_AF_3);
    GPIO_PinAFConfig(MOTOR_PORT,MOTOR_FI_PIN_SOURCE,GPIO_AF_3);
    GPIO_InitStructure.GPIO_Pin  = MOTOR_BI_PIN | MOTOR_FI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MOTOR_PORT, &GPIO_InitStructure);
    
    GPIO_ResetBits(MOTOR_PORT, MOTOR_BI_PIN);
    GPIO_ResetBits(MOTOR_PORT, MOTOR_FI_PIN);
}

static void MOTOR_Stop(void)
{
    GPIO_ResetBits(MOTOR_PORT, MOTOR_BI_PIN);
    GPIO_ResetBits(MOTOR_PORT, MOTOR_FI_PIN);
}

static void MOTOR_Run(unsigned char direction) 
{
    switch(direction)
    {
        case DIR_CLOCKWISE:
            GPIO_SetBits(MOTOR_PORT, MOTOR_BI_PIN);
            GPIO_ResetBits(MOTOR_PORT, MOTOR_FI_PIN);
            break;
        
        case DIR_COUNTER_CLOCKWISE:
            GPIO_ResetBits(MOTOR_PORT, MOTOR_BI_PIN);
            GPIO_SetBits(MOTOR_PORT, MOTOR_FI_PIN);
            break;
        
        default:
            break;
    }

}


static u8 Passwd_in[4]={'1','2','3','4'}; //example: 4 digit password

void MOTOR_Proc(void)
{
    u8 Unlock = 0;
    
    if ((Password_wr[0] == 4) && (0 == memcmp(Passwd_in,&Password_wr[1],Password_wr[0])))
    {
        Unlock = 1;
        Password_wr[0] = 0; //invalid the passwd
    }
    
    if (Unlock)
    {
        MOTOR_Run(DIR_CLOCKWISE);
    }
    
    if (GPIO_ReadInputData(GPIOA) & 0x0001) //unlock ok simulation
    {
        MOTOR_Stop();
        Unlock = 0;
        LockFlag = 0;
    }
    else
    {
        LockFlag = 1;
    }
}
