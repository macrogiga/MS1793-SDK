#include "BSP.h"
#include "mg_api.h"





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
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
	
	if(SPIx==SPI1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  //SPI1 clk enable
		SPI_CS_Disable;
		
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

void SysTick_Configuration(void)
{
    SysTick_Config(48000);
}

/*
GPA9    R
GPA10   G
GPA11   B
GPA4   Y
GPA7   W
*/
void PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE); //AF2,GPA9,10,11, timer1@Ch2,3,4
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM14, ENABLE);  //AF4,GPA4   timer14@Ch1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);  //AF5,GPA7   timer17@Ch1
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA , ENABLE); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,GPIO_AF_2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10,GPIO_AF_2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11,GPIO_AF_2);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource4,GPIO_AF_4);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7,GPIO_AF_5);
	
	TIM_TimeBaseStructure.TIM_Period = 255*100; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 1K
	TIM_TimeBaseStructure.TIM_Prescaler = 0; //设置用来作为TIMx时钟频率除数的预分频值  480分频 100k
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    TIM_TimeBaseInit(TIM17, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位


	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC1Init(TIM17, &TIM_OCInitStructure);
    
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH2预装载使能
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH3预装载使能
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH4预装载使能
	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);  //CH1预装载使能
	TIM_OC1PreloadConfig(TIM17, TIM_OCPreload_Enable);  //CH1预装载使能
	
    TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器
    TIM_ARRPreloadConfig(TIM14, ENABLE); //使能TIMx在ARR上的预装载寄存器
    TIM_ARRPreloadConfig(TIM17, ENABLE); //使能TIMx在ARR上的预装载寄存器
    
    TIM_Cmd(TIM1, ENABLE);  //使能TIM1
    TIM_Cmd(TIM14, ENABLE);  //使能TIM14
    TIM_Cmd(TIM17, ENABLE);  //使能TIM17
    
    TIM1->BDTR |= 0x8000;
    TIM14->BDTR |= 0x8000;
    TIM17->BDTR |= 0x8000;
    
    SetLEDLum(0,0,0,100);//上电亮白光
    //SetLEDLum(0,0,0,0);//Yellow
    //SetLEDLum(r,g,b,101);//rgb
    SetLEDLum(0,0,100,101);
}


void LED_ONOFF(unsigned char onFlag)//module indicator,GPA8
{
    if(onFlag){
        GPIOA->BRR = GPIO_Pin_8;  //low, on
    }else{
        GPIOA->BSRR = GPIO_Pin_8; //high, off
    }
}

void BSP_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	SystemClk_HSEInit();
	PWM_Init();
	SysTick_Configuration();
    
	SPIM_Init(SPI1,0x06); //8Mhz
	
	//IRQ - pA12
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入   
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //module led indicator PA8
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_5);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
//    LED_ONOFF(1);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
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
    
	PWR->CR = PWR->CR & 0xfffd; //PDDS = 0;enter stop mode
	SCB->SCR |= 0x4;
}


/////////////////////Following functions are porting functions/////////////////////////////////

unsigned char SPI_WriteRead(unsigned char SendData,unsigned char WriteFlag) //porting api
{
	SPI1->TXREG = SendData;
	while(!(SPI1->CSTAT & SPI_FLAG_TXEPT));
	while(!(SPI1->CSTAT & SPI_FLAG_RXAVL));
	return SPI1->RXREG;
}

void SPI_CS_Enable_(void) //porting api
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_8);
}

void SPI_CS_Disable_(void) //porting api
{
    GPIO_SetBits(GPIOB, GPIO_Pin_8);
}

char IsIrqEnabled(void) //porting api
{
    return (!(GPIO_ReadInputData(GPIOA) & 0x1000)); //PA12
}

void McuGotoSleepAndWakeup(void) // auto goto sleep AND wakeup, porting api
{
#if 0
    SysClk48to8();
    SCB->SCR &= 0xFB;//sleep
    __WFE();        
    SysClk8to48();
//#else
    SysClk48to8();
    SCB->SCR |= 0x4;
    __WFI();

    RCC->CR|=RCC_CR_HSION;
    RCC->CR |= RCC_CR_PLLON;
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
    SysTick_Config(48000);
#endif
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
    
    RCC->CR &=~(RCC_CR_PLLON);		//clear PLL
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
extern void Led_getInfo(unsigned char* data);
void UpdateLEDValueAll(void) //porting function
{
    int t;
    unsigned char data[7];
    unsigned char EnableLED_Flag = 0;
//    unsigned char Led_R;
//    unsigned char Led_G;
//    unsigned char Led_B;
//    unsigned int Led_Y;
//    unsigned int Led_W;
    unsigned int Led_Lum_percent = 100;
        
    Led_getInfo(data);
    EnableLED_Flag = data[0];
    if(EnableLED_Flag == 0)
    {
        TIM_SetCompare2(TIM1,0); //G
        TIM_SetCompare3(TIM1,0); //B
        TIM_SetCompare4(TIM1,0); //R
        TIM_SetCompare1(TIM14,0); //Y
        TIM_SetCompare1(TIM17,0); //W
    }
    else
    {
        Led_Lum_percent = data[6];
		t = data[4] * Led_Lum_percent; TIM_SetCompare2(TIM1,t); //Rx100
		t = data[5] * Led_Lum_percent; TIM_SetCompare3(TIM1,t); //Gx100
        t = data[2] * Led_Lum_percent; TIM_SetCompare4(TIM1,t); //Bx100
        t = data[1] * Led_Lum_percent; TIM_SetCompare1(TIM14,t); //Yx100
        t = data[3] * Led_Lum_percent; TIM_SetCompare1(TIM17,t); //Wx100
    }
    
}
