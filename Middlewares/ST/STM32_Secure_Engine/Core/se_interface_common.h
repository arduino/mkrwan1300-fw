/**
  ******************************************************************************
  * @file    se_interface_common.h
  * @author  MCD Application Team
  * @brief   This file contains common definitions for Secure Engine Interface
  *          module functionalities.
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
#ifndef SE_INTERFACE_COMMON_H
#define SE_INTERFACE_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#elif defined(__CC_ARM)
#include "mapping_sbsfu.h"
#endif /* (__ICCARM__) || (__GNUC__) */
#include "se_def.h"
/** @addtogroup SE Secure Engine
  * @{
  */

/** @addtogroup SE_INTERFACE
  * @{
  */

/** @addtogroup  SE_INTERFACE_COMMON
  * @{
  */

/** @defgroup SE_INTERFACE_COMMON_Private_Defines Private Defines
  * @{
  */

/**
  * @brief Secure Engine Middleware version
  */
#define SE_MDW_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define SE_MDW_VERSION_SUB1   (0x00U) /*!< [23:16] sub1 version */
#define SE_MDW_VERSION_SUB2   (0x00U) /*!< [15:8]  sub2 version */
#define SE_MDW_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */
#define SE_MDW_VERSION        ((SE_MDW_VERSION_MAIN << 24U)\
                               |(SE_MDW_VERSION_SUB1 << 16U)\
                               |(SE_MDW_VERSION_SUB2 << 8U )\
                               |(SE_MDW_VERSION_RC)) /*!< Fw Version */

/**
  * @}
  */


/** @defgroup SE_INTERFACE_COMMON_Private_Macros Private Macros
  * @{
  */

/**
  * @brief Check if the caller is located in SBSFU region
  */
#define __IS_SFU_RESERVED() \
  do{ \
    if ((__get_LR())< SB_REGION_ROM_START){\
      return SE_ERROR;}\
    if ((__get_LR())> SB_REGION_ROM_END){\
      return SE_ERROR;}\
  }while(0)

/**
  * @brief This macro sets the CallGate function pointer.
  * A local variable is used to avoid constraints regarding the SRAM1 handling,
  * as the SE_Init is called in SB_SFU context but the se_interface functions can be called in the UserApp context too.
  */
#define SET_CALLGATE() \
  SE_ErrorStatus(*SE_CallGatePtr)(SE_FunctionIDTypeDef ID, SE_StatusTypeDef *peSE_Status, \
                                  ...);  /*!< Secure Engine CALLGATE  pointer function*/ \
  SE_CallGatePtr = (SE_ErrorStatus(*)(SE_FunctionIDTypeDef, SE_StatusTypeDef *, \
                                      ...))((uint32_t) SE_CALLGATE_REGION_ROM_START + 1U);
/**
  * @}
  */


/** @defgroup SE_INTERFACE_COMMON_Functions SE Interface common functions
  *  @brief Functions used by se_interface_bootloader and se_interface_application
  * @{
  */


/** @defgroup SE_INTERFACE_COMMON_SecModeFunctions SE Interface common functions for Secure Mode
  *  @brief Functions used by se_interface_bootloader and se_interface_application to handle the Secure Mode
  * @{
  */
void SE_EnterSecureMode(void);
void SE_ExitSecureMode(void);

/**
  * @}
  */

#if defined(SFU_ISOLATE_SE_WITH_MPU)
/** @defgroup SE_INTERFACE_COMMON_MpuIsolationFunctions SE Interface common functions for MPU Isolation
  *  @brief Functions used by se_interface_bootloader and se_interface_application to handle the MPU isolation of the
  *         Secure Engine
  * @{
  */

uint32_t SE_IsUnprivileged(void);
void SE_SysCall(SE_ErrorStatus *e_ret_status, uint32_t syscall, SE_StatusTypeDef *peSE_Status, void *params);
void SE_SVC_Handler(uint32_t *args);
/**
  * @}
  */
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

#endif /* SE_INTERFACE_COMMON_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

