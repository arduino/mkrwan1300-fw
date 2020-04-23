/**
  ******************************************************************************
  * @file    se_interface_bootloader.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Engine Reserved Interface
  *          module functionalities. These services are used by the bootloader.
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
#ifndef SE_INTERFACE_BOOTLOADER_H
#define SE_INTERFACE_BOOTLOADER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "se_def.h"
#include "se_bootinfo.h"

/** @addtogroup SE Secure Engine
  * @{
  */

/** @addtogroup SE_INTERFACE
  * @{
  */

/** @addtogroup SE_INTERFACE_BOOTLOADER
  * @{
  */

/** @addtogroup SE_INTERFACE_BOOTLOADER_Exported_Functions
  * @{
  */

/* SE Initialization functions */
SE_ErrorStatus SE_Init(SE_StatusTypeDef *peSE_Status, uint32_t uSystemCoreClock);
SE_ErrorStatus SE_Startup(void);

/* BootInfo handlers */
SE_ErrorStatus SE_INFO_ReadBootInfo(SE_StatusTypeDef *peSE_Status, SE_BootInfoTypeDef *pxSE_BootInfo);
SE_ErrorStatus SE_INFO_WriteBootInfo(SE_StatusTypeDef *peSE_Status, SE_BootInfoTypeDef *pxSE_BootInfo);

/* Lock function to prevent the user-app from running bootloader reserved code */
SE_ErrorStatus SE_LockRestrictServices(SE_StatusTypeDef *pSE_Status);

/* Images handling functions */
SE_ErrorStatus SE_SFU_IMG_Erase(SE_StatusTypeDef *pSE_Status, void *pDestination, uint32_t Length);
SE_ErrorStatus SE_SFU_IMG_Read(SE_StatusTypeDef *pSE_Status, void *pDestination, const void *pSource, uint32_t Length);
SE_ErrorStatus SE_SFU_IMG_Write(SE_StatusTypeDef *pSE_Status, void *pDestination, const void *pSource, uint32_t Length);
SE_ErrorStatus SE_VerifyFwRawHeaderTag(SE_StatusTypeDef *peSE_Status, SE_FwRawHeaderTypeDef *pxFwRawHeader);

/* Crypto-agnostic functions */
SE_ErrorStatus SE_Decrypt_Init(SE_StatusTypeDef *peSE_Status, SE_FwRawHeaderTypeDef *pxSE_Metadata, int32_t SE_FwType);
SE_ErrorStatus SE_Decrypt_Append(SE_StatusTypeDef *peSE_Status, const uint8_t *pInputBuffer, int32_t InputSize,
                                 uint8_t *pOutputBuffer, int32_t *pOutputSize);
SE_ErrorStatus SE_Decrypt_Finish(SE_StatusTypeDef *peSE_Status, uint8_t *pOutputBuffer, int32_t *pOutputSize);
SE_ErrorStatus SE_AuthenticateFW_Init(SE_StatusTypeDef *peSE_Status, SE_FwRawHeaderTypeDef *pxSE_Metadata,
                                      int32_t SE_FwType);
SE_ErrorStatus SE_AuthenticateFW_Append(SE_StatusTypeDef *peSE_Status, const uint8_t *pInputBuffer, int32_t InputSize,
                                        uint8_t *pOutputBuffer, int32_t *pOutputSize);
SE_ErrorStatus SE_AuthenticateFW_Finish(SE_StatusTypeDef *peSE_Status, uint8_t *pOutputBuffer, int32_t *pOutputSize);

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

#endif /* SE_INTERFACE_BOOTLOADER_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
