/**
  ******************************************************************************
  * @file    se_bootinfo.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for SE BootInfo functionalities.
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
#ifndef SE_BOOTINFO_H
#define SE_BOOTINFO_H

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

/** @addtogroup SE_BOOTINFO SE BootInfo
  * @{
  */

/** @defgroup SE_BOOTINFO_Exported_Types Exported Types
  * @{
  */

/**
  * @brief Specific the BootInfoArea status
  */
typedef enum
{
  SE_INFO_NOT_ACCESSED = 0U,  /*!< BootInfo is not initialized/validated yet */
  SE_INFO_VALID,              /*!< BootInfo is correctly found and verified */
  SE_INFO_RECOVERED,          /*!< Problem detected, but recovered thanks to the BootInfoBckp */
  SE_INFO_NOT_FOUND,          /*!< BootInfo is not present. Empty */
  SE_INFO_NOT_VALID,          /*!< BootInfo is not valid and recover from BootInfoBckp is not possible */
} SE_INFO_StatusTypedef;

/**
  * @brief Specifies the BootInfo data structure
  */
typedef struct
{
  uint32_t LastExecStatus;                   /*!< The last execution status set by the Secure Boot before a
                                                  reset/reboot occurred */
  uint32_t LastExecError;                    /*!< The last execution error set by the Secure Boot before a
                                                  reset/reboot occurred */
  uint32_t ConsecutiveBootOnErrorCounter;    /*!< Counter counting the number of consecutive boot procedures triggered
                                                  by an error (in UserApp or SB_SFU). Reset as soon as a normal boot
                                                  procedure (power off/on) is performed. */
  uint32_t CRC32;                            /*!< The 32bit checksum calculated on the previous structure fields.
                                                  It is mandatory to keep this field as the last uint32_t of this
                                                  structure. */
} SE_BootInfoTypeDef;

/**
  * @}
  */

/** @defgroup SE_BOOTINFO_Exported_Functions Exported Functions
  * @{
  */

/** @addtogroup SE_BOOTINFO_Initialization_Functions
  * @{
  */
SE_ErrorStatus SE_INFO_BootInfoAreaInit(SE_INFO_StatusTypedef *peBootInfoStatus);
SE_ErrorStatus SE_INFO_BootInfoAreaDeInit(void);

/**
  * @}
  */

/** @addtogroup SE_BOOTINFO_Control_Functions
  * @{
  */
SE_ErrorStatus SE_INFO_BootInfoAreaFactoryReset(void);
SE_ErrorStatus SE_INFO_ReadBootInfoArea(SE_BootInfoTypeDef *pxBootInfo, SE_INFO_StatusTypedef *peBootInfoStatus);
SE_ErrorStatus SE_INFO_WriteBootInfoArea(SE_BootInfoTypeDef *pxBootInfo, SE_INFO_StatusTypedef *peBootInfoStatus);
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

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* SE_BOOTINFO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

