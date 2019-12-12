#include "BSP.h"
#include "oled.h"
#include "mg_api.h"



#if defined(MS1793_EVBOARD)
  #define DHT11_PIN GPIO_Pin_7
  #define DHT11_PIN_SOURCE GPIO_PinSource7
  #define DHT11_PORT GPIOA
#elif defined(MS1791_EVBOARD)
  #define DHT11_PIN GPIO_Pin_5
  #define DHT11_PIN_SOURCE GPIO_PinSource5
  #define DHT11_PORT GPIOB
#else //default
  #define DHT11_PIN GPIO_Pin_7
  #define DHT11_PIN_SOURCE GPIO_PinSource7
  #define DHT11_PORT GPIOA
#endif

void ReadDHT11(void);

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

void IIC_Init(I2C_TypeDef* I2Cx)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

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
    I2Cx->IC_TAR = 0x78>>1;//Address
    //I2Cx->IC_SAR = 0xAA>>1;//Address
    I2Cx->IC_SS_SCL_HCNT = 0x18/*0x77*/;
    I2Cx->IC_SS_SCL_LCNT = 0x18/*0x55*/;
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
}


void BSP_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    SystemClk_HSEInit();
    SysTick_Configuration();
    
    //SPIM_Init(SPI1,0x08); //6Mhz
    SPIM_Init(SPI_BLE,0x06); //8Mhz
    

    //IRQ - pa12(MS1793)  PD2(MS1791)
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
    
    //DHT11 PA7
    GPIO_PinAFConfig(DHT11_PORT,DHT11_PIN_SOURCE,GPIO_AF_3);
    GPIO_InitStructure.GPIO_Pin  = DHT11_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_7);
    
//    GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_3;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    /*将PA3配置为模拟输入*/
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//    
//    ADC1_SingleChannel(ADC_Channel_3);
    

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    SYSCFG_EXTILineConfig(IRQ_EXTIPORT, IRQ_EXTISOURCE);
    EXTI_InitStructure.EXTI_Line = IRQ_EXTI;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = IRQ_EXTNUMBER;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_SetPriority (IRQ_EXTNUMBER, (1<<__NVIC_PRIO_BITS) - 1);
    
    IIC_Init(I2C1);
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


void McuGotoSleepAndWakeup(void) // auto goto sleep AND wakeup, porting api
{
}

static u8 dht_step = 0;
static u16 HumiVal = 0xffff, TempVal = 0xffff;
static u8 flag_capturenotify = 0;

u8 CaptureDataReady(void)
{
    return flag_capturenotify;
}
u8 CaptureDataGet(u16* humi, u16* temp)
{
    if(flag_capturenotify){
        flag_capturenotify = 0;
        *humi = HumiVal;
        *temp = TempVal;
        return 1;
    }
    return 0;
}
u8 CaptureDataMon(u16* humi, u16* temp)
{
    if(flag_capturenotify){
        *humi = HumiVal;
        *temp = TempVal;
        return 1;
    }
    return 0;
}

void ReadDHT(void)
{
    static u32 timestart = 0;
    
    if(dht_step == 0){
        dht_step = 1;
        timestart = GetSysTickCount();
        CaptureReset();
    }else if(dht_step == 1){
        if((GetSysTickCount() - timestart) > 19){//at least 18ms
            dht_step = 2;
            timestart = GetSysTickCount();
            CaptureStart();
        }
    }else if(dht_step == 2){ //check
        if((GetSysTickCount() - timestart) > 6){ 
            CaptureStop();
        }
    }
}

void IrqMcuGotoSleepAndWakeup(void)
{
    if(ble_run_interrupt_McuCanSleep())
    {

        if (CaptureDataReady() && (0 == (I2C1->IC_STATUS & 0x20)))
        {
#if 1 //SLEEP
            SCB->SCR &= 0xFB;
            __WFE();
#endif
        }

    }
}

//////DO NOT REMOVE, used in ble lib///////
void SysClk8to48(void)
{
}
void SysClk48to8(void)
{
}

static char dis_int_count = 0;
void DisableEnvINT(void)
{ 
//    //to disable int
//    __ASM volatile("cpsid i");
//    
//    dis_int_count ++;
}

void EnableEnvINT(void)
{
//    //to enable/recover int
//    dis_int_count --;
//    if(dis_int_count<=0) //protection purpose
//    {
//        dis_int_count = 0; //reset
//        __ASM volatile("cpsie i");
//    }
}

void UpdateLEDValueAll(void) //porting function
{
}

void BeepInit(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);     //Open TIM3 clock
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);  //开启GPIOA时钟
    
    TIM_TimeBaseStructure.TIM_Period = 1000; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值  
    TIM_TimeBaseStructure.TIM_Prescaler =48-1; //设置用来作为TIMx时钟频率除数的预分频值  
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH2预装载使能
    
    TIM_ARRPreloadConfig(TIM3, ENABLE); //使能TIMx在ARR上的预装载寄存器
    
    TIM_Cmd(TIM3, DISABLE);  //禁止TIM3
}

void BeepOn(u16 Freq)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_1);
    
    TIM_SetCounter(TIM3,1000000/Freq);
    TIM_SetCompare2(TIM3,500000/Freq);
    TIM_Cmd(TIM3, ENABLE);  //使能TIM3
}

void BeepOff(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    TIM_Cmd(TIM3, DISABLE);  //禁止TIM3
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_ResetBits(GPIOA, GPIO_Pin_7);
}


static u8 Index=0;

void CaptureReset(void)
{
    GPIO_ResetBits(DHT11_PORT, DHT11_PIN);
}

void CaptureStart(void)
{
    unsigned char delay = 0x40;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_SetBits(DHT11_PORT, DHT11_PIN);
    
    while(delay--);
    
    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(DHT11_PORT,DHT11_PIN_SOURCE,GPIO_AF_1);
    
    TIM_ITConfig(TIM3, TIM_IT_CC2/*|TIM_IT_Update*/, ENABLE);  //中断配置
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2/*|TIM_IT_Update*/); //清除中断标志位
    TIM_Cmd(TIM3, ENABLE);
}

void CaptureStop(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);
    TIM_Cmd(TIM3, DISABLE);
    
    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
    
    GPIO_SetBits(GPIOA, DHT11_PIN);
    Index = 0;
    dht_step = 0;
}


void CaptureInit(void)   //用T3的CH1 APB1  PA6    AF1
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;
    TIM_ICInitTypeDef  TIM3_ICInitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);     //Open TIM3 clock
     
    TIM_TimeBaseStructure.TIM_Period = 0xffff; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值  
    TIM_TimeBaseStructure.TIM_Prescaler =48*4-1; //设置用来作为TIMx时钟频率除数的预分频值  
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    
    
    /*配置中断优先级*/
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM3_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
    TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; 
    TIM3_ICInitStructure.TIM_ICFilter = 0x0; 
    
    TIM_PWMIConfig(TIM3, &TIM3_ICInitStructure);     //PWM输入配置
    TIM_SelectInputTrigger(TIM3, TIM_TS_TI2FP2);     //选择有效输入端
    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);  //配置为主从复位模式
    
    CaptureStop();
}

void TIM3_IRQHandler(void)
{
    u16 TempPW = 0;
    static u8  Data[5];
    
    if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)//捕获1发生捕获事件
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
        TempPW = TIM_GetCapture2(TIM3);        //采集占空比

        Index++;
        if (Index > 1)
        {
            if (TempPW >= 25)//30
            {
                Data[(Index-2)/8] |= (1<<(7-(Index-2)%8));
            }
            else if (TempPW < 25)//20
            {
                Data[(Index-2)/8] &= ~(1<<(7-(Index-2)%8));
            }
        }
        if (Index > 40)
        {
            if (Data[4] == (Data[0]+Data[1]+Data[2]+Data[3]))
            {
                HumiVal = Data[0]*10 + Data[1];
                TempVal = Data[2]*10 + Data[3];
            }
            else
            {
                HumiVal = 999;
                TempVal = 999;
            }
            flag_capturenotify = 1;
            CaptureStop();
        }
    }
}
