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


int main(void)
{
    unsigned long temp=0x200000;

    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.
    
    BSP_Init();  

    Write_Iwdg_ON(IWDG_Prescaler_32, 0x4E2); //1s

    radio_initBle(TXPWR_0DBM, &ble_mac_addr);
    
    ble_run(160*2); //320*0.625=200 ms
}
