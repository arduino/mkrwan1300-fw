/**
  ******************************************************************************
  * @file    se_fwimg.h
  * @author  MCD Application Team
  * @brief   This file provides set of firmware functions to manage Secure Firmware
  *          Update functionalities.
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
#ifndef SE_FWIMG_H
#define SE_FWIMG_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SE
  * @{
  */

/** @addtogroup SE_CORE
  * @{
  */

/** @addtogroup SE_IMG
  * @{
  */

/** @addtogroup SE_IMG_Exported_Functions
  * @{
  */
SE_ErrorStatus SE_IMG_Erase(void *pDestination, uint32_t Length);
SE_ErrorStatus SE_IMG_Read(void *pDestination, const void *pSource, uint32_t Length);
SE_ErrorStatus SE_IMG_Write(void *pDestination, const void *pSource, uint32_t Length);

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

#endif /* SE_FWIMG_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
