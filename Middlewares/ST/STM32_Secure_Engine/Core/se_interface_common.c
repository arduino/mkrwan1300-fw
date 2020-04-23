/**
  ******************************************************************************
  * @file    se_interface_common.c
  * @author  MCD Application Team
  * @brief   Secure Engine Interface module.
  *          This file provides set of common firmware functions to manage SE Interface
  *          functionalities. These services are used by the se_interface_bootloader.c
  *          and se_interface_application.c files.
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

/* Place code in a specific section*/
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".SE_IF_Code"
#elif defined(__CC_ARM)
#pragma arm section code = ".SE_IF_Code"
#endif /* __ICCARM__ */

#include "main.h"                       /* se_interface_common.c is compiled in SBSFU project using main.h from 
                                         * this project 
                                         */
#include "se_low_level.h"               /* This file is part of SE_CoreBin and adapts the Secure Engine 
                                         * (and its interface) to the STM32 board specificities 
                                         */
#include "se_interface_common.h"
#if defined(SFU_ISOLATE_SE_WITH_MPU)
#include "se_callgate.h"
#include "sfu_low_level.h"
#endif /* SFU_ISOLATE_SE_WITH_MPU */
#include "se_intrinsics.h"

/** @addtogroup SE Secure Engine
  * @{
  */

/** @addtogroup  SE_INTERFACE Secure Engine Interface
  * @{
  */

/** @defgroup SE_INTERFACE_COMMON Common elements for SE interface
  *  @brief Common elements used by se_interface_application and se_interface_bootloader.
  *  @note Please note that this code is compiled in the SB_SFU project (not in SE_CoreBin).
  *        Nevertheless, this file also includes "se_low_level.h" from SE_CoreBin to make sure the SE interface is in
  *        sync with the SE code.
  * @{
  */
/* Private variables ---------------------------------------------------------*/
uint32_t primask_bit;                            /*!< interruption mask saved when disabling ITs then restore when 
                                                      re-enabling ITs */ 

/* Functions Definition ------------------------------------------------------*/

/** @defgroup SE_INTERFACE_COMMON_Functions SE Interface common functions
  *  @brief Functions used by se_interface_bootloader and se_interface_application
  * @{
  */

/** @defgroup SE_INTERFACE_COMMON_SecModeFunctions SE Interface common functions for Secure Mode
  *  @brief Functions used by se_interface_bootloader and se_interface_application to handle the Secure Mode
  * @{
  */

/**
  * @brief SE Enter Secure Mode.
  * @param None.
  * @retval None.
  */
void SE_EnterSecureMode(void)
{
#if !defined(CKS_ENABLED)
  /* Disable interrupts */
  primask_bit = __get_PRIMASK();
  __disable_irq();
#endif /* !CKS_ENABLED */
}

/**
  * @brief SE Exit Secure Mode.
  * @param None.
  * @retval None.
  */
void SE_ExitSecureMode(void)
{
#if !defined(CKS_ENABLED)
  /* Re-enable the interrupts */
  __set_PRIMASK(primask_bit);
#endif /* !CKS_ENABLED */
}

/**
  * @}
  */

#if defined(SFU_ISOLATE_SE_WITH_MPU)

/** @defgroup SE_INTERFACE_COMMON_MpuIsolationFunctions SE Interface common functions for MPU Isolation
  *  @brief Functions used by se_interface_bootloader and se_interface_application to handle the MPU isolation of the
  *         Secure Engine
  * @{
  */

/**
  * @brief This is a helper function to determine if we are currently running in non-privileged mode or not
  * @param void
  * @retval 0 if we are in privileged mode, 1 if we are in non-privileged mode
  */
uint32_t SE_IsUnprivileged(void)
{
  return ((__get_IPSR() == 0) && ((__get_CONTROL() & 1) == 1));
}

/**
  * @brief This functions triggers a Secure Engine system call (supervisor call): request privileged operation
  * @param e_ret_status return code (see @ref SE_ErrorStatus)
  * @param syscall Identifier of the service to be called (see @ref SE_FunctionIDTypeDef)
  * @param peSE_Status Secure Engine Status (see @ref SE_StatusTypeDef)
  * @param params arguments
  * @retval void
  */
void SE_SysCall(SE_ErrorStatus *e_ret_status, uint32_t syscall, SE_StatusTypeDef *peSE_Status, void *params)
{
  /*
    * You cannot directly change to privileged mode from unprivileged mode without going through an exception,
    * for example an SVC.
    * Handled by @ref MPU_SVC_Handler()  and finally @ref SE_SVC_Handler()
    */
  __ASM volatile("SVC #0");  /* 0 is the hard-coded value to indicate a Secure Engine syscall */
}

/**
  * @brief This function triggers a Secure Engine Privileged Operation requested with SE_SysCall
  * @param args arguments
  *             The second argument is the identifier of the requested operation.
  * @retval uint32_t
  * @note When this function is entered, privileged mode has already been entered.
  *       When leaving this function, the unprivileged mode is automatically enabled.
  * @note This function is exported to the MPU SVC handler (see @ref MPU_SVC_Handler).
  */
void SE_SVC_Handler(uint32_t *args)
{
  SE_ErrorStatus ret;
  SE_ErrorStatus *pRet = (SE_ErrorStatus *)(args[0]);
  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode();

  switch (args[1]) /* main case for short operations that can be done under interrupts */
  {
    /* no extra parameters calls */
    case SE_LOCK_RESTRICT_SERVICES:
      ret = (*SE_CallGatePtr)((SE_FunctionIDTypeDef)args[1],
                              (SE_StatusTypeDef *)args[2]);
      break;

    /* one extra parameters calls */
    case SE_BOOT_INFO_READ_ALL_ID:
    case SE_BOOT_INFO_WRITE_ALL_ID:
    case SE_CRYPTO_HL_AUTHENTICATE_METADATA:
    case SE_APP_GET_ACTIVE_FW_INFO:
      ret = (*SE_CallGatePtr)((SE_FunctionIDTypeDef)args[1],
                              (SE_StatusTypeDef *)args[2],
                              (((uint32_t *)args[3])[0]));
      break;
    /* two extra parameters calls */
    case SE_CRYPTO_LL_DECRYPT_INIT_ID:
    case SE_CRYPTO_LL_AUTHENTICATE_FW_INIT_ID:
    case SE_CRYPTO_LL_DECRYPT_FINISH_ID:
    case SE_CRYPTO_LL_AUTHENTICATE_FW_FINISH_ID:
    case SE_IMG_ERASE:
      ret = (*SE_CallGatePtr)((SE_FunctionIDTypeDef)args[1],
                              (SE_StatusTypeDef *)args[2],
                              (void *)(((uint32_t *)args[3])[0]),
                              (void *)(((uint32_t *)args[3])[1]));
      break;
    /* three extra parameters calls */
    case SE_IMG_READ:
    case SE_IMG_WRITE:
      ret = (*SE_CallGatePtr)((SE_FunctionIDTypeDef)args[1],
                              (SE_StatusTypeDef *)args[2],
                              (void *)(((uint32_t *)args[3])[0]),
                              (void *)(((uint32_t *)args[3])[1]),
                              (void *)(((uint32_t *)args[3])[2]));
      break;
    /* four extra parameters calls */
    case SE_CRYPTO_LL_DECRYPT_APPEND_ID:
    case SE_CRYPTO_LL_AUTHENTICATE_FW_APPEND_ID:
      ret = (*SE_CallGatePtr)((SE_FunctionIDTypeDef)args[1],
                              (SE_StatusTypeDef *)args[2],
                              (void *)(((uint32_t *)args[3])[0]),
                              (void *)(((uint32_t *)args[3])[1]),
                              (void *)(((uint32_t *)args[3])[2]),
                              (void *)(((uint32_t *)args[3])[3]));
      break;

    default:
      ret = SE_ERROR;
  }
  /*  Check return value address  */
  if ((SFU_LL_Buffer_in_ram((void *)pRet, sizeof(*pRet)) == SFU_SUCCESS)
      && (SFU_LL_BufferCheck_in_se_ram((void *)pRet, sizeof(*pRet)) == SFU_ERROR))
  {
    *pRet = ret;
  }

  /* Exit Secure Mode*/
  SE_ExitSecureMode();

  /*
    * End of the privileged operation execution: switching to unprivileged mode automatically.
    */
}

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

/* Stop placing data in specified section*/
#if defined(__ICCARM__)
#pragma default_function_attributes =
#elif defined(__CC_ARM)
#pragma arm section code
#endif /* __ICCARM__ */
