#include <string.h>
#include <stdio.h>
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
    unsigned char *ft_val = (unsigned char *)(0x1FFFF804);
    unsigned char ft_value[2] = {0xc0, 0x12};

    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.
    
    BSP_Init();
    
#ifdef USE_UART
    UartInit(UART1);
    NVIC_SetPriority (UART1_IRQn, (1<<__NVIC_PRIO_BITS) - 2); //priority higher than ble int
#endif
    
    Write_Iwdg_ON(IWDG_Prescaler_256, 1000); //fff-26.208s max

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
    
#ifdef USE_UART
    printf("\r\nMAC:%02x-%02x-%02x-%02x-%02x-%02x", ble_mac_addr[5],ble_mac_addr[4],ble_mac_addr[3],ble_mac_addr[2],ble_mac_addr[1],ble_mac_addr[0]);
#endif
    
    ble_run_interrupt_start(160*2); //320*0.625=200 ms
    
    while(1){
        IrqMcuGotoSleepAndWakeup();
        
    }
}
