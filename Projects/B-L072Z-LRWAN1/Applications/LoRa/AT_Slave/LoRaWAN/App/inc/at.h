/**
  ******************************************************************************
  * @file    at.h
  * @author  MCD Application Team
  * @brief   Header for driver at.c module
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
#ifndef __AT_H__
#define __AT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "util_console.h"


/* Exported types ------------------------------------------------------------*/
/*
 * AT Command Id errors. Note that they are in sync with ATError_description static array
 * in command.c
 */
typedef enum eATEerror
{
  AT_OK = 0,
  AT_ERROR,
  AT_PARAM_ERROR,
  AT_BUSY_ERROR,
  AT_TEST_PARAM_OVERFLOW,
  AT_NO_NET_JOINED,
  AT_RX_ERROR,
  AT_NO_CLASS_B_ENABLE,
  AT_MAX,
} ATEerror_t;

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* AT printf */
#define AT_PRINTF     PRINTF

/* AT Command strings. Commands start with AT */
#define AT_RESET      "Z"
#define AT_BAND       "+BAND"
#define AT_DEUI       "+DEUI"
#define AT_DADDR      "+DADDR"
#define AT_APPKEY     "+APPKEY"
#define AT_NWKSKEY    "+NWKSKEY"
#define AT_FNWKSKEY    "+FNWKSKEY"
#define AT_SNWKSKEY    "+SNWKSKEY"
#define AT_APPSKEY    "+APPSKEY"
#define AT_JOINEUI     "+APPEUI" /*to match with V1.0.x specification- For V1.1.x "+APPEUI" will be replaced by "+JOINEUI"*/
#define AT_ADR        "+ADR"
#define AT_TXP        "+TXP"
#define AT_DR         "+DR"
#define AT_DCS        "+DCS"
#define AT_PNM        "+PNM"
#define AT_RX2FQ      "+RX2FQ"
#define AT_RX2DR      "+RX2DR"
#define AT_RX1DL      "+RX1DL"
#define AT_RX2DL      "+RX2DL"
#define AT_JN1DL      "+JN1DL"
#define AT_JN2DL      "+JN2DL"
#define AT_NJM        "+NJM"
#define AT_NWKID      "+NWKID"
#define AT_CLASS      "+CLASS"
#define AT_JOIN       "+JOIN"
#define AT_NJS        "+NJS"
#define AT_SENDB      "+SENDB"
#define AT_SEND       "+SEND"
#define AT_RECVB      "+RECVB"
#define AT_RECV       "+RECV"
#define AT_VER        "+VER"
#define AT_CFM        "+CFM"
#define AT_CFS        "+CFS"
#define AT_SNR        "+SNR"
#define AT_RSSI       "+RSSI"
#define AT_BAT        "+BAT"
#define AT_TRSSI      "+TRSSI"
#define AT_TTONE      "+TTONE"
#define AT_TTLRA      "+TTLRA"
#define AT_TRLRA      "+TRLRA"
#define AT_TCONF      "+TCONF"
#define AT_TOFF       "+TOFF"
#define AT_CERTIF     "+CERTIF"
#define AT_PGSLOT     "+PGSLOT"
#define AT_BFREQ      "+BFREQ"
#define AT_BTIME      "+BTIME"
#define AT_BGW        "+BGW"
#define AT_LTIME      "+LTIME"
#define AT_CHANMASK   "+CHANMASK"
#define AT_CHANDEFMASK "+CHANDEFMASK"

/* Exported functions ------------------------------------------------------- */

/**
 * @brief  Store the received data
 * @param  Application port
 * @param  Buffer of the received data
 * @param  Size of the received data
 * @retval None
 */
void set_at_receive(uint8_t AppPort, uint8_t *Buff, uint8_t BuffSize);

/**
 * @brief  Return AT_OK in all cases
 * @param  Param string of the AT command - unused
 * @retval AT_OK
 */
ATEerror_t at_return_ok(const char *param);

/**
 * @brief  Return AT_ERROR in all cases
 * @param  Param string of the AT command - unused
 * @retval AT_ERROR
 */
ATEerror_t at_return_error(const char *param);

/**
 * @brief  Trig a reset of the MCU
 * @param  Param string of the AT command - unused
 * @retval AT_OK
 */
ATEerror_t at_reset(const char *param);

/**
 * @brief  Print RF Band in use
 * @param  Param string of the AT command
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_Band_get(const char *param);

/**
 * @brief  Set RF Band in use
 * @param  Param string of the AT command
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_Band_set(const char *param);

/**
 * @brief  Print Device EUI
 * @param  Param string of the AT command - unused
 * @retval AT_OK
 */
ATEerror_t at_DevEUI_get(const char *param);

/**
 * @brief  Print Join Eui
 * @param  Param string of the AT command
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_JoinEUI_get(const char *param);

/**
 * @brief  Set Join Eui
 * @param  Param string of the AT command
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_JoinEUI_set(const char *param);

/**
 * @brief  Set DevAddr
 * @param  String pointing to provided DevAddr
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_DevAddr_set(const char *param);

/**
 * @brief  Print the DevAddr
 * @param  String pointing to provided DevAddr
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_DevAddr_get(const char *param);

/**
 * @brief  Print Application Key
 * @param  Param string of the AT command
 * @retval AT_OK
 */
ATEerror_t at_AppKey_get(const char *param);

/**
 * @brief  Set Application Key
 * @param  Param string of the AT command
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_AppKey_set(const char *param);

/**
 * @brief  Print Network Session Key
 * @param  String pointing to provided DevAddr
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_NwkSKey_get(const char *param);

/**
 * @brief  Set Network Session Key
 * @param  String pointing to provided DevAddr
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_NwkSKey_set(const char *param);

/**
 * @brief  Print Forwarding Network session integrity key
 * @param  String pointing to provided DevAddr
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_FNwkSKey_get(const char *param);

/**
 * @brief  Set Forwarding Network session integrity key
 * @param  String pointing to provided DevAddr
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_FNwkSKey_set(const char *param);

/**
 * @brief  Print Serving Network session integrity key
 * @param  String pointing to provided DevAddr
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_SNwkSKey_get(const char *param);

/**
 * @brief  Set Serving Network session integrity key
 * @param  String pointing to provided DevAddr
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_SNwkSKey_set(const char *param);


/**
 * @brief  Print Application Session Key
 * @param  String pointing to provided DevAddr
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_AppSKey_get(const char *param);

/**
 * @brief  Set Application Session Key
 * @param  String pointing to provided DevAddr
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_AppSKey_set(const char *param);

/**
 * @brief  Print Adaptative Data Rate setting
 * @param  String pointing to provided ADR setting
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_ADR_get(const char *param);

/**
 * @brief  Set Adaptative Data Rate setting
 * @param  String pointing to provided ADR setting
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_ADR_set(const char *param);

/**
 * @brief  Print Transmit Power
 * @param  String pointing to provided power
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_TransmitPower_get(const char *param);

/**
 * @brief  Set Transmit Power
 * @param  String pointing to provided power
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_TransmitPower_set(const char *param);

/**
 * @brief  Print Data Rate
 * @param  String pointing to provided rate
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_DataRate_get(const char *param);

/**
 * @brief  Set Data Rate
 * @param  String pointing to provided rate
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_DataRate_set(const char *param);

/**
 * @brief  Set ETSI Duty Cycle parameter
 * @param  String pointing to provided Duty Cycle value
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_DutyCycle_set(const char *param);

/**
 * @brief  Get ETSI Duty Cycle parameter
 * @param  0 if disable, 1 if enable
 * @retval AT_OK
 */
ATEerror_t at_DutyCycle_get(const char *param);

/**
 * @brief  Print Public Network setting
 * @param  String pointing to provided Network setting
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_PublicNetwork_get(const char *param);

/**
 * @brief  Set Public Network setting
 * @param  String pointing to provided Network setting
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_PublicNetwork_set(const char *param);

/**
 * @brief  Print Rx2 window frequency
 * @param  String pointing to parameter
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_Rx2Frequency_get(const char *param);

/**
 * @brief  Set Rx2 window frequency
 * @param  String pointing to parameter
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_Rx2Frequency_set(const char *param);

/**
 * @brief  Print Rx2 window data rate
 * @param  String pointing to parameter
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_Rx2DataRate_get(const char *param);

/**
 * @brief  Set Rx2 window data rate
 * @param  String pointing to parameter
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_Rx2DataRate_set(const char *param);

/**
 * @brief  Print the delay between the end of the Tx and the Rx Window 1
 * @param  String pointing to provided param
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_Rx1Delay_get(const char *param);

/**
 * @brief  Set the delay between the end of the Tx and the Rx Window 1
 * @param  String pointing to provided param
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_Rx1Delay_set(const char *param);

/**
 * @brief  Print the delay between the end of the Tx and the Rx Window 2
 * @param  String pointing to provided param
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_Rx2Delay_get(const char *param);

/**
 * @brief  Set the delay between the end of the Tx and the Rx Window 2
 * @param  String pointing to provided param
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_Rx2Delay_set(const char *param);

/**
 * @brief  Print the delay between the end of the Tx and the Join Rx Window 1
 * @param  String pointing to provided param
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_JoinAcceptDelay1_get(const char *param);

/**
 * @brief  Set the delay between the end of the Tx and the Join Rx Window 1
 * @param  String pointing to provided param
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_JoinAcceptDelay1_set(const char *param);

/**
 * @brief  Print the delay between the end of the Tx and the Join Rx Window 2
 * @param  String pointing to provided param
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_JoinAcceptDelay2_get(const char *param);

/**
 * @brief  Set the delay between the end of the Tx and the Join Rx Window 2
 * @param  String pointing to provided param
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_JoinAcceptDelay2_set(const char *param);

/**
 * @brief  Print network join mode
 * @param  String pointing to provided param
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_NetworkJoinMode_get(const char *param);

/**
 * @brief  Set network join mode
 * @param  String pointing to provided param: "1" for OTAA, "0" for ABP
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_NetworkJoinMode_set(const char *param);

/**
 * @brief  Print the Network ID
 * @param  String pointing to provided parameter
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_NetworkID_get(const char *param);

/**
 * @brief  Set the Network ID
 * @param  String pointing to provided parameter
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_NetworkID_set(const char *param);

/**
 * @brief  Print the Device Class
 * @param  String pointing to provided param
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_DeviceClass_get(const char *param);

/**
 * @brief  Set the Device Class
 * @param  String pointing to provided param
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_DeviceClass_set(const char *param);

/**
 * @brief  Join a network
 * @param  String parameter
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_Join(const char *param);

/**
 * @brief  Print the join status
 * @param  String parameter
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_NetworkJoinStatus(const char *param);

/**
 * @brief  Send a message
 * @param  String parameter of hexadecimal value
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_SendBinary(const char *param);

/**
 * @brief  Print last received message
 * @param  String parameter
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_Send(const char *param);

/**
 * @brief  Print last received data in binary format with hexadecimal value
 * @param  String parameter
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_ReceiveBinary(const char *param);

/**
 * @brief  Print last received data
 * @param  String parameter
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_Receive(const char *param);

/**
 * @brief  Print the version of the AT_Slave FW
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_version_get(const char *param);

/**
 * @brief  Set if message acknowledgment is required (1) or not (0)
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_ack_set(const char *param);

/**
 * @brief  Get if message acknowledgment is required (1) or not (0)
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_ack_get(const char *param);

/**
 * @brief  Get if the last message has been acknowledged or not
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_isack_get(const char *param);

/**
 * @brief  Get the SNR
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_snr_get(const char *param);

/**
 * @brief  Get the RSSI
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_rssi_get(const char *param);

/**
 * @brief  Get the battery level
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_bat_get(const char *param);

/**
 * @brief  Set the unicast pingslot periodicity
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_PingSlot_set(const char *param);

/**
 * @brief  Get the unicast pingslot periodicity, Data rate and PsFrequency
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_PingSlot_get(const char *param);

/**
* @brief  Get the Beacon Frequency
* @param  String parameter
* @retval AT_OK
*/
ATEerror_t at_BeaconFreq_get(const char *param);

/**
* @brief  Get the Beacon Time
* @param  String parameter
* @retval AT_OK
*/
ATEerror_t at_BeaconTime_get(const char *param);

/**
* @brief  Get the Beacon GW specific parameters
* @param  String parameter
* @retval AT_OK
*/
ATEerror_t at_BeaconGatewayCoordinate_get(const char *param);

/**
* @brief  Get the local time in UTC format
* @param  String parameter
* @retval AT_OK
*/

ATEerror_t at_LocalTime_get(const char *param);
/**
 * @brief  Sart Tx test
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_test_txTone(const char *param);

/**
 * @brief  Sart Rx tone
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_test_rxTone(const char *param);

/**
 * @brief  stop Rx or Tx test
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_test_stop(const char *param);

/**
 * @brief  Start Tx LoRa test
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_test_txlora(const char *param);

/**
 * @brief  Start Rx LoRa test
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_test_rxlora(const char *param);

/**
 * @brief  Set Rx or Tx test config
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_test_set_lora_config(const char *param);

/**
 * @brief  Get Rx or Tx test config
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_test_get_lora_config(const char *param);

/**
 * @brief  set the Modem in Certif Mode
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t at_Certif(const char *param);

/**
 * @brief  Gets the current region's channel mask, note this is reset when changing regions
 * @param  String parameter
 * @retval AT_OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_ChannelMask_get(const char *param);

/**
 * @brief  Sets the current region's channel mask, note this is reset when changing regions
 * @param  String parameter
 * @retval AT_OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_ChannelMask_set(const char *param);

/**
 * @brief  Gets the current region's default mask, note this is reset when changing regions
 * @param  String parameter
 * @retval AT_OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_ChannelDefaultMask_get(const char *param);

/**
 * @brief  Sets the current region's channel mask, note this is reset when changing regions
 * @param  String parameter
 * @retval AT_OK, or an appropriate AT_xxx error code
 */
ATEerror_t at_ChannelDefaultMask_set(const char *param);

#ifdef __cplusplus
}
#endif

#endif /* __AT_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
