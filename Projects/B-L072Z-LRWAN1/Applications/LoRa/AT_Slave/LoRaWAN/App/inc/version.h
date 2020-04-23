/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: LoRaMac classA device implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis, Gregory Cristian and Wael Guibene
*/
/**
  ******************************************************************************
  * @file    version.h
  * @author  MCD Application Team
  * @brief   defines the lora mac version
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
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

#ifndef __VERSION_H__
#define __VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "lora_mac_version.h"
/* Exported constants --------------------------------------------------------*/
#define __APP_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define __APP_VERSION_SUB1   (0x03U) /*!< [23:16] sub1 version */
#define __APP_VERSION_SUB2   (0x00U) /*!< [15:8]  sub2 version */
#define __APP_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */
#define __APP_VERSION        ((__APP_VERSION_MAIN <<24)\
                                             |(__APP_VERSION_SUB1 << 16)\
                                             |(__APP_VERSION_SUB2 << 8 )\
                                             |(__APP_VERSION_RC))

#define __MAC_VERSION   (uint32_t) (LORA_MAC_VERSION  )

/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /*__VERSION_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
