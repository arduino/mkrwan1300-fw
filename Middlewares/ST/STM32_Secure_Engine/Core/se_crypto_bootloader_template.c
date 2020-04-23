/**
  ******************************************************************************
  * @file    se_crypto_bootloader.c
  * @author  MCD Application Team
  * @brief   Secure Engine CRYPTO module.
  *          This file provides set of firmware functions to manage SE Crypto
  *          functionalities. These services are used by the bootloader.
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
#include "se_crypto_bootloader.h"
#include "se_crypto_common.h"     /* re-use common crypto code (wrapper to cryptolib in this example)  */
#include "se_key.h"               /* required to access the keys */

/** @addtogroup SE Secure Engine
  * @{
  */

/** @addtogroup SE_CORE SE Core
  * @{
  */

/** @defgroup  SE_CRYPTO SE Crypto
  * @brief Crypto services (used by the bootloader and common crypto functions)
  * @{
  */

/** @defgroup SE_CRYPTO_BOOTLOADER SE Crypto for Bootloader
  * @brief Crypto functions used by the bootloader.
  *        The implementation of these functions is crypto-dependent.
  *        These functions use the generic SE_FwRawHeaderTypeDef structure to fill crypto specific structures.
  * @{
  */

/** @defgroup SE_CRYPTO_BOOTLOADER_Private_Variables Private Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup SE_CRYPTO_BOOTLOADER_Private_Macros Private Macros
  * @{
  */


/**
  * @brief Clean up the RAM area storing the Firmware key.
  *        This applies only to the secret symmetric key loaded with SE_ReadKey().
  */
#define SE_CLEAN_UP_FW_KEY() do { /* implement if needed */; } while(0)

/**
  * @brief Clean up the RAM area storing the ECC Public Key.
  *        This applies only to the public asymmetric key loaded with SE_ReadKey_Pub().
  */
#define SE_CLEAN_UP_PUB_KEY() do { /* implement if needed */; } while(0)

/**
  * @}
  */



/** @defgroup SE_CRYPTO_BOOTLOADER_Exported_Functions Exported Functions
  * @brief The implementation of these functions is crypto-dependent.
  *        These functions use the generic SE_FwRawHeaderTypeDef structure to fill crypto specific structures.
  * @{
  */

SE_ErrorStatus SE_CRYPTO_Encrypt_Init(SE_FwRawHeaderTypeDef *pxSE_Metadata)
{
  /* use the common cypto code from se_crypto_common.c when possible */
}

SE_ErrorStatus SE_CRYPTO_Header_Append(const uint8_t *pInputBuffer, int32_t InputSize)
{
  /* use the common cypto code from se_crypto_common.c when possible */
}

SE_ErrorStatus SE_CRYPTO_Encrypt_Append(const uint8_t *pInputBuffer, int32_t InputSize, uint8_t *pOutputBuffer,
                                        int32_t *pOutputSize)
{
  /* use the common cypto code from se_crypto_common.c when possible */
}

SE_ErrorStatus SE_CRYPTO_Encrypt_Finish(uint8_t *pOutputBuffer, int32_t *pOutputSize)
{
  /* use the common cypto code from se_crypto_common.c when possible */
}

SE_ErrorStatus SE_CRYPTO_Decrypt_Init(SE_FwRawHeaderTypeDef *pxSE_Metadata)
{
  /* use the common cypto code from se_crypto_common.c when possible */
}

SE_ErrorStatus SE_CRYPTO_Decrypt_Append(const uint8_t *pInputBuffer, int32_t InputSize, uint8_t *pOutputBuffer,
                                        int32_t *pOutputSize)
{
  /* use the common cypto code from se_crypto_common.c when possible */
}

SE_ErrorStatus SE_CRYPTO_Decrypt_Finish(uint8_t *pOutputBuffer, int32_t *pOutputSize)
{
  /* use the common cypto code from se_crypto_common.c when possible */
}

SE_ErrorStatus SE_CRYPTO_Authenticate_Metadata(SE_FwRawHeaderTypeDef *pxSE_Metadata)
{
  /* use the common cypto code from se_crypto_common.c when possible */
}

SE_ErrorStatus SE_CRYPTO_AuthenticateFW_Init(SE_FwRawHeaderTypeDef *pxSE_Metadata)
{
  /* use the common cypto code from se_crypto_common.c when possible */
}

SE_ErrorStatus SE_CRYPTO_AuthenticateFW_Append(const uint8_t *pInputBuffer, int32_t InputSize, uint8_t *pOutputBuffer,
                                               int32_t *pOutputSize)
{
  /* use the common cypto code from se_crypto_common.c when possible */
}

SE_ErrorStatus SE_CRYPTO_AuthenticateFW_Finish(uint8_t *pOutputBuffer, int32_t *pOutputSize)
{
  /* use the common cypto code from se_crypto_common.c when possible */
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

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
