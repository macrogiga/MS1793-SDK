#include <string.h>
#include <stdlib.h>
#include "HAL_conf.h"
#include "BSP.h"
#include "mg_api.h"


unsigned char *ble_mac_addr;
unsigned char* get_local_addr(void) //used for ble pairing case
{
    return ble_mac_addr;
}


const unsigned char OTAAdvDat[]=
{
    0x02,0x01,0x06,
    0x03,0x03,0x90,0xfe};
unsigned char OTARspDat[]=
{
    0x4,0x09,'O','T','A',
    0x05,0xff,0xcd,0xab,0x23,0x56};
    
int main(void)
{
    unsigned long temp=0x180000;

    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.
    
    BSP_Init();

    radio_initBle(TXPWR_0DBM, &ble_mac_addr);
    
    ble_set_adv_data((unsigned char *)OTAAdvDat,sizeof(OTAAdvDat));
    ble_set_adv_rsp_data((unsigned char *)OTARspDat,sizeof(OTARspDat));

    
    ble_run(200); //200*0.625=125 ms
}
