#include <string.h>
#include "HAL_conf.h"
#include "BSP.h"
#include "iwdg.h"
#include "mg_api.h"




unsigned char *ble_mac_addr;


unsigned char AdvDat[]=
{
    0x02,0x01,0x06,
    0x03,0x03,0xe7,0xfe,
    0x09,0xff,0x9c,0x05,0xff,0xff,0xff,0xff,0xff,0xff
};

int main(void)
{
    unsigned long temp=0x200000;
    unsigned char i;

    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.
    
    BSP_Init();

    Write_Iwdg_ON(IWDG_Prescaler_32, 0x4E2); //1s

    SetBleIntRunningMode();
    radio_initBle(TXPWR_0DBM, &ble_mac_addr);

#if 0 //reuse demo mac
    ble_mac_addr[2] = 0x28;
    ble_mac_addr[1] = 0xee;
    ble_mac_addr[0] = 0x2c;
    radio_setBleAddr(ble_mac_addr);
#endif

    for (i=0;i<6;i++)
    {
        AdvDat[sizeof(AdvDat)-i-1] = *(ble_mac_addr+i);
    }
    ble_set_adv_data(AdvDat, sizeof(AdvDat));

    ble_run_interrupt_start(160*2); //320*0.625=200ms
    
    /* Infinite loop */
    while (1)
    {
        IrqMcuGotoSleepAndWakeup();
        
        //usr proc
        //key_proc();
    }
}
