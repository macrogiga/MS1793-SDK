
/* Includes ------------------------------------------------------------------*/

/** @addtogroup Template_Project
  * @{
  */
#include "HAL_conf.h"
#include "BSP.h"
#include "mg_api.h"



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

volatile unsigned int SysTick_Count = 0;

unsigned int GetSysTickCount(void) //porting api
{
    return SysTick_Count;
}


u16 Timer2S=0;
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    SysTick_Count ++;
    Timer2S++;
    ble_nMsRoutine();
}

void EXTI0_1_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line0); 
}

#ifdef MS1797_EVBOARD
void EXTI2_3_IRQHandler(void)
#else
void EXTI4_15_IRQHandler(void)
#endif
{
    EXTI_ClearITPendingBit(IRQ_EXTI);

    ble_run(0);
}
