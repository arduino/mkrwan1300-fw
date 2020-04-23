/**
  ******************************************************************************
  * @file    se_interface_application.c
  * @author  MCD Application Team
  * @brief   Secure Engine Interface module.
  *          This file provides set of firmware functions to manage SE Interface
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

/* Place code in a specific section*/
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".SE_IF_Code"
#elif defined(__CC_ARM)
#pragma arm section code = ".SE_IF_Code"
#endif /* __ICCARM__ */

/* Includes ------------------------------------------------------------------*/
#include "main.h"                       /* se_interface_application.c is compiled in SBSFU project using main.h 
                                         * from this project 
                                         */
#include "se_low_level.h"               /* This file is part of SE_CoreBin and adapts the Secure Engine 
                                         * (and its interface) to the STM32 board specificities 
                                         */
#include "se_interface_common.h"
#include "se_callgate.h"
#include "se_interface_application.h"
#include "se_intrinsics.h"
/*
*/
/** @addtogroup SE Secure Engine
  * @{
  */

/** @defgroup  SE_INTERFACE Secure Engine Interface
  * @brief APIs the SE users (bootloader, user app) can call.
  * @note Please note that this code is compiled in the SB_SFU project (not in SE_CoreBin).
  *       Nevertheless, this file also includes "se_low_level.h" from SE_CoreBin to make sure the SE interface is in
  *       sync with the SE code.
  * @{
  */

/** @defgroup  SE_INTERFACE_APPLICATION Secure Engine Interface for Application
  * @brief Interface functions the User Application calls to use SE services.
  * @{
  *
  * "se_interface" is built and set in the context of the SB_SFU project.
  * Then some symbols are exported to the User Application for the User Application
  * to call some Secure Engine services through "se_interface"
  * (SB_SFU and the User Application are exclusive, they do not run in parallel).
  * As "se_interface" is set by SB_SFU but can be used by the User Application, it is important
  * not to introduce global variables if you do not want to create dependencies between the SB_SFU RAM mapping (SRAM1)
  * and the User Application RAM mapping.
  */

/* DO NOT ADD ANY VARIABLE HERE, SEE EXPLANATIONS ABOVE */

/** @defgroup SE_INTERFACE_APPLICATION_Exported_Functions Exported Functions
  * @ Functions the User Application calls to run the SE services.
  * @{
  */

/**
  * @brief Service called by the User Application to retrieve the Active Firmware Info.
  * @param peSE_Status Secure Engine Status.
  * @param p_FwInfo Active Firmware Info structure that will be filled.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
__root SE_ErrorStatus SE_APP_GetActiveFwInfo(SE_StatusTypeDef *peSE_Status, SE_APP_ActiveFwInfo_t *p_FwInfo)
{
  SE_ErrorStatus e_ret_status;

#ifdef SFU_ISOLATE_SE_WITH_MPU
  if (0 != SE_IsUnprivileged())
  {
    uint32_t params[1] = {(uint32_t)p_FwInfo};
    SE_SysCall(&e_ret_status, SE_APP_GET_ACTIVE_FW_INFO, peSE_Status, &params);
  }
  else
  {
#endif /* SFU_ISOLATE_SE_WITH_MPU */

    /* Set the CallGate function pointer */
    SET_CALLGATE();

    /*Enter Secure Mode*/
    SE_EnterSecureMode();

    /*Secure Engine Call*/
    e_ret_status = (*SE_CallGatePtr)(SE_APP_GET_ACTIVE_FW_INFO, peSE_Status, p_FwInfo);

    /*Exit Secure Mode*/
    SE_ExitSecureMode();
#ifdef SFU_ISOLATE_SE_WITH_MPU
  }
#endif /* SFU_ISOLATE_SE_WITH_MPU */
  return e_ret_status;
}
#ifdef SFU_ISOLATE_SE_WITH_MPU
__root void SE_APP_SVC_Handler(uint32_t *args)
{
  SE_SVC_Handler(args);
}
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


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
