/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Bleeper board GPIO driver implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
/**
  ******************************************************************************
  * @file    mlm32l0xx_it.h
  * @author  MCD Application Team
  * @brief   manages interupt
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
#ifndef __32L082MLM_IT_H__
#define __32L082MLM_IT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
 * @brief  RTC IRQ Handler on the RTC Alarm
 * @param  None
 * @retval None
 */
void RTC_IRQHandler(void);

/**
 * @brief  Handles External lines 0 to 1 interrupt request.
 * @param  None
 * @retval None
 */
void EXTI0_1_IRQHandler(void);

/**
 * @brief  Handles External lines 2 to 3 interrupt request.
 * @param  None
 * @retval None
 */
void EXTI2_3_IRQHandler(void);

/**
 * @brief  Handles External lines 4 to 15 interrupt request.
 * @param  None
 * @retval None
 */
void EXTI4_15_IRQHandler(void);

/**
 * @brief  vcom IRQ Handler
 * @param  None
 * @retval None
 */
void UARTX_IRQHandler(void);


#ifdef __cplusplus
}
#endif

#endif /* __32L082MLM_IT_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
