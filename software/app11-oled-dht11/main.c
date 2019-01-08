#include <stdio.h>
#include <string.h>
#include "HAL_conf.h"
#include "BSP.h"
#include "iwdg.h"
#include "mg_api.h"
#include "oled.h"


extern volatile unsigned int SysTick_Count;

unsigned char *ble_mac_addr;


const unsigned char AdvDat_HRS[]=
{
    0x02,0x01,0x06,
    0x03,0x19,0x41,0x03,
    0x07,0x03,0x0D,0x18,0x0A,0x18,0x0F,0x18
};


char Str[]="DHT11 --";

extern u16 Timer2S;



int main(void)
{
    u8 Line = 0;
    u16 ValHumi = 0xffff;
    u16 ValTemp = 0xffff;
    
    BSP_Init();

    CaptureInit();
    SysTick_Count = 0;
    while(SysTick_Count < 1000){}; //delay for OLED module init
    OLED_Init();
    
    SysTick_Count = 0;
    while(SysTick_Count < 1000){}; //delay for logo display
    
    SetBleIntRunningMode();
    radio_initBle(TXPWR_0DBM, &ble_mac_addr);
    
    ble_set_adv_data((unsigned char *)AdvDat_HRS, sizeof(AdvDat_HRS));

    SysTick_Count = 0;
    while(SysTick_Count < 5){}; //delay at least 5ms between radio_initBle() and ble_run...

    Write_Iwdg_ON(IWDG_Prescaler_32, 0x4E2); //1250*0.8ms=1s

    ble_run_interrupt_start(160*2);
    //ble_run(160*2); //320*0.625=200 ms

    OLED_Clear();
    Timer2S = 0;
    
    while(1)
    {
        IrqMcuGotoSleepAndWakeup();

        if(CaptureDataReady() == 0) ReadDHT();
        
        if (Timer2S >= 2000) //sampling&display interval
        {
            Timer2S = 0;

            if(CaptureDataGet(&ValHumi, &ValTemp)){
                sprintf(Str,"H:%d.%d%% T:%d.%d'C",ValHumi/10,ValHumi%10,ValTemp/10,ValTemp%10);
            }else{
                CaptureStop(); //recover from exception
                Timer2S = 0;
                while(Timer2S < 500){};
                
                sprintf(Str," No Data");
            }

            if (Line > 7)
            {
                Line = 0;
                OLED_Clear();
            }
            OLED_DispStr(0,Line,Str);
            Line += 2; //+1 if use 6*8 dot, +2 if use 8*16 dot
        }
    }
}

