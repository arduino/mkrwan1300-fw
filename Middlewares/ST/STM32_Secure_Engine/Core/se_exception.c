/**
  ******************************************************************************
  * @file    se_exception.c
  * @author  MCD Application Team
  * @brief   Secure Engine exception module.
  *          This file provides set of firmware functions to manage SE
  *          non maskeable interruption
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
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
#include "stddef.h"
#include "se_low_level.h"
#include "se_exception.h"

/** @addtogroup SE Secure Engine
  * @{
  */

/** @addtogroup SE_CORE SE Core
  * @{
  */

/** @defgroup  SE_EXCEPTIONS SE Exception
  * @{
  */

/** @defgroup SE_EXCEPTIONS_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief SE Non Maskeable Interrupt handler
  * @param None.
  * @retval None.
  */

void SE_NMI_ExceptionHandler(void)
{

  NVIC_SystemReset();
}

/**
  * @brief HardFault Handler
  * @param None.
  * @retval None.
  */
void  HardFault_Handler(void)
{
  SE_NMI_ExceptionHandler();
}

/**
  * @brief MPU Fault Handler
  * @param None.
  * @retval None.
  */
void   MemManage_Handler(void)
{
  SE_NMI_ExceptionHandler();
}


/**
  * @brief Bus Fault Handler
  * @param None.
  * @retval None.
  */
void   BusFault_Handler(void)
{
  SE_NMI_ExceptionHandler();
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
