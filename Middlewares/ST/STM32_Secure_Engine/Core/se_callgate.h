/**
  ******************************************************************************
  * @file    se_callgate.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Engine CALLGATE module
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
#ifndef SE_CALLGATE_H
#define SE_CALLGATE_H

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

/** @addtogroup SE_CALLGATE SE CallGate
  * @{
  */

/** @defgroup SE_CALLGATE_Exported_Constants Exported Constants
  * @{
  */

/** @defgroup SE_CALLGATE_ID_Functions Secure Engine CallGate Function ID structure definition
  * @{
  */

/*Generic functions*/
#define SE_INIT_ID                                (0x00UL)    /*!< Secure Engine Init     */

/* CRYPTO Low level functions for bootloader only */
#define SE_CRYPTO_LL_DECRYPT_INIT_ID              (0x04UL)    /*!< CRYPTO Low level Decrypt_Init */
#define SE_CRYPTO_LL_DECRYPT_APPEND_ID            (0x05UL)    /*!< CRYPTO Low level Decrypt_Append */
#define SE_CRYPTO_LL_DECRYPT_FINISH_ID            (0x06UL)    /*!< CRYPTO Low level Decrypt_Finish */
#define SE_CRYPTO_LL_AUTHENTICATE_FW_INIT_ID      (0x07UL)    /*!< CRYPTO Low level Authenticate_FW_Init */
#define SE_CRYPTO_LL_AUTHENTICATE_FW_APPEND_ID    (0x08UL)    /*!< CRYPTO Low level Authenticate_FW_Append */
#define SE_CRYPTO_LL_AUTHENTICATE_FW_FINISH_ID    (0x09UL)    /*!< CRYPTO Low level Authenticate_FW_Finish */

/* CRYPTO High level functions for bootloader only */
#define SE_CRYPTO_HL_AUTHENTICATE_METADATA        (0x10UL)    /*!< CRYPTO High level Authenticate Metadata */

/* Next ranges are kept for future use (additional crypto schemes, additional user code) */
#define SE_APP_GET_ACTIVE_FW_INFO                 (0x20UL)    /*!< User Application retrieves the Active 
                                                                             Firmware Info */

/* BootInfo access functions (bootloader only) */
#define SE_BOOT_INFO_READ_ALL_ID                  (0x80UL)    /*!< SE_INFO_ReadBootInfo (bootloader only) */
#define SE_BOOT_INFO_WRITE_ALL_ID                 (0x81UL)    /*!< SE_INFO_WriteBootInfo (bootloader only) */

/* SE IMG interface (bootloader only) */
#define SE_IMG_READ                               (0x92UL)    /*!< SFU reads a Flash protected area (bootloader only) */
#define SE_IMG_WRITE                              (0x93UL)    /*!< SFU write a Flash protected area (bootloader only) */
#define SE_IMG_ERASE                              (0x94UL)    /*!< SFU erase a Flash protected area (bootloader only) */

/* LOCK service to be used by the bootloader only */
#define SE_LOCK_RESTRICT_SERVICES                 (0x100UL)   /*!< SFU lock part of SE services (bootloader only) */

/* Secure Engine add-on middle wares */
#define SE_MW_ADDON_MSB_MASK     (0x70000000U)         /*!< SE add-ons MSB bits reserved for add-on middle wares IDs */
#define SE_MW_ADDON_KMS_MSB      (0x10000000U)         /*!< KMS services ID range begin */


typedef uint32_t SE_FunctionIDTypeDef;                 /*!< Secure Engine CallGate Function ID structure definition */

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup SE_CALLGATE_Exported_Functions
  * @{
  */
SE_ErrorStatus SE_CallGate(SE_FunctionIDTypeDef eID, SE_StatusTypeDef *peSE_Status, ...);

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

#endif /* SE_CALLGATE_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
