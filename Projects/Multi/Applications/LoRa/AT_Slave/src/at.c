/*******************************************************************************
 * @file    at.c
 * @author  MCD Application Team
 * @version V1.1.2
 * @date    08-September-2017
 * @brief   at command API
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "at.h"
#include "lora.h"
#include "Commissioning.h"
#include "utilities.h"
#include "LoRaMac.h"
#include "LoRaMacTest.h"
#include "radio.h"
#include "vcom.h"
#include "tiny_sscanf.h"
#include "version.h"
#include "hw_msp.h"
#include "test_rf.h"

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/**
 * @brief Max size of the data that can be received
 */
#define MAX_RECEIVED_DATA 255

/* Private macro -------------------------------------------------------------*/
/**
 * @brief Macro to return when an error occurs
 */
#define CHECK_STATUS(status) do {                    \
    ATEerror_t at_status = translate_status(status); \
    if (at_status != AT_OK) { return at_status; }    \
  } while (0)

/* Private variables ---------------------------------------------------------*/
/**
 * @brief Buffer that contains the last received data
 */
static char ReceivedData[MAX_RECEIVED_DATA];

/**
 * @brief Size if the buffer that contains the last received data
 */
static unsigned ReceivedDataSize = 0;

/**
 * @brief Application port the last received data were on
 */
static uint8_t ReceivedDataPort;

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Translate a LoRaMacStatus_t into an ATEerror_t
 * @param  the LoRaMacStatus_t status
 * @retval the corresponding ATEerror_t code
 */
static ATEerror_t translate_status(LoRaMacStatus_t status);

/**
 * @brief  Get 16 bytes values in hexa
 * @param  The string containing the 16 bytes, something like ab:cd:01:...
 * @param  The buffer that will contain the bytes read
 * @retval The number of bytes read
 */
static int sscanf_16_hhx(const char *from, uint8_t *pt);

/**
 * @brief  Print 16 bytes as %02x
 * @param  the pointer containing the 16 bytes to print
 * @retval None
 */
static void print_16_02x(uint8_t *pt);

/**
 * @brief  Get 4 bytes values in hexa
 * @param  The string containing the 16 bytes, something like ab:cd:01:21
 * @param  The buffer that will contain the bytes read
 * @retval The number of bytes read
 */
static int sscanf_uint32_as_hhx(const char *from, uint32_t *value);

/**
 * @brief  Print 4 bytes as %02x
 * @param  the value containing the 4 bytes to print
 * @retval None
 */
static void print_uint32_as_02x(uint32_t value);

/**
 * @brief  Print 8 bytes as %02x
 * @param  the pointer containing the 8 bytes to print
 * @retval None
 */
static void print_8_02x(uint8_t *pt);

/**
 * @brief  Print an int
 * @param  the value to print
 * @retval None
 */
static void print_d(int value);

/**
 * @brief  Print an unsigned int
 * @param  the value to print
 * @retval None
 */
static void print_u(unsigned int value);

/* Exported functions ------------------------------------------------------- */

void set_at_receive(uint8_t AppPort, uint8_t* Buff, uint8_t BuffSize)
{
  if (MAX_RECEIVED_DATA <= BuffSize)
    BuffSize = MAX_RECEIVED_DATA;
  memcpy1((uint8_t *)ReceivedData, Buff, BuffSize);
  ReceivedDataSize = BuffSize;
  ReceivedDataPort = AppPort;
}

ATEerror_t at_return_ok(const char *param)
{
  return AT_OK;
}

ATEerror_t at_return_error(const char *param)
{
  return AT_ERROR;
}

ATEerror_t at_reset(const char *param)
{
  NVIC_SystemReset();
  return AT_OK;
}

ATEerror_t at_DevEUI_get(const char *param)
{
  AT_PRINTF("+OK=");
  print_8_02x(lora_config_deveui_get());
  return AT_OK;
}

ATEerror_t at_AppEUI_get(const char *param)
{
  AT_PRINTF("+OK=");
  print_8_02x(lora_config_appeui_get());
  return AT_OK;
}

ATEerror_t at_AppEUI_set(const char *param)
{
  uint8_t AppEui[8];
  if (tiny_sscanf(param, "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
                  &AppEui[0], &AppEui[1], &AppEui[2], &AppEui[3],
                  &AppEui[4], &AppEui[5], &AppEui[6], &AppEui[7]) != 8)
  {
    return AT_PARAM_ERROR;
  }

  lora_config_appeui_set(AppEui);
  return AT_OK;
}

ATEerror_t at_DevAddr_set(const char *param)
{
	uint32_t DevAddr;
	sscanf_uint32_as_hhx(param, &DevAddr);

	lora_config_devaddr_set(DevAddr);
	return AT_OK;
}

ATEerror_t at_DevAddr_get(const char *param)
{

  AT_PRINTF("+OK=");
  print_uint32_as_02x(lora_config_devaddr_get());
  return AT_OK;
}

ATEerror_t at_NetworkID_get(const char *param)
{
  AT_PRINTF("+OK=");
  print_uint32_as_02x(lora_config_networkid_get());
  return AT_OK;
}

ATEerror_t at_NetworkID_set(const char *param)
{
  uint32_t NetworkID;
  sscanf_uint32_as_hhx(param, &NetworkID);

  lora_config_networkid_set(NetworkID);
  return AT_OK;
}

ATEerror_t at_AppKey_get(const char *param)
{
  AT_PRINTF("+OK=");
  print_16_02x(lora_config_appkey_get());
  return AT_OK;
}

ATEerror_t at_AppKey_set(const char *param)
{
  uint8_t AppKey[16];
  int ret = sscanf_16_hhx(param, AppKey);
  if (ret != 16)
  {
    return AT_PARAM_ERROR;
  }
  
  lora_config_appkey_set(AppKey);
  return AT_OK;
}

extern LoRaMacRegion_t globalRegion;
ATEerror_t at_Band_get(const char *param)
{
  AT_PRINTF("+OK=");
  print_d(globalRegion);
  return AT_OK;
}

ATEerror_t at_Band_set(const char *param)
{
  LoRaMacRegion_t region;
  if (tiny_sscanf(param, "%hhu", &region)  != 1)
  {
    return AT_PARAM_ERROR;
  }
  if (region != globalRegion) {
	  globalRegion = region;
	  TriggerReinit();
  }
  return AT_OK;
}

ATEerror_t at_NwkSKey_get(const char *param)
{
  AT_PRINTF("+OK=");
  print_16_02x(lora_config_nwkskey_get());
  return AT_OK;
}

ATEerror_t at_NwkSKey_set(const char *param)
{
  uint8_t NwkSkey[16];
  int ret = sscanf_16_hhx(param, NwkSkey);
  if (ret != 16)
  {
    return AT_PARAM_ERROR;
  }

  lora_config_nwkskey_set(NwkSkey);
  return AT_OK;
}

ATEerror_t at_AppSKey_get(const char *param)
{
  AT_PRINTF("+OK=");
  print_16_02x(lora_config_appskey_get());
  return AT_OK;
}

ATEerror_t at_AppSKey_set(const char *param)
{
  uint8_t AppSKey[16];
  int ret = sscanf_16_hhx(param, AppSKey);
  if (ret != 16)
  {
    return AT_PARAM_ERROR;
  }

  lora_config_appskey_set(AppSKey);
  return AT_OK;
}

ATEerror_t at_Certif( const char *param )
{
  lora_wan_certif( );
  return AT_OK;
}

ATEerror_t at_ADR_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_ADR;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  AT_PRINTF("+OK=");
  print_d(mib.Param.AdrEnable);

  return AT_OK;
}

ATEerror_t at_ADR_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_ADR;

  switch (param[0])
  {
    case '0':
    case '1':
      mib.Param.AdrEnable = param[0] - '0';
      status = LoRaMacMibSetRequestConfirm(&mib);
      CHECK_STATUS(status);
      break;
    default:
      return AT_PARAM_ERROR;
  }

  return AT_OK;
}

ATEerror_t at_TransmitPower_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_CHANNELS_TX_POWER;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  AT_PRINTF("+OK=");
  print_d(mib.Param.ChannelsTxPower);

  return AT_OK;
}

ATEerror_t at_TransmitPower_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;
  uint8_t ignored;

  mib.Type = MIB_CHANNELS_TX_POWER;
  if (tiny_sscanf(param, "%hhu,%hhu", &ignored, &mib.Param.ChannelsTxPower) != 1)
  {
    return AT_PARAM_ERROR;
  }
  status = LoRaMacMibSetRequestConfirm(&mib);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_DataRate_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_CHANNELS_DATARATE;

  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  AT_PRINTF("+OK=");
  print_d(mib.Param.ChannelsDatarate);

  return AT_OK;
}

ATEerror_t at_DataRate_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_CHANNELS_DATARATE;
  if (tiny_sscanf(param, "%hhu", &mib.Param.ChannelsDatarate) != 1)
  {
    return AT_PARAM_ERROR;
  }
  status = LoRaMacMibSetRequestConfirm(&mib);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_DutyCycle_set(const char *param)
{
  switch (param[0])
  {
    case '0':
      lora_config_duty_cycle_set(DISABLE);
      break;
    case '1':
      lora_config_duty_cycle_set(ENABLE);
      break;
    default:
      return AT_PARAM_ERROR;
  }

  return AT_OK;
}

ATEerror_t at_DutyCycle_get(const char *param)
{
  AT_PRINTF("+OK=");
  if (lora_config_duty_cycle_get() == ENABLE)
    AT_PRINTF("1\r");
  else
    AT_PRINTF("0\r");

  return AT_OK;
}


ATEerror_t at_PublicNetwork_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_PUBLIC_NETWORK;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  AT_PRINTF("+OK=");
  print_d(mib.Param.EnablePublicNetwork);

  return AT_OK;
}

ATEerror_t at_PublicNetwork_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_PUBLIC_NETWORK;
  switch (param[0])
  {
    case '0':
    case '1':
      mib.Param.EnablePublicNetwork = param[0] - '0';
      status = LoRaMacMibSetRequestConfirm(&mib);
      CHECK_STATUS(status);
      break;
    default:
      return AT_PARAM_ERROR;
  }

  return AT_OK;
}

ATEerror_t at_Rx2Frequency_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_RX2_CHANNEL;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  AT_PRINTF("+OK=");
  print_d(mib.Param.Rx2Channel.Frequency);

  return AT_OK;
}

ATEerror_t at_Rx2Frequency_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_RX2_CHANNEL;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);

  if (tiny_sscanf(param, "%lu", &mib.Param.Rx2Channel.Frequency) != 1)
  {
    return AT_PARAM_ERROR;
  }

  status = LoRaMacMibSetRequestConfirm(&mib);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_Rx2DataRate_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_RX2_CHANNEL;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  AT_PRINTF("+OK=");
  print_d(mib.Param.Rx2Channel.Datarate);

  return AT_OK;
}

ATEerror_t at_Rx2DataRate_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_RX2_CHANNEL;

  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);

  if (tiny_sscanf(param, "%hhu", &mib.Param.Rx2Channel.Datarate) != 1)
  {
    return AT_PARAM_ERROR;
  }

  status = LoRaMacMibSetRequestConfirm(&mib);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_Rx1Delay_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_RECEIVE_DELAY_1;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  AT_PRINTF("+OK=");
  print_u(mib.Param.ReceiveDelay1);

  return AT_OK;
}

ATEerror_t at_Rx1Delay_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_RECEIVE_DELAY_1;
  if (tiny_sscanf(param, "%lu", &mib.Param.ReceiveDelay1) != 1)
  {
    return AT_PARAM_ERROR;
  }
  status = LoRaMacMibSetRequestConfirm(&mib);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_Rx2Delay_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_RECEIVE_DELAY_2;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  print_u(mib.Param.ReceiveDelay2);

  return AT_OK;
}

ATEerror_t at_Rx2Delay_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_RECEIVE_DELAY_2;
  if (tiny_sscanf(param, "%lu", &mib.Param.ReceiveDelay2) != 1)
  {
    return AT_PARAM_ERROR;
  }
  status = LoRaMacMibSetRequestConfirm(&mib);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_JoinAcceptDelay1_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_JOIN_ACCEPT_DELAY_1;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  AT_PRINTF("+OK=");
  print_u(mib.Param.JoinAcceptDelay1);

  return AT_OK;
}

ATEerror_t at_JoinAcceptDelay1_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_JOIN_ACCEPT_DELAY_1;
  if (tiny_sscanf(param, "%lu", &mib.Param.JoinAcceptDelay1) != 1)
  {
    return AT_PARAM_ERROR;
  }
  status = LoRaMacMibSetRequestConfirm(&mib);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_JoinAcceptDelay2_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_JOIN_ACCEPT_DELAY_2;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  AT_PRINTF("+OK=");
  print_u(mib.Param.JoinAcceptDelay2);

  return AT_OK;
}

ATEerror_t at_JoinAcceptDelay2_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_JOIN_ACCEPT_DELAY_2;
  if (tiny_sscanf(param, "%lu", &mib.Param.JoinAcceptDelay2) != 1)
  {
    return AT_PARAM_ERROR;
  }
  status = LoRaMacMibSetRequestConfirm(&mib);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_NetworkJoinMode_get(const char *param)
{
  AT_PRINTF("+OK=");
  print_d((lora_config_otaa_get() == ENABLE ? 1 : 0));
  return AT_OK;
}

ATEerror_t at_NetworkJoinMode_set(const char *param)
{
  FunctionalState status;

  switch (param[0])
  {
    case '0':
      status = DISABLE;
      break;
    case '1':
      status = ENABLE;
      break;
    default:
      return AT_PARAM_ERROR;
  }

  lora_config_otaa_set(status);
  return AT_OK;
}

ATEerror_t at_UplinkCounter_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_UPLINK_COUNTER;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  AT_PRINTF("+OK=");
  print_u(mib.Param.UpLinkCounter);

  return AT_OK;
}

ATEerror_t at_UplinkCounter_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_UPLINK_COUNTER;
  if (tiny_sscanf(param, "%lu", &mib.Param.UpLinkCounter) != 1)
  {
    return AT_PARAM_ERROR;
  }
  status = LoRaMacMibSetRequestConfirm(&mib);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_DownlinkCounter_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_DOWNLINK_COUNTER;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  AT_PRINTF("+OK=");
  print_u(mib.Param.DownLinkCounter);

  return AT_OK;
}

ATEerror_t at_DownlinkCounter_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_DOWNLINK_COUNTER;
  if (tiny_sscanf(param, "%lu", &mib.Param.DownLinkCounter) != 1)
  {
    return AT_PARAM_ERROR;
  }
  status = LoRaMacMibSetRequestConfirm(&mib);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_DeviceClass_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_DEVICE_CLASS;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  AT_PRINTF("+OK=%c\r", 'A' + mib.Param.Class);

  return AT_OK;
}

ATEerror_t at_DeviceClass_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_DEVICE_CLASS;
  switch (param[0])
  {
    case 'A':
    case 'B':
    case 'C':
      /* assume CLASS_A == 0, CLASS_B == 1, etc, which is the case for now */
      mib.Param.Class = (DeviceClass_t)(param[0] - 'A');
      status = LoRaMacMibSetRequestConfirm(&mib);
      CHECK_STATUS(status);
      break;
    default:
      return AT_PARAM_ERROR;
  }

  return AT_OK;
}

ATEerror_t at_Join(const char *param)
{
  LoRaMacStatus_t status;
  status = lora_join();
  CHECK_STATUS(status);
  return AT_OK;
}

ATEerror_t at_NetworkJoinStatus(const char *param)
{
  MibRequestConfirm_t mibReq;
  LoRaMacStatus_t status;

  mibReq.Type = MIB_NETWORK_JOINED;
  status = LoRaMacMibGetRequestConfirm(&mibReq);

  if (status == LORAMAC_STATUS_OK)
  {
	AT_PRINTF("+OK=");
    print_d(mibReq.Param.IsNetworkJoined ? 1 : 0);
    return AT_OK;
  }
  return AT_ERROR;
}

ATEerror_t at_SendBinary(const char *param)
{
  LoRaMacStatus_t status;

  status = lora_send(param, strlen(param), 1, 0);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_Send(const char *param)
{
  LoRaMacStatus_t status;

  status = lora_send(param, strlen(param), 0, 0);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_ReceiveBinary(const char *param)
{
  unsigned i;

  AT_PRINTF("+RECV=");
  AT_PRINTF("%d,%d\r\n\n", ReceivedDataPort, ReceivedDataSize);

  for (i = 0; i < ReceivedDataSize; i++)
  {
    AT_PRINTF("%02x", ReceivedData[i]);
  }
  AT_PRINTF("\r");
  ReceivedDataSize = 0;

  return AT_OK;
}

// Defaults to binary
#define USE_BINARY 	1
#define USE_HEX		0
static uint8_t format_send_v2 = USE_HEX;

ATEerror_t at_Receive(const char *param)
{

  AT_PRINTF("+RECV=");

  if (format_send_v2==0)
  {
	  AT_PRINTF("%d,%d\r\n\n", ReceivedDataPort, ReceivedDataSize);
	  if (ReceivedDataSize)
	  {
		AT_PRINTF("%s", ReceivedData);
	  }
  }
  else
  {
	  AT_PRINTF("%d,%d\r\n\n", ReceivedDataPort, ReceivedDataSize*2);
	  for (unsigned i = 0; i < ReceivedDataSize; i++)
	  {
	    AT_PRINTF("%02x", ReceivedData[i]);
	  }
  }
  ReceivedDataSize = 0;
  AT_PRINTF("\r");

  return AT_OK;
}

ATEerror_t at_SendV2(const char *param)
{
  LoRaMacStatus_t status;
  at_ack_set("0");

  size_t length = 0;

  if (tiny_sscanf(param, "%hhu", &length) != 1)
  {
    return AT_PARAM_ERROR;
  }
  uint8_t data[64];
  int i = 0;
  // grab other #len bytes from the serial buffer
  while (i<length) {
	if (IsNewCharReceived() == SET) {
		data[i] = GetNewChar();
		i++;
    }
  }

  status = lora_send(data, length, format_send_v2, 1);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_SendV2Confirmation(const char *param)
{
  LoRaMacStatus_t status;
  at_ack_set("1");

  size_t length = 0;

  if (tiny_sscanf(param, "%hhu", &length) != 1)
  {
	return AT_PARAM_ERROR;
  }
  uint8_t data[64];
  int i = 0;
  // grab other #len bytes from the serial buffer
  while (i<length) {
	if (IsNewCharReceived() == SET) {
		data[i] = GetNewChar();
		i++;
    }
  }

  status = lora_send(data, length, format_send_v2, 1);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_Port_get(const char *param)
{
  AT_PRINTF("+OK=");
  print_u(lora_config_application_port_get());

  return AT_OK;
}

ATEerror_t at_Port_set(const char *param)
{
  uint8_t application_port;
  if (tiny_sscanf(param, "%hhu", &application_port) != 1)
  {
	return AT_PARAM_ERROR;
  }
  /* set the application port to send to */
  lora_config_application_port_set(application_port);

  return AT_OK;
}

ATEerror_t at_Format_get(const char *param)
{
  AT_PRINTF("+OK=");
  print_u(format_send_v2);

  return AT_OK;
}

ATEerror_t at_Format_set(const char *param)
{
  if (tiny_sscanf(param, "%hhu", &format_send_v2) != 1)
  {
	return AT_PARAM_ERROR;
  }

  return AT_OK;
}

ATEerror_t at_version_get(const char *param)
{
  AT_PRINTF("+OK=");
  AT_PRINTF(AT_VERSION_STRING"\r");
  return AT_OK;
}

ATEerror_t at_device_get(const char *param)
{
  AT_PRINTF("+OK=");
  AT_PRINTF(AT_DEVICE_STRING"\r");
  return AT_OK;
}

ATEerror_t at_ack_set(const char *param)
{
  switch (param[0])
  {
    case '0':
      lora_config_reqack_set(DISABLE);
      break;
    case '1':
      lora_config_reqack_set(ENABLE);
      break;
    default:
      return AT_PARAM_ERROR;
  }

  return AT_OK;
}

ATEerror_t at_ack_get(const char *param)
{
  print_d (((lora_config_reqack_get() == ENABLE) ? 1 : 0));
  return AT_OK;
}

ATEerror_t at_isack_get(const char *param)
{
  print_d(((lora_config_isack_get() == ENABLE) ? 1 : 0));
  return AT_OK;
}

ATEerror_t at_snr_get(const char *param)
{
  print_u(lora_config_snr_get());
  return AT_OK;
}

ATEerror_t at_rssi_get(const char *param)
{
  print_d(lora_config_rssi_get());
  return AT_OK;
}

ATEerror_t at_bat_get(const char *param)
{
  print_u(HW_GetBatteryLevel());
  return AT_OK;
}

ATEerror_t at_test_txTone(const char *param)
{
  return TST_TxTone(param, strlen(param));
}

ATEerror_t at_test_rxTone(const char *param)
{
  return TST_RxTone(param, strlen(param));
}

ATEerror_t at_test_txlora(const char *param)
{
  TST_TX_LoraStart( param, strlen(param) );
  return AT_OK;
}

ATEerror_t at_test_rxlora(const char *param)
{
  TST_RX_LoraStart( );
  return AT_OK;
}

ATEerror_t at_test_get_lora_config(const char *param)
{
  return TST_get_lora_config( param, strlen(param));
}

ATEerror_t at_test_set_lora_config(const char *param)
{
  return TST_SET_lora_config( param, strlen(param));
}

ATEerror_t at_test_stop(const char *param)
{
  return TST_stop( );
}
/* Private functions ---------------------------------------------------------*/

static ATEerror_t translate_status(LoRaMacStatus_t status)
{
  if (status == LORAMAC_STATUS_BUSY)
  {
    return AT_BUSY_ERROR;
  }
  if (status == LORAMAC_STATUS_PARAMETER_INVALID)
  {
    return AT_PARAM_ERROR;
  }
  if (status == LORAMAC_STATUS_NO_NETWORK_JOINED)
  {
    return AT_NO_NET_JOINED;
  }
  if (status != LORAMAC_STATUS_OK)
  {
    return AT_ERROR;
  }
  return AT_OK;
}

static int sscanf_16_hhx(const char *from, uint8_t *pt)
{
  return tiny_sscanf(from, "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
                     &pt[0], &pt[1], &pt[2], &pt[3], &pt[4], &pt[5], &pt[6],
                     &pt[7], &pt[8], &pt[9], &pt[10], &pt[11], &pt[12], &pt[13],
                     &pt[14], &pt[15]);
}

static void print_16_02x(uint8_t *pt)
{
  AT_PRINTF("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r",
            pt[0], pt[1], pt[2], pt[3],
            pt[4], pt[5], pt[6], pt[7],
            pt[8], pt[9], pt[10], pt[11],
            pt[12], pt[13], pt[14], pt[15]);
}

static int sscanf_uint32_as_hhx(const char *from, uint32_t *value)
{
  return tiny_sscanf(from, "%02hhx%02hhx%02hhx%02hhx",
                     &((unsigned char *)(value))[3],
                     &((unsigned char *)(value))[2],
                     &((unsigned char *)(value))[1],
                     &((unsigned char *)(value))[0]);
}

static void print_uint32_as_02x(uint32_t value)
{
  AT_PRINTF("%02x%02x%02x%02x\r",
            (unsigned)((unsigned char *)(&value))[3],
            (unsigned)((unsigned char *)(&value))[2],
            (unsigned)((unsigned char *)(&value))[1],
            (unsigned)((unsigned char *)(&value))[0]);
}

static void print_8_02x(uint8_t *pt)
{
  AT_PRINTF("%02x%02x%02x%02x%02x%02x%02x%02x\r",
            pt[0], pt[1], pt[2], pt[3], pt[4], pt[5], pt[6], pt[7]);
}

static void print_d(int value)
{
  AT_PRINTF("%d\r", value);
}

static void print_u(unsigned int value)
{
  AT_PRINTF("%u\r", value);
}
