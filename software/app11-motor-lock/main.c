#include <string.h>
#include "HAL_conf.h"
#include "BSP.h"
#include "iwdg.h"
#include "mg_api.h"
#include "motor.h"



extern volatile unsigned int SysTick_Count;

unsigned char *ble_mac_addr;

unsigned char pld_adv[] = {2,0x1,0x6, 17,0x15,0xd0,0x00,0x2d,0x12,0x1e,0x4b,0x0f,0xa4,0x99,0x4e,0xce,0xb5,0x31,0xf4,0x05,0x79};

unsigned char PinCode[6]={'1','2','3','4','5','6'};


int main(void)
{
    unsigned long temp=0x200000;

    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.
    
    BSP_Init();
    MOTOR_Init();

    SetBleIntRunningMode();
    radio_initBle(TXPWR_0DBM, &ble_mac_addr);
    
    ble_set_adv_data(pld_adv, sizeof(pld_adv));
    
    SetLePinCode(PinCode);
    
    SysTick_Count = 0;
    while(SysTick_Count < 5){}; //delay at least 5ms between radio_initBle() and ble_run...

    Write_Iwdg_ON(IWDG_Prescaler_32, 0x4E2); //1250*0.8ms=1s

    ble_run_interrupt_start(160*2); //320*0.625=200 ms
    
    while(1)
    {
        IrqMcuGotoSleepAndWakeup();
        
        MOTOR_Proc();
    }
}

