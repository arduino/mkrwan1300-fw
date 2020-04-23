/**
  ******************************************************************************
  * @file    se_utils.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Engine UTILS module
  *          functionalities.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SE_EXCEPTION_H
#define SE_EXCEPTION_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "se_def.h"

/** @addtogroup SE Secure Engine
  * @{
  */

/** @addtogroup SE_CORE SE Core
  * @{
  */

/** @addtogroup SE_EXCEPTIONS SE Exception
  * @{
  */

/** @addtogroup SE_EXCEPTIONS_Exported_Functions
  * @{
  */
void SE_NMI_ExceptionHandler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);

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

#ifdef __cplusplus
}
#endif

#endif /* SE_EXCEPTION_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

