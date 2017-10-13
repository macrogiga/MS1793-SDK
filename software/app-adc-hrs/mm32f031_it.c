
/* Includes ------------------------------------------------------------------*/
// #include "stm32f0xx_it.h"
// #include "stm32f0xx_exti.h"
// #include "stm32f0xx_tim.h"
// #include "stm32f0xx_gpio.h"

/** @addtogroup Template_Project
  * @{
  */
#include "HAL_conf.h"

//unsigned short capture = 0;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/*static */unsigned int SysTick_Count = 0;

unsigned int GetSysTickCount(void) //porting api
{
    return SysTick_Count;
}


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    SysTick_Count ++;
}

void EXTI0_1_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line0); 
}
void EXTI4_15_IRQHandler(void)
{
	/*EXTI_ClearITPendingBit(EXTI_Line10);*/
	EXTI_ClearITPendingBit(EXTI_Line11);
	EXTI_ClearITPendingBit(EXTI_Line12); 
}
