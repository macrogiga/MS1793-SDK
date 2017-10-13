#include "HAL_rcc.h"
#include "HAL_iwdg.h"
#include "iwdg.h" 

/********************************************************************************************************
**函数信息 ：PVU_CheckStatus(void)                       
**功能描述 ：检查独立看门狗预分频位状态
**输入参数 ：无
**输出参数 ：无
********************************************************************************************************/
void PVU_CheckStatus(void)
{
	while(1)
	{
		/*检查预分频位状态,为RESET才可改变预分频值*/
		if(IWDG_GetFlagStatus(IWDG_FLAG_PVU)==RESET)                                                       
		{
			break;
		}
	}
}
/********************************************************************************************************
**函数信息 ：RVU_CheckStatus(void)                  
**功能描述 ：检查独立看门狗重载标志
**输入参数 ：无
**输出参数 ：无
********************************************************************************************************/
void RVU_CheckStatus(void)
{
	while(1)
	{
		/*检查重载标志状态*/
		if(IWDG_GetFlagStatus(IWDG_FLAG_RVU)==RESET)  
		{
			break;
		}
	}
}
/********************************************************************************************************
**函数信息 ：Write_Iwdg_PR(void)             
**功能描述 ：启动独立看门狗
**输入参数 ：IWDG_Prescaler 可选IWDG_Prescaler_X, X为4,8,16,32,64,128,256,对应分频值与X取值相同
Reload<=0xfff,为计数器重载值
**输出参数 ：无
**    备注 ：复位时常计算已LSI 40KHz为参考
Tiwdg=(X/LSI)*Reload
********************************************************************************************************/
void Write_Iwdg_ON(unsigned short int IWDG_Prescaler,unsigned short int Reload)
{
	/*启动内部低速时钟,等待时钟就绪*/
	RCC_LSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET);
	
	/*设置时钟预分频*/	
	PVU_CheckStatus();
	IWDG_WriteAccessCmd(0x5555);
	IWDG_SetPrescaler(IWDG_Prescaler);
	
	/*设置重载寄存器值*/	
	RVU_CheckStatus();
	IWDG_WriteAccessCmd(0x5555);
	IWDG_SetReload(Reload&0xfff);	
	
	/*装载并使能计数器*/	
	IWDG_ReloadCounter();
	IWDG_Enable();
}

/********************************************************************************************************
**函数信息 ：void Write_Iwdg_RL(void)           
**功能描述 ：喂狗函数
**输入参数 ：
**输出参数 ：无
********************************************************************************************************/
void Write_Iwdg_RL(void)
{
	IWDG_ReloadCounter();
}

