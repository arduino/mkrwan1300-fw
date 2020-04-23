/**
  ******************************************************************************
  * @file    se_def_metadata.h
  * @author  MCD Application Team
  * @brief   This file contains metadata definitions for SE functionalities.
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
#ifndef SE_DEF_METADATA_H
#define SE_DEF_METADATA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "se_crypto_config.h"

/** @addtogroup  SE Secure Engine
  * @{
  */

/** @addtogroup  SE_CORE SE Core
  * @{
  */

/** @addtogroup  SE_CORE_DEF SE Definitions
  * @{
  */

/** @defgroup SE_DEF_METADATA SE Metadata Definitions
  *  @brief definitions related to FW metadata (FW header) which are crypto dependent.
  *  @note This file is a template file and must be adapted for each cryptographic scheme.
  *  The se_def_metadata.h file declared in the SE_CoreBin project must be included by the SB_SFU project.
  * @{
  */

/** @defgroup SE_DEF_METADATA_Exported_Constants Exported Constants
  * @{
  */

/**
  * @brief  Firmware Image Header (FW metadata) constants
  */

#define SE_FW_HEADER_TOT_LEN    ((int32_t) sizeof(SE_FwRawHeaderTypeDef))   /*!< FW INFO header Total Length*/
#define SE_FW_HEADER_METADATA_LEN    ((int32_t) sizeof(SE_FwRawHeaderTypeDef))   /*!< FW Metadata INFO header Length*/

/**
  * @}
  */

/** @defgroup SE_DEF_METADATA_Exported_Types Exported Types
  * @{
  */

/**
  * @brief  Secure Engine crypto structure definition
  * There must be a Firmware Header structure definition
  * This structure MUST be called SE_FwRawHeaderTypeDef
  * This structure MUST contain a field named 'FwVersion'
  * This structure MUST contain a field named 'FwSize'
  */

/**
  * @brief  Firmware Header structure definition
  * @note This structure MUST be called SE_FwRawHeaderTypeDef
  * @note This structure MUST contain a field named 'FwVersion'
  * @note This structure MUST contain a field named 'FwSize'
  */
typedef struct
{
  uint16_t FwVersion;              /*!< Firmware version*/
  uint32_t FwSize;                 /*!< Firmware size (bytes)*/
  /* If you want to support Firmware Authentication then the field below is also required */
  /* uint8_t  FwTag[SE_TAG_LEN]; */     /*!< Firmware Tag*/
  /* If you want to authenticate the Firmware metadata the the field below is also required */
  /* uint8_t  HeaderMAC[SE_TAG_LEN]; */  /*!< MAC of the full header message */
} SE_FwRawHeaderTypeDef;

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

#endif /* SE_DEF_METADATA_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

