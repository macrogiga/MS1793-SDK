#include <string.h>
#include "HAL_conf.h"
#include "BSP.h"
#include "iwdg.h"
#include "mg_api.h"



unsigned char *ble_mac_addr;
unsigned char* get_local_addr(void) //used for ble pairing case
{
    return ble_mac_addr;
}

unsigned char pld_adv[] = {2, 0x1, 0x6, 3, 0x19, 0xc1, 0x03, 3, 0x3, 0x12, 0x18};
unsigned char len_adv = 11; //max 31


int main(void)
{
    unsigned long temp=0x200000;

    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.
    
    BSP_Init();  

    Write_Iwdg_ON(IWDG_Prescaler_32, 0x4E2); //1s

    SetBleIntRunningMode();
    radio_initBle(TXPWR_0DBM, &ble_mac_addr);  
     
    ble_set_adv_data(pld_adv, len_adv);
    
    //ble_run(160*2); //320*0.625=200 ms
    ble_run_interrupt_start(160*2); //320*0.625=200 ms
    
    while(1)
    {
        if(ble_run_interrupt_McuCanSleep())
        {
            //to do MCU sleep and wakeup steps
        }
    }
}
