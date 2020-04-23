/**
  ******************************************************************************
  * @file    se_interface_application.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Engine Interface module
  *          functionalities. These services are used by the application.
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
#ifndef SE_INTERFACE_APPLICATION_H
#define SE_INTERFACE_APPLICATION_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "se_def.h"
#include "se_user_application.h"

/** @addtogroup SE Secure Engine
  * @{
  */

/** @addtogroup SE_INTERFACE Secure Engine Interface
  * @{
  */

/** @addtogroup  SE_INTERFACE_APPLICATION
  * @{
  */

/** @addtogroup SE_INTERFACE_APPLICATION_Exported_Functions
  * @brief These are the functions the User Application can call to use the SE services.
  * @{
  */

/*
 * The functions below are demonstrating how the user application can use some SE services.
 */

SE_ErrorStatus SE_APP_GetActiveFwInfo(SE_StatusTypeDef *peSE_Status, SE_APP_ActiveFwInfo_t *p_FwInfo);
#ifdef SFU_ISOLATE_SE_WITH_MPU
void SE_APP_SVC_Handler(uint32_t *args);
#endif /* SFU_ISOLATE_SE_WITH_MPU */

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

#endif /* SE_INTERFACE_APPLICATION_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

