/**
  ******************************************************************************
  * @file    se_key.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Engine KEY module
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
#ifndef SE_KEY_H
#define SE_KEY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup  SE Secure Engine
  * @{
  */

/** @addtogroup SE_KEY SE Key
  * @{
  */


/** @addtogroup SE_KEY_Exported_Functions
  * @brief Functions SE_CoreBin can use to retrieve the keys.
  *        These functions are implemented in se_key.s
  * @note Depending on the crypto scheme, @ref SE_ReadKey or @ref SE_ReadKey_Pub can be useless.
  *       Nevertheless, we do not use compiler switches as the linker will remove the unused function(s).
  * @{
  */
/**
  * @brief Function to retrieve the symmetric key.
  * @param pKey: pointer to an array of uint8_t with the appropriate size
  * @retval void
  */
void SE_ReadKey(uint8_t *pKey);
/**
  * @brief Function to retrieve the public asymmetric key.
  * @param pPubKey: pointer to an array of uint8_t with the appropriate size
  * @retval void
  */
void SE_ReadKey_Pub(uint8_t *pPubKey);
/**
  * @brief Function to retrieve the external token pairing keys.
  * @param pPairingKey: pointer to an array of uint8_t with the appropriate size (32 bytes)
  * @retval void
  */
void SE_ReadKey_Pairing(uint8_t *pPairingKey);
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

#endif /* SE_KEY_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

