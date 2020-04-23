/**
  ******************************************************************************
  * @file    se_low_level.c
  * @author  MCD Application Team
  * @brief   Secure Engine Interface module.
  *          This file provides set of firmware functions to manage SE low level
  *          interface functionalities.
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
#endif /* __ICCARM__ */

/* Includes ------------------------------------------------------------------*/
#include "se_low_level_template.h"

/** @addtogroup SE Secure Engine
  * @{
  */

/** @defgroup  SE_HARDWARE Low Layer Interface
  * @{
  */


/** @defgroup SE_HARDWARE_Private_Variables Private Variables
  * @{
  */
static CRC_HandleTypeDef CrcHandle;                  /*!< SE Crc Handle*/

/**
  * @}
  */


/** @defgroup SE_HARDWARE_Exported_Functions Exported Functions
  * @{
  */

/** @defgroup SE_HARDWARE_Exported_CRC_Functions CRC Exported Functions
  * @{
  */

/**
  * @brief  Set CRC configuration and call HAL CRC initialization function.
  * @param  None.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise
  */
SE_ErrorStatus SE_LL_CRC_Config(void)
{
  return SE_SUCCESS;
}

/**
  * @brief  Wrapper to HAL CRC initilization function.
  * @param  None
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_CRC_Init(void)
{
  return SE_SUCCESS;
}

/**
  * @brief  Wrapper to HAL CRC de-initilization function.
  * @param  None
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_CRC_DeInit(void)
{
  return SE_SUCCESS;
}

/**
  * @brief  Wrapper to HAL CRC Calculate function.
  * @param  pBuffer: pointer to data buffer.
  * @param  uBufferLength: buffer length in 32-bits word.
  * @retval uint32_t CRC (returned value LSBs for CRC shorter than 32 bits)
  */
uint32_t SE_LL_CRC_Calculate(uint32_t pBuffer[], uint32_t uBufferLength)
{
  return 0;
}

/**
  * @}
  */

/** @defgroup SE_HARDWARE_Exported_FLASH_Functions FLASH Exported Functions
  * @{
  */

/**
  * @brief  This function does an erase of nb pages in user flash area
  * @param  pStart: pointer to  user flash area
  * @param  Length: number of bytes.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_FLASH_Erase(void *pStart, uint32_t Length)
{
  return SE_SUCCESS;
}


/**
  * @brief  Write in Flash protected area
  * @param  pDestination pointer to destination area in Flash
  * @param  pSource pointer to input buffer
  * @param  Length number of bytes to be written
  * @retval SE_SUCCESS if successful, otherwise SE_ERROR
  */

SE_ErrorStatus SE_LL_FLASH_Write(void *pDestination, const void *pSource, uint32_t Length)
{
  return SE_SUCCESS;
}

/**
  * @brief  Read in Flash protected area
  * @param  pDestination: Start address for target location
  * @param  pSource: pointer on buffer with data to read
  * @param  Length: Length in bytes of data buffer
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_FLASH_Read(void *pDestination, const void *pSource, uint32_t Length)
{
  return SE_SUCCESS;
}

/**
  * @brief Flash IRQ Handler
  * @param None.
  * @retval None.
  */
void FLASH_IRQHandler(void)
{
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

/* Stop placing data in specified section*/
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
