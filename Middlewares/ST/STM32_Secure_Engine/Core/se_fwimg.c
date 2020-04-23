/**
  ******************************************************************************
  * @file    se_fwimg.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions used by the bootloader
  *          to manage Secure Firmware Update functionalities.
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
#include "se_def.h"
#include "se_fwimg.h"
#include "se_low_level.h"
#include "se_exception.h"
#include "string.h"

/** @addtogroup SE
  * @{
  */

/** @addtogroup SE_CORE SE Core
  * @{
  */

/** @defgroup SE_IMG SE Firmware Image
  * @brief Code used to handle the Firmware Images.
  * This contains functions used by the bootloader.
  * @{
  */


/** @defgroup SE_IMG_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  Write in Flash protected area
  * @param  pDestination pointer to destination area in Flash
  * @param  pSource pointer to input buffer
  * @param  Length number of bytes to be written
  * @retval SE_SUCCESS if successful, otherwise SE_ERROR
  */

SE_ErrorStatus SE_IMG_Write(void *pDestination, const void *pSource, uint32_t Length)
{
  SE_ErrorStatus ret;
  uint32_t areabegin = (uint32_t)pDestination;
  uint32_t areaend = areabegin + Length - 1U;

  /* The header of slot #0 is placed in protected area */
  if ((areabegin >= SFU_IMG_SLOT_0_REGION_BEGIN_VALUE) &&
      (areaend < (SFU_IMG_SLOT_0_REGION_BEGIN_VALUE + SFU_IMG_IMAGE_OFFSET)))
  {
    /* Writing in protected memory */
    ret = SE_LL_FLASH_Write(pDestination, pSource, Length);
  }
  else
  {
    /* Abnormal case: this primitive should not be used to access this address */
    ret = SE_ERROR;
  }
  return ret;
}

/**
  * @brief  Read from Flash protected area
  * @param  pDestination pointer to output buffer
  * @param  pSource pointer to source area in Flash
  * @param  Length number of bytes to be read
  * @retval SE_SUCCESS if successful, otherwise SE_ERROR
  */

SE_ErrorStatus SE_IMG_Read(void *pDestination, const void *pSource, uint32_t Length)
{
  SE_ErrorStatus ret;
  uint32_t areabegin = (uint32_t)pSource;
  uint32_t areaend = areabegin + Length - 1U;
  /* The header of slot #0 is placed in protected area */
  if ((areabegin >= SFU_IMG_SLOT_0_REGION_BEGIN_VALUE) &&
      (areaend < (SFU_IMG_SLOT_0_REGION_BEGIN_VALUE + SFU_IMG_IMAGE_OFFSET)))
  {
    /* Accessing protected memory */
    ret = SE_LL_FLASH_Read(pDestination, pSource, Length);
  }
  else
  {
    /* Abnormal case: this primitive should not be used to access this address */
    ret = SE_ERROR;
  }
  return ret;
}

/**
  * @brief  Erase a Flash protected area
  * @param  pDestination pointer to destination area in Flash
  * @param  Length number of bytes to be erased
  * @retval SE_SUCCESS if successful, otherwise SE_ERROR
  */

SE_ErrorStatus SE_IMG_Erase(void *pDestination, uint32_t Length)
{
  SE_ErrorStatus ret;
  uint32_t areabegin = (uint32_t)pDestination;
  uint32_t areaend = areabegin + Length - 1U;
  /* The header of slot #0 is placed in protected area */
  if ((areabegin >= SFU_IMG_SLOT_0_REGION_BEGIN_VALUE) &&
      (areaend < (SFU_IMG_SLOT_0_REGION_BEGIN_VALUE + SFU_IMG_IMAGE_OFFSET)))
  {
    /* Accessing protected memory */
    ret = SE_LL_FLASH_Erase(pDestination, Length);
  }
  else
  {
    /* Abnormal case: this primitive should not be used to access this address */
    ret = SE_ERROR;
  }
  return ret;
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


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
