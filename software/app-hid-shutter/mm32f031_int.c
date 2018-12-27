
/* Includes ------------------------------------------------------------------*/

/** @addtogroup Template_Project
  * @{
  */
 #include "HAL_conf.h"
 #include "mg_api.h"
 #include "BSP.h"


extern unsigned char SleepStatus;

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

    if(1 == SleepStatus){ //sleep
        SysClk8to48();
    }else if(2 == SleepStatus){ //stop
        RCC->CR|=RCC_CR_HSION;
        RCC->CR |= RCC_CR_PLLON;
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
        SysTick_Config(48000);
    }
    SleepStatus = 0;
    
    ble_run(0);
}
