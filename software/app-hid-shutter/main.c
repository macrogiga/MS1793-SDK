#include <string.h>
#include "HAL_conf.h"
#include "BSP.h"
#include "iwdg.h"
#include "mg_api.h"



extern volatile unsigned int SysTick_Count;

unsigned char *ble_mac_addr;


unsigned char pld_adv[] = {2, 0x1, 0x6, 3, 0x19, 0xc0, 0x03, 3, 0x3, 0x12, 0x18};


int main(void)
{
    unsigned long temp=0x200000;
    unsigned char *ft_val = (unsigned char *)(0x1FFFF804);
    unsigned char ft_value[2] = {0xc0, 0x12};

    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.
    
    BSP_Init();

    Write_Iwdg_ON(IWDG_Prescaler_32, 0x4E2); //1s

    SetBleIntRunningMode();
    radio_initBle(TXPWR_0DBM, &ble_mac_addr);
    
#if defined(MS1791_EVBOARD)
    if((*ft_val > 11) && (*ft_val < 27)){
        ft_value[1] = *ft_val;
        mg_activate(0x53);
        mg_writeBuf(0x4, ft_value, 2);
        mg_activate(0x56);
    }
#endif
    
    ble_set_adv_data(pld_adv, sizeof(pld_adv));
    
    SysTick_Count = 0;
    while(SysTick_Count < 5){}; //delay at least 5ms between radio_initBle() and ble_run...
    
    //ble_run(160*2); //320*0.625=200 ms
    ble_run_interrupt_start(160*2); //320*0.625=200 ms
    
    while(1)
    {
        IrqMcuGotoSleepAndWakeup();
        
        //user proc
    }
}
