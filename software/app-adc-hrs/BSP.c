#include "BSP.h"
#include "adc.h"
#include "HAL_bkp.h"
#include "mg_api.h"


/********************************************************************************************************
**函数信息 ：BKP_DATA(void)         
**功能描述 : BKP数据读写测试，判断写和读的数据是否一致
**输入参数 ：FirstBackupData
**输出参数 ：i
********************************************************************************************************/
u8 BKP_DATA(void)
{
    u16 i;

    /* Enable PWR and BKP clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* Enable write access to Backup domain */
    PWR_BackupAccessCmd(ENABLE);

    /* Clear Tamper pin Event(TE) pending flag */
    BKP_ClearFlag();

    i = BKP_ReadBackupRegister(BKP_DR1);
    if (i != 0x1234)
    {
        BKP_WriteBackupRegister(BKP_DR1, 0x1234);
        return 0;
    }
    else
    {
        return 1;
    }
}


/********************************************************************************************************
**函数信息 ：SPIM_TXEn(SPI_TypeDef* SPIx)                     
**功能描述 :关闭 SPI 在双向模式下的数据传输方向 
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_TXEn(SPI_TypeDef* SPIx)
{
    //Transmit Enable bit TXEN
    SPI_BiDirectionalLineConfig(SPIx, SPI_Direction_Tx);
}

/********************************************************************************************************
**函数信息 ：SPIM_TXDisable(SPI_TypeDef* SPIx)                     
**功能描述 :关闭 SPI 在双向模式下的数据传输方向 
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_TXDisable(SPI_TypeDef* SPIx)
{
    //disable TXEN
    SPI_BiDirectionalLineConfig(SPIx, SPI_Disable_Tx);
}

/********************************************************************************************************
**函数信息 ：SPIM_RXEn(SPI_TypeDef* SPIx)                     
**功能描述 :关闭 SPI 在双向模式下的数据传输方向 
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_RXEn(SPI_TypeDef* SPIx)
{
    //enable RXEN
    SPI_BiDirectionalLineConfig(SPIx, SPI_Direction_Rx);
}

/********************************************************************************************************
**函数信息 ：SPIM_RXDisable(SPI_TypeDef* SPIx)                     
**功能描述 :关闭 SPI 在双向模式下的数据传输方向 
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_RXDisable(SPI_TypeDef* SPIx)
{
    //disable RXEN
    SPI_BiDirectionalLineConfig(SPIx, SPI_Disable_Rx);
}

/********************************************************************************************************
**函数信息 ：SPIM_Init(SPI_TypeDef* SPIx, unsigned short spi_baud_div)            
**功能描述 :可修改参数初始化SPI
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2  ;spi_baud_div
**输出参数 ：无
********************************************************************************************************/
void SPIM_Init(SPI_TypeDef* SPIx,unsigned short spi_baud_div)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    if(SPIx==SPI1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  //SPI1 clk enable
        //SPI_CS_Disable;

        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_0);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_0);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_0);

        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;   //spi1_cs  pb8
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        
        GPIO_SetBits(GPIOB, GPIO_Pin_8); //csn=H
        

        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;   //spi1_sck  pb3
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 推免复用输出
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;   //spi1_mosi  pb5
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 推免复用输出
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;  //spi1_miso  pb4
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //上拉输入   
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
    else if(SPIx==SPI2)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);  //SPI2 clk enable
        //SPI_CSInternalSelected(SPI2, SPI_CS_BIT0, DISABLE);

        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOD, ENABLE);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_3);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_4);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_4);
        
        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;   //spi2_cs  pb15
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        
        GPIO_SetBits(GPIOB, GPIO_Pin_15); //csn=H
        
        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;   //spi2_sck  pb14
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 推免复用输出
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;   //spi2_mosi  pb13
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 推免复用输出
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;  //spi2_miso  pb12
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //上拉输入
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }


    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_DataWidth = SPI_DataWidth_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;    // mode0 SPI_CPOL_Low, SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;  // mode3 SPI_CPOL_High,SPI_CPHA_2Edge
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = spi_baud_div;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPIx, &SPI_InitStructure);
    
    SPI_Cmd(SPIx, ENABLE); //Enables the specified SPI peripheral SPI使能、主机模式 8位数据模式 SPI 的波特率

    SPIM_TXEn(SPIx);
    SPIM_RXEn(SPIx);
}

/********************************************************************************************************
**函数信息 ：SystemClk_HSEInit (void)                        
**功能描述 ：系统时钟初始化函数，初始化之前先复位所有时钟
**输入参数 ：无
**输出参数 ：无
********************************************************************************************************/
void SetSysClock_HSI(u8 PLL)
{
  unsigned char temp=0;   
  
  RCC->CR|=RCC_CR_HSION;  
  while(!(RCC->CR&RCC_CR_HSIRDY));
  RCC->CFGR=RCC_CFGR_PPRE1_2; //APB1=DIV2;APB2=DIV1;AHB=DIV1;
  
  RCC->CFGR&=~RCC_CFGR_PLLSRC;	  //PLLSRC ON 
  
  RCC->CR &=~(RCC_CR_PLLON);		//clear PLL//	RCC->CR &=~(7<<20);		//clear PLL
  
  RCC->CR &=~(0x1f<<26);	
  RCC->CR|=(PLL - 1) << 26;   //setting PLL value 2~16
  
  FLASH->ACR=FLASH_ACR_LATENCY_1|FLASH_ACR_PRFTBE;	  //FLASH 2 delay clk cycles
  
  RCC->CR|=RCC_CR_PLLON;  //PLLON
  while(!(RCC->CR&RCC_CR_PLLRDY));//waiting for PLL locked
  RCC->CFGR&=~RCC_CFGR_SW;
  RCC->CFGR|=RCC_CFGR_SW_PLL;//PLL to be the sys clock
  while(temp!=0x02)     //waiting PLL become the sys clock
  {    
    temp=RCC->CFGR>>2;
    temp&=0x03;
  } 
}

void SystemClk_HSEInit(void)
{
    SetSysClock_HSI(4);//HSI:12*4=48M

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);//selecting PLL clock as sys clock
    while (RCC_GetSYSCLKSource() != 0x08)
    {}
}

void SysTick_Configuration(void)
{
    SysTick_Config(48000);
}

void LED_ONOFF(unsigned char onFlag)//module indicator,GPA8
{
    if(onFlag){
        GPIOA->BRR = GPIO_Pin_8;  //low, on
    }else{
        GPIOA->BSRR = GPIO_Pin_8; //high, off
    }
}

void Sys_Standby(void)
{  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	//使能PWR外设时钟
    //RCC->APB2RSTR|=0X01FC;//复位所有IO口
    PWR_WakeUpPinCmd(ENABLE);  //使能唤醒管脚功能
   PWR_EnterSTANDBYMode();	  //进入待命（STANDBY）模式 

}

#ifdef USE_I2C
void IIC_Init(I2C_TypeDef* I2Cx)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
#ifdef USE_I2CIRQ
    NVIC_InitTypeDef NVIC_InitStructure;

    //I2C NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = I2C1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
#endif

    if(I2Cx==I2C1)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);  //i2c1 clk enable
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9 | GPIO_Pin_10;   //i2c1_scl  pa9  i2c1_sda  pa10
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // 复用开漏输出
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_4);
        GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_4);
    }

    I2Cx->IC_ENABLE = 0x0;
    I2Cx->IC_CON = 0x163;
    I2Cx->IC_TAR = 0xAA>>1;//Address
    //I2Cx->IC_SAR = 0xAA>>1;//Address
    I2Cx->IC_SS_SCL_HCNT = 0x77;
    I2Cx->IC_SS_SCL_LCNT = 0x55;
    I2Cx->IC_RX_TL=0x0;//set Rx FIFO threshold level 
    I2Cx->IC_TX_TL=0x0;//set Tx FIFO threschold level 
    I2Cx->IC_INTR_MASK=0X00;//disable all inttrupt
    I2Cx->IC_ENABLE=0x1;
}

static u8 *DataPt;
static u8 DataLen=0, DataCnt=0;
void I2C_Send(u8 *pdat, u8 len)
{
    DataPt = pdat;
    DataLen = len;

    DataCnt = 0;
#ifdef USE_I2CIRQ
    I2C1->IC_DATA_CMD = DataPt[DataCnt++];
    I2C1->IC_INTR_MASK = I2C_IT_TX_EMPTY;
#else
    do
    {
        if (DataLen - DataCnt == 1)
        {
            I2C1->IC_DATA_CMD = DataPt[DataCnt++] | 0x200;
        }
        else
        {
            I2C1->IC_DATA_CMD = DataPt[DataCnt++];
        }
        
        while(!(I2C1->IC_STATUS & I2C_STATUS_FLAG_TFE));
    }
    while(DataCnt < DataLen);
    
#endif
}

#ifdef USE_I2CIRQ
void I2C1_IRQHandler(void)
{
    if(I2C1->IC_INTR_STAT & I2C_IT_TX_EMPTY)
    {
        I2C_ClearFlag(I2C1,I2C_IT_TX_EMPTY);
        if (DataCnt < DataLen)
        {
            if (DataLen - DataCnt == 1)
            {
               I2C1->IC_DATA_CMD = DataPt[DataCnt++] | 0x200;
            }
            else
            {
                I2C1->IC_DATA_CMD = DataPt[DataCnt++];
            }
        }
        else//over
        {
            I2C1->IC_INTR_MASK=0x0000;//disable all inttrupt
        }
    }
}
#endif
#endif

void BSP_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    SystemClk_HSEInit();
    SysTick_Configuration();
    
    //SPIM_Init(SPI_BLE,0x08); //6Mhz
    SPIM_Init(SPI_BLE,0x06); //8Mhz
    

    //IRQ - pa12(MS1793)  PD2(MS1797)
    GPIO_InitStructure.GPIO_Pin  = IRQ_BLE_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
    GPIO_Init(IRQ_BLE_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0; //PA.0
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPD;//下拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化IO
    
    //module led indicator PA8
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_5);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
#if defined MS1797_EVBOARD
    GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    /*将PA4配置为模拟输入*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    ADC1_SingleChannel(ADC_Channel_4);
#else
    GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    /*将PA3配置为模拟输入*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    ADC1_SingleChannel(ADC_Channel_3);
#endif
    

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

#ifdef MS1797_EVBOARD
    SYSCFG_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource2);
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_SetPriority (EXTI2_3_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
#else
    SYSCFG_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource12);
    EXTI_InitStructure.EXTI_Line = EXTI_Line12;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_SetPriority (EXTI4_15_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
#endif
}




/////////////////////Following functions are porting functions/////////////////////////////////

unsigned char SPI_WriteRead(unsigned char SendData,unsigned char WriteFlag) //porting api
{
    SPI_BLE->TXREG = SendData;
    while(!(SPI_BLE->CSTAT & SPI_FLAG_TXEPT));
    while(!(SPI_BLE->CSTAT & SPI_FLAG_RXAVL));
    return SPI_BLE->RXREG;
}

void SPI_CS_Enable_(void) //porting api
{
    GPIO_ResetBits(CSN_BLE_PORT, CSN_BLE_PIN);
}

void SPI_CS_Disable_(void) //porting api
{
    GPIO_SetBits(CSN_BLE_PORT, CSN_BLE_PIN);
}

char IsIrqEnabled(void) //porting api
{
    return (!(GPIO_ReadInputData(IRQ_BLE_PORT) & IRQ_BLE_PIN));
}


extern char GetConnectedStatus(void);
void McuGotoSleepAndWakeup(void) // auto goto sleep AND wakeup, porting api
{
    if (0 == GetConnectedStatus())
    {
        radio_standby();
        Sys_Standby();
    }
    else
    { //enter SLEEP/STOP to save power
#ifdef USE_I2C
        if (0 == (I2C1->IC_STATUS & 0x20))
#endif
        {
#if 0 //SLEEP
        SysClk48to8();
        SCB->SCR &= 0xFB;
        __WFE();
        
        SysClk8to48();
#else //STOP
        SysClk48to8();
        SCB->SCR |= 0x4;
        __WFI();
        
        RCC->CR|=RCC_CR_HSION;
        RCC->CR |= RCC_CR_PLLON;
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
        SysTick_Config(48000);
#endif
        }
    }
}

//////DO NOT REMOVE, used in ble lib///////
void SysClk8to48(void)
{
    SystemClk_HSEInit();
    SysTick_Config(48000);
}
void SysClk48to8(void)
{
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);//selecting PLL clock as sys clock
    
    while (RCC_GetSYSCLKSource() != 0x0)
    {}
    
    RCC->CR &=~(RCC_CR_PLLON); //clear PLL
    SysTick_Config(8000);
}

static char dis_int_count = 0;
void DisableEnvINT(void)
{ 
    //to disable int
    __ASM volatile("cpsid i");
    
    dis_int_count ++;
}

void EnableEnvINT(void)
{
    //to enable/recover int
    dis_int_count --;    
    if(dis_int_count<=0) //protection purpose
    {
        dis_int_count = 0; //reset
        __ASM volatile("cpsie i");
    }
}

//api provide in blelib
//    EnableLED_Flag; Led_R; Led_G; Led_B; Led_Y; Led_W; Led_Lum_percent; 
void UpdateLEDValueAll(void) //porting function
{
    
}

