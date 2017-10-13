#include <string.h>
#include "HAL_conf.h"
#include "BSP.h"
#include "iwdg.h"
#include "mg_api.h"



extern void UartInit(UART_TypeDef* UARTx);

unsigned char *ble_mac_addr;
unsigned char* get_local_addr(void) //used for ble pairing case
{
    return ble_mac_addr;
}

//unsigned char pld_adv[] = {2, 1, 6, 3, 0xA, 'm', 'g'};
//unsigned char len_adv = 7; //max 31


int main(void)
{
    unsigned long temp=0x200000;

    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.
    
    BSP_Init();  

    Write_Iwdg_ON(IWDG_Prescaler_32, 0x4E2); //1s

    radio_initBle(TXPWR_0DBM, &ble_mac_addr);  
    
    //SetFixAdvChannel(1);
    
#ifdef USE_UART
	UartInit(UART1);
	printf("\r\nMAC:%02x-%02x-%02x-%02x-%02x-%02x", ble_mac_addr[5],ble_mac_addr[4],ble_mac_addr[3],ble_mac_addr[2],ble_mac_addr[1],ble_mac_addr[0]);
#endif
    
    ble_run(160*2); //320*0.625=200 ms
}
