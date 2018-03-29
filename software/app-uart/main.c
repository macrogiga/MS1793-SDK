#include <string.h>
#include "HAL_conf.h"
#include "BSP.h"
#include "iwdg.h"
#include "mg_api.h"



extern void UartInit(UART_TypeDef* UARTx);
extern volatile unsigned int SysTick_Count;

unsigned char *ble_mac_addr;


//unsigned char pld_adv[] = {2,1,6, 3,0x08,'m','g'};


int main(void)
{
    unsigned long temp=0x200000;

    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.
    
    BSP_Init();
#ifdef USE_UART
	UartInit(UART1);
#endif
    
    Write_Iwdg_ON(IWDG_Prescaler_32, 0x4E2); //1s

    SetBleIntRunningMode();
    radio_initBle(TXPWR_0DBM, &ble_mac_addr);
    
    SysTick_Count = 0;
    while(SysTick_Count < 5){}; //delay at least 5ms between radio_initBle() and ble_run...

#ifdef USE_UART
	printf("\r\nMAC:%02x-%02x-%02x-%02x-%02x-%02x", ble_mac_addr[5],ble_mac_addr[4],ble_mac_addr[3],ble_mac_addr[2],ble_mac_addr[1],ble_mac_addr[0]);
#endif
    
    //ble_run(160*2); //320*0.625=200 ms
    ble_run_interrupt_start(160*2);
    
    while(1){
        IrqMcuGotoSleepAndWakeup();
        
    }
}
