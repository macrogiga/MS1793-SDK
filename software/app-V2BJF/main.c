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

unsigned char pld_adv[] = {2,1,6, 3,3,0x90,0xfe,0x09 ,0xFF ,0x11 ,0x02 ,0x11 ,0x02 ,0x93 ,0xF5 ,0x40 ,0xE1};
unsigned char len_adv = 17; //max 31

int main(void)
{
    unsigned long temp=0x400000;

    BSP_Init();
    while(temp--);//delay if necessary
    Write_Iwdg_ON(IWDG_Prescaler_32, 0x4E2); //1s

    ble_set_adv_data(pld_adv, len_adv); 
    
    radio_initBle(TXPWR_0DBM, &ble_mac_addr);
    radio_setXtal(8,0);
    
    ble_run(160*2); //320*0.625=200 ms
    
}
