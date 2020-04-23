/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Bleeper board GPIO driver implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
/**
  ******************************************************************************
  * @file    mlm32l0xx_it.c
  * @author  MCD Application Team
  * @brief   manages interupt
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "mlm32l0xx_it.h"
#include "vcom.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

void RTC_IRQHandler(void)
{
  HW_RTC_IrqHandler();
}

void EXTI0_1_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI2_3_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}


void EXTI4_15_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

void USARTx_IRQHandler(void)
{
  vcom_IRQHandler();
}

void USARTx_DMA_TX_IRQHandler(void)
{
  vcom_DMA_TX_IRQHandler();

}

/* Private functions ---------------------------------------------------------*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
