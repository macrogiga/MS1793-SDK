/**
******************************************************************************
* @file    wwdg_test.c  
* @brief   配置定时器3,实现两路可调10KHz PWM功能
*          PA6,PA7可观察输出波形
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "pwm.h" 
#include "mg_api.h"



#define     LED_TOTAL       5 //number of led
#define     LED_PLUS        (LED_TOTAL*24)
#define     PWM_CHANNUM     (LED_PLUS+1)

#define DUTY_OFF 15  //T0H
#define DUTY_ON  65  //T1H
#define DUTY_RST  0  //


static u8 PWM_Data[PWM_CHANNUM]={
15,15,15,15,15,15,15,15,15, 15,15,15,15,15,15,15,15,15, 15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15, 15,15,15,15,15,15,15,15,15, 15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15, 15,15,15,15,15,15,15,15,15, 15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15, 15,15,15,15,15,15,15,15,15, 15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15, 15,15,15,15,15,15,15,15,15, 15,15,15,15,15,15,
DUTY_RST
}; 


//static u8  plus_state =0;  //0:stop  1:plus
static u16 Plus_cnt= 0;

void TIM_DMA_Config()
{
    DMA_InitTypeDef DMA_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(TIM1->CCR1);
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&PWM_Data;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = PWM_CHANNUM;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel2, ENABLE);
    DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);
}

void TIM_NVIC(IRQn_Type num,u8 Priority)
{
    NVIC_InitTypeDef  NVIC_StructInit;
    NVIC_StructInit.NVIC_IRQChannel=num;
    NVIC_StructInit.NVIC_IRQChannelCmd=ENABLE;

    NVIC_StructInit.NVIC_IRQChannelPreemptionPriority = Priority;

    NVIC_Init(&NVIC_StructInit);
}

void PWM_Setup(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    
    TIM_DMA_Config();
    TIM_NVIC(TIM1_BRK_UP_TRG_COM_IRQn,1);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);  //?a??GPIOA

    //plus_state = 1;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM1_CHA
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //?′ó?í?íìê?3?
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_2);

    TIM_TimeBaseStructure.TIM_Period = (48000000/600000 - 1);   //éè???ú??ò????üD?ê??t×°è????ˉμ?×??ˉ??×°????′??÷?ü?úμ??μ	 80K
    TIM_TimeBaseStructure.TIM_Prescaler =0; //éè??ó?à′×÷?aTIMxê±?ó?μ?ê3yêyμ??¤·??μ?μ  2?·??μ
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //éè??ê±?ó·???:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM?òé???êy?￡ê?
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //?ù?YTIM_TimeBaseInitStruct?D???¨μ?2?êy3?ê??ˉTIMxμ?ê±???ùêyμ￥??
    
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //?????¨ê±?÷?￡ê?:TIM??3??í?èμ÷???￡ê?2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //±è??ê?3?ê1?ü
    TIM_OCInitStructure.TIM_Pulse = 30; //éè??′y×°è?2???±è????′??÷μ???3??μ
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //ê?3???D?:TIMê?3?±è????D???
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //?ù?YTIM_OCInitStruct?D???¨μ?2?êy3?ê??ˉíaéèTIMx
    
    
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1?¤×°??ê1?ü 

    TIM_ClearFlag(TIM1, TIM_FLAG_Update); //更新定时器时会产生更新时间,清除标志位
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE); //	允许定时器16更新中断
    TIM_ARRPreloadConfig(TIM1, ENABLE); //ê1?üTIMx?úARRé?μ??¤×°????′??÷
    TIM_DMACmd(TIM1,TIM_DMA_CC1,ENABLE);
    TIM_Cmd(TIM1, ENABLE);  //ê1?üTIM1
    TIM_CtrlPWMOutputs(TIM1, ENABLE);//

    Plus_cnt = 0;
}


void TIM1_BRK_UP_TRG_COM_IRQHandler()
{
    if ((TIM1->SR & TIM_IT_Update))
    {
        TIM1->SR &= (uint16_t)~TIM_IT_Update;

        //GPIOB->BSRR = GPIO_Pin_3;
        Plus_cnt++;
        if(Plus_cnt == LED_PLUS)
        {
            Plus_cnt = 0;
            TIM1->BDTR &= (uint16_t)(~((uint16_t)TIM_BDTR_MOE));
            TIM1->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN)) ; 
            //plus_state  = 0;
            NVIC_EnableIRQ(SysTick_IRQn);
        }
        //GPIOB->BRR = GPIO_Pin_3;
    }
}
extern u8 flag_newcmd;
static void UpdatePWMData(u8 color_g, u8 color_r, u8 color_b)
{
    static u8 i = 0;
    u8 j = 0;

    if(flag_newcmd == 0) return;
    flag_newcmd = 0;

    {
        for (j=0; j<8; j++){
            if (color_g & (1<<(7-j))){
                PWM_Data[i*24+j] = DUTY_ON;
            }else{
                PWM_Data[i*24+j] = DUTY_OFF;
            }
        }
        
        for (j=8; j<16; j++){
            if (color_r & (1<<(15-j))){
                PWM_Data[i*24+j] = DUTY_ON;
            }else{
                PWM_Data[i*24+j] = DUTY_OFF;
            }
        }
        
        for (j=16; j<24; j++){
            if (color_b & (1<<(23-j))){
                PWM_Data[i*24+j] = DUTY_ON;
            }else{
                PWM_Data[i*24+j] = DUTY_OFF;
            }
        }
    }

    if(++i >= LED_TOTAL) i = 0;
}

void UpdateColor(void)
{
    unsigned long t = 0;
    unsigned char data[7];
    unsigned char EnableLED_Flag = 0;
    unsigned char Led_Lum_percent = 100;
    
    unsigned char r,g,b;
    
    Led_getInfo(data);
    EnableLED_Flag = data[0];
    if(EnableLED_Flag == 0)
    {
        UpdatePWMData(0,0,0);
    }
    else
    {
        Led_Lum_percent = data[6];
        
        t = data[1] * Led_Lum_percent/100; 
        r = t;
        t = data[2] * Led_Lum_percent/100; 
        g = t;
        t = data[3] * Led_Lum_percent/100; 
        b = t;

        UpdatePWMData(g,r,b);
    }
    
    NVIC_DisableIRQ(SysTick_IRQn);
    TIM_DMA_Config();
    TIM1->BDTR |= (uint16_t)(((uint16_t)TIM_BDTR_MOE));
    TIM1->CR1 |= (uint16_t)(((uint16_t)TIM_CR1_CEN)) ;
}
