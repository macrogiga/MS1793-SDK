#include "adc.h"



/********************************************************************************************************
**函数信息 ：void ADC1_SingleChannel(uint8_t ADC_Channel_x)      
**功能描述 ：配置ADC1单次转换模式
**输入参数 ：ADC_Channel_x , x为0~8
**输出参数 ：无
**    备注 ：
********************************************************************************************************/
void ADC1_SingleChannel(uint8_t ADC_Channel_x)
{
    ADC_InitTypeDef  ADC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    
    /* Initialize the ADC_PRESCARE values */
    ADC_InitStructure.ADC_PRESCARE = ADC_PCLK2_PRESCARE_16;
    /* Initialize the ADC_Mode member */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Single;
    /* Initialize the ADC_ContinuousConvMode member */
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    /* Initialize the ADC_DataAlign member */
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    /* Initialize the ADC_ExternalTrigConv member */
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;//ADC1 通道1，即PA1
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    /*屏蔽所有通道*/
    ADC_RegularChannelConfig(ADC1, DISABLE_ALL_CHANNEL , 0, 0); 
    /*使能选中通道,后面参数保留*/
    ADC_RegularChannelConfig(ADC1, ADC_Channel_x, 0, ADC_SampleTime_13_5Cycles); 
    
    ADC_Cmd(ADC1, ENABLE); 
}

/********************************************************************************************************
**函数信息 ：ADC1_SingleChannel_Get()       
**功能描述 ：获取ADC1转换数据
**输入参数 ：ADC_Channel_x , x为0~8
*puiADData ,ADC1实际转换数据
**输出参数 ：ucStatus ,0 表示数据获取失败,1 表示成功
********************************************************************************************************/
u16 ADC1_SingleChannel_Get(uint8_t ADC_Channel_x)
{	
    u16 puiADData;
    
	/*ADCR寄存器的ADST位使能，软件启动转换*/
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); 
    while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==0);
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	puiADData=ADC1->ADDATA&0xfff;
	return puiADData;
}

/********************************************************************************************************
**函数信息 ：Get_Adc_Average(uint8_t ADC_Channel_x,uint8_t times)     
**功能描述 ：获取几次ADC1采样值的平均值
**输入参数 ：ADC_Channel_x , x为0~8
**输出参数 ：puiADData为ADC读到的值
********************************************************************************************************/
u16 Get_Adc_Average(uint8_t ADC_Channel_x,uint8_t times)
{
	u32 temp_val=0;
	u8 t;
	u8 delay;
    
	for(t=0;t<times;t++)
	{
		temp_val+=ADC1_SingleChannel_Get(ADC_Channel_x);
		for(delay=0;delay<100;delay++);
	}
	return temp_val/times;
} 	
