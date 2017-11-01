#include <string.h>
#include "HAL_conf.h"
#include "BSP.h"
#include "iwdg.h"
#include "mg_api.h"


#ifdef USE_I2C
    extern void IIC_Init(I2C_TypeDef* I2Cx);
#endif

extern u8 BKP_DATA(void);
extern volatile unsigned int SysTick_Count;

unsigned char *ble_mac_addr;
unsigned char* get_local_addr(void) //used for ble pairing case
{
    return ble_mac_addr;
}

const unsigned char AdvDat_HRS[]=
{
    0x02,0x01,0x06,
    0x03,0x19,0x41,0x03,
    0x07,0x03,0x0D,0x18,0x0A,0x18,0x0F,0x18
};

int main(void)
{
    BSP_Init();

#ifdef USE_I2C
    IIC_Init(I2C1);
#endif

    radio_initBle(TXPWR_0DBM, &ble_mac_addr);
    
    ble_set_adv_data((unsigned char *)AdvDat_HRS, sizeof(AdvDat_HRS));

    SysTick_Count = 0;
    if (BKP_DATA())
    {
        SysClk48to8();
        while(SysTick_Count <= 8){
            SCB->SCR &= 0xFB;
            __WFE();
        }
        SysClk8to48();
        
    }else{
        while(SysTick_Count <= 1500){};
    }

    Write_Iwdg_ON(IWDG_Prescaler_32, 0x4E2); //1250*0.8ms=1s
    
    ble_run(160*2); //320*0.625=200 ms
}

