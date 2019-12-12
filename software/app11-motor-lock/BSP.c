#include "BSP.h"
#include "mg_api.h"




unsigned char SleepStop = 0x00; //01-sleep, 02-stop

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
    //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
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

/********************************************************************************************************
**函数信息 ：UartInit(UART_TypeDef* UARTx)
**功能描述 ：初始化串口
**输入参数 ：UART_TypeDef* UARTx ，选择UART1、UART2、UART3
**输出参数 ：无
********************************************************************************************************/

void UartInit(UART_TypeDef* UARTx)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    UART_InitTypeDef UART_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);	//使能UART1，GPIOA时钟
    
    //UART1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = UART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    
    //UART 初始化设置
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);
    
    UART_InitStructure.UART_BaudRate = 115200;//串口波特率
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;//字长为8位数据格式
    UART_InitStructure.UART_StopBits = UART_StopBits_1;//一个停止位
    UART_InitStructure.UART_Parity = UART_Parity_No;//无奇偶校验位
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;//无硬件数据流控制
    UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;	//收发模式
    
    UART_Init(UART1, &UART_InitStructure); //初始化串口1
    UART_ITConfig(UART1, UART_IT_RXIEN, ENABLE);//开启串口接受中断
    UART_ITConfig(UART1, UART_IT_TXIEN, DISABLE);
    UART_Cmd(UART1, ENABLE);                    //使能串口1 
    
    //UART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
    
    //UART1_RX	  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  
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
    //	RCC->APB2RSTR|=0X01FC;//复位所有IO口
    PWR_WakeUpPinCmd(ENABLE);  //使能唤醒管脚功能
    PWR_EnterSTANDBYMode();	  //进入待命（STANDBY）模式 
    
}

void BSP_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    SystemClk_HSEInit();
    SysTick_Configuration();
    
    //SPIM_Init(SPI_BLE,0x08); //6Mhz
    SPIM_Init(SPI_BLE,0x06); //8Mhz
    //UartInit(UART1);
    
    //IRQ - pa12(MS1793)  PD2(MS1791)
    GPIO_InitStructure.GPIO_Pin  = IRQ_BLE_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入   
    GPIO_Init(IRQ_BLE_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;	 //PA.0
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPD;//下拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化IO
    
    //module led indicator PA8
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_5);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
//    //motor module PD2,PD3
//    GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_5);
//    GPIO_PinAFConfig(GPIOD,GPIO_PinSource3,GPIO_AF_5);
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2 | GPIO_Pin_3;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
//    
//    GPIO_ResetBits(GPIOD, GPIO_Pin_2);
//    GPIO_ResetBits(GPIOD, GPIO_Pin_3);
    

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
    
//	PWR->CR = PWR->CR & 0xfffd; //PDDS = 0;enter stop mode
//	SCB->SCR |= 0x4;
/*
    //key
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_3);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_3);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_3);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_3);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_5);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_7|GPIO_Pin_10|GPIO_Pin_11;	 //PA.3/4/7/10/11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//下拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化IO*/
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



#define TOUT_STDBY 300 //1000/200*60 = 300, 1 min
extern unsigned int StandbyTimeout;

void IrqMcuGotoSleepAndWakeup(void)
{
    if(ble_run_interrupt_McuCanSleep())
    {
        if (TOUT_STDBY < StandbyTimeout)
        {
            RCC_LSICmd(DISABLE);  //in STANDBY iwdg will cause reset
            
            radio_standby();
            Sys_Standby();
        }else{ //enter SLEEP/STOP to save power
    #if 1//STOP
            SleepStop = 2;
            SysClk48to8();
            SCB->SCR |= 0x4;
            __WFI();
    #endif
        }

    }
}

//////DO NOT REMOVE, used in ble lib///////
void McuGotoSleepAndWakeup(void) // auto goto sleep AND wakeup, porting api
{
}
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
    
    RCC->CR &=~(RCC_CR_PLLON);		//clear PLL
    SysTick_Config(8000);
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

