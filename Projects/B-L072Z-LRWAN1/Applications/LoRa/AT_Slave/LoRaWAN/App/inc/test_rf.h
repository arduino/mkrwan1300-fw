/**
  ******************************************************************************
  * @file    test_rf.h
  * @author  MCD Application Team
  * @brief   Header for test_rf.c
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
#ifndef __TEST_RF_H__
#define __TEST_RF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
* @brief RF transmit test command
* @param [IN] buf Pointer to buffer of data
* @param [IN] size Size of data
* @retval LoRa status
*/
ATEerror_t TST_TxTone(const char *buf, unsigned bufSize);

/**
 * @brief RF Receive test command
 * @param [IN] buf Pointer to buffer of data
 * @param [IN] size Size of data
 * @retval LoRa status
 */
ATEerror_t TST_RxTone(const char *buf, unsigned bufSize);

ATEerror_t TST_TX_LoraStart(const char *buf, unsigned bufSize);

ATEerror_t TST_RX_LoraStart(const char *buf, unsigned bufSize);

ATEerror_t TST_SET_lora_config(const char *buf, unsigned bufSize);

ATEerror_t TST_get_lora_config(const char *buf, unsigned bufSize);
/**
 * @brief RF test stop
 * @param [IN] None
 * @retval LoRa status
 */
ATEerror_t TST_stop(void);
#ifdef __cplusplus
}
#endif

#endif /* __TEST_RF_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
