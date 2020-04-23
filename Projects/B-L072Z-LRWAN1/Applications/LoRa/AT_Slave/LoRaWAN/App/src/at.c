/**
  ******************************************************************************
  * @file    at.c
  * @author  MCD Application Team
  * @brief   at command API
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

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "at.h"
#include "utilities.h"
#include "lora.h"
#include "LoRaMacTest.h"
#include "radio.h"
#include "vcom.h"
#include "tiny_sscanf.h"
#include "version.h"
#include "hw_msp.h"
#include "test_rf.h"

#ifdef LORAMAC_CLASSB_ENABLED
#include "LoRaMacClassB.h"


typedef struct sLoRaMacClassBPingSlotNvmCtx
{
  struct sPingSlotCtrlNvm
  {
    /*!
     * Set when the server assigned a ping slot to the node
     */
    uint8_t Assigned         : 1;
    /*!
     * Set when a custom frequency is used
     */
    uint8_t CustomFreq       : 1;
  } Ctrl;
  /*!
   * Number of ping slots
   */
  uint8_t PingNb;
  /*!
   * Period of the ping slots
   */
  uint16_t PingPeriod;
  /*!
   * Reception frequency of the ping slot windows
   */
  uint32_t Frequency;
  /*!
   * Datarate of the ping slot
   */
  int8_t Datarate;
} LoRaMacClassBPingSlotNvmCtx_t;
typedef struct sLoRaMacClassBBeaconNvmCtx
{
  struct sBeaconCtrlNvm
  {
    /*!
     * Set if the node has a custom frequency for beaconing and ping slots
     */
    uint8_t CustomFreq          : 1;
  } Ctrl;
  /*!
   * Beacon reception frequency
   */
  uint32_t Frequency;
} LoRaMacClassBBeaconNvmCtx_t;

typedef struct sLoRaMacClassBNvmCtx
{
  /*!
  * Class B ping slot context
  */
  LoRaMacClassBPingSlotNvmCtx_t PingSlotCtx;
  /*!
  * Class B beacon context
  */
  LoRaMacClassBBeaconNvmCtx_t BeaconCtx;
} LoRaMacClassBNvmCtx_t;

typedef struct sLoRaMacClassBCtx
{
  /*!
  * Class B ping slot context
  */
  PingSlotContext_t PingSlotCtx;
  /*!
  * Class B beacon context
  */
  BeaconContext_t BeaconCtx;
  /*!
  * State of the beaconing mechanism
  */
  BeaconState_t BeaconState;
  /*!
  * State of the ping slot mechanism
  */
  PingSlotState_t PingSlotState;
  /*!
  * State of the multicast slot mechanism
  */
  PingSlotState_t MulticastSlotState;
  /*!
  * Timer for CLASS B beacon acquisition and tracking.
  */
  TimerEvent_t BeaconTimer;
  /*!
  * Timer for CLASS B ping slot timer.
  */
  TimerEvent_t PingSlotTimer;
  /*!
  * Timer for CLASS B multicast ping slot timer.
  */
  TimerEvent_t MulticastSlotTimer;
  /*!
  * Container for the callbacks related to class b.
  */
  LoRaMacClassBCallback_t LoRaMacClassBCallbacks;
  /*!
  * Data structure which holds the parameters which needs to be set
  * in class b operation.
  */
  LoRaMacClassBParams_t LoRaMacClassBParams;
  /*
   * Callback function to notify the upper layer about context change
   */
  LoRaMacClassBNvmEvent LoRaMacClassBNvmEvent;
  /*!
  * Non-volatile module context.
  */
  LoRaMacClassBNvmCtx_t *NvmCtx;
} LoRaMacClassBCtx_t;



/* External variables --------------------------------------------------------*/
extern LoRaMacClassBCtx_t Ctx;
extern uint8_t DefaultPingSlotPeriodicity;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

static uint16_t TabPingNbPinPeriod[8] = {32, 64, 128, 256, 512, 1024, 2018, 4096};

static bool ClassBEnableRequest = false;
#endif

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
/*!
 * User application data buffer size
 */
#define LORAWAN_APP_DATA_BUFF_SIZE                           242

/*!
 * User application data
 */
static uint8_t AppDataBuff[LORAWAN_APP_DATA_BUFF_SIZE];

/*!
 * User application data structure
 */
static lora_AppData_t AppData = { AppDataBuff,  0, 0 };
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

void set_at_receive(uint8_t AppPort, uint8_t *Buff, uint8_t BuffSize)
{
  unsigned i;
  MibRequestConfirm_t mib;

  if (MAX_RECEIVED_DATA <= BuffSize)
  {
    BuffSize = MAX_RECEIVED_DATA;
  }
  memcpy1((uint8_t *)ReceivedData, Buff, BuffSize);
  ReceivedDataSize = BuffSize;
  ReceivedDataPort = AppPort;

  mib.Type = MIB_DEVICE_CLASS;
  LoRaMacMibGetRequestConfirm(&mib);


  if (mib.Param.Class == CLASS_A)
  {
    /*aynchronous notification to the host*/
    AT_PRINTF("+EVT:%d:", ReceivedDataPort);
  }
  else /*We are either in Class C or in Class B*/
  {
#ifdef LORAMAC_CLASSB_ENABLED
    if (Ctx.MulticastSlotState != PINGSLOT_STATE_RX) /*unicast mode*/
    {
      AT_PRINTF("+EVT: UNICAST\r\n");
    }
    else /*(multicast mode*/
    {
      AT_PRINTF("+EVT: MULTICAST\r\n");
    }
#endif /* LORAMAC_CLASSB_ENABLED */
    AT_PRINTF("+EVT:%d:", ReceivedDataPort);
  }

  /* Received data to be copied*/
  for (i = 0; i < ReceivedDataSize; i++)
  {
    AT_PRINTF("%02x", ReceivedData[i]);
  }
  AT_PRINTF("\r\n");

  /* the ReceivedDataSize variable is not reset. Allow to still have access to the received*/
  /* data by the way of either AT+RECVB or AT+RECV ---- ReceivedDataSize = 0;*/


  /*to indicate in which received window we are : either RXC for Class_C or RX3 for Class_B*/
  if (mib.Param.Class == CLASS_C)
  {
    AT_PRINTF("+EVT:RXC, RSSI %d, SNR %d\r\n", lora_config_rssi_get(), lora_config_snr_get());
  }
  else
  {
    AT_PRINTF("+EVT:RX3, RSSI %d, SNR %d\r\n", lora_config_rssi_get(), lora_config_snr_get());
  }
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
  /* return AT_OK; */
}

ATEerror_t at_DevEUI_get(const char *param)
{
  print_8_02x(lora_config_deveui_get());
  return AT_OK;
}

ATEerror_t at_JoinEUI_get(const char *param)
{
  print_8_02x(lora_config_joineui_get());
  return AT_OK;
}

ATEerror_t at_JoinEUI_set(const char *param)
{
  uint8_t JoinEui[8];
  if (tiny_sscanf(param, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                  &JoinEui[0], &JoinEui[1], &JoinEui[2], &JoinEui[3],
                  &JoinEui[4], &JoinEui[5], &JoinEui[6], &JoinEui[7]) != 8)
  {
    return AT_PARAM_ERROR;
  }

  lora_config_joineui_set(JoinEui);
  return AT_OK;
}

ATEerror_t at_DevAddr_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_DEV_ADDR;
  if (sscanf_uint32_as_hhx(param, &mib.Param.DevAddr) != 4)
  {
    return AT_PARAM_ERROR;
  }
  status = LoRaMacMibSetRequestConfirm(&mib);
  CHECK_STATUS(status);
  return AT_OK;
}

ATEerror_t at_DevAddr_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_DEV_ADDR;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  print_uint32_as_02x(mib.Param.DevAddr);
  return AT_OK;
}

ATEerror_t at_AppKey_get(const char *param)
{
  print_16_02x(lora_config_appkey_get());
  return AT_OK;
}

ATEerror_t at_AppKey_set(const char *param)
{
  uint8_t AppKey[16];
  if (sscanf_16_hhx(param, AppKey) != 16)
  {
    return AT_PARAM_ERROR;
  }

  lora_config_appkey_set(AppKey);
  return AT_OK;
}

ATEerror_t at_NwkSKey_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_NWK_S_ENC_KEY;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  print_16_02x(mib.Param.NwkSEncKey);

  return AT_OK;
}

ATEerror_t at_NwkSKey_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;
  uint8_t NwkSKey[16];

  mib.Type = MIB_NWK_S_ENC_KEY;

  if (sscanf_16_hhx(param, NwkSKey) != 16)
  {
    return AT_PARAM_ERROR;
  }

  mib.Param.NwkSEncKey = NwkSKey;
  status = LoRaMacMibSetRequestConfirm(&mib);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_AppSKey_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_APP_S_KEY;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  print_16_02x(mib.Param.AppSKey);

  return AT_OK;
}

ATEerror_t at_AppSKey_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;
  uint8_t AppSKey[16];

  mib.Type = MIB_APP_S_KEY;
  if (sscanf_16_hhx(param, AppSKey) != 16)
  {
    return AT_PARAM_ERROR;
  }
  mib.Param.AppSKey = AppSKey;
  status = LoRaMacMibSetRequestConfirm(&mib);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_Certif(const char *param)
{
  lora_wan_certif();
  return AT_OK;
}

ATEerror_t at_ADR_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_ADR;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
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
  print_d(mib.Param.ChannelsTxPower);

  return AT_OK;
}

ATEerror_t at_TransmitPower_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_CHANNELS_TX_POWER;
  if (tiny_sscanf(param, "%hhu", &mib.Param.ChannelsTxPower) != 1)
  {
    return AT_PARAM_ERROR;
  }
  status = LoRaMacMibSetRequestConfirm(&mib);
  CHECK_STATUS(status);

  return AT_OK;
}

ATEerror_t at_DataRate_get(const char *param)
{

  print_d(lora_config_tx_datarate_get());

  return AT_OK;
}

ATEerror_t at_DataRate_set(const char *param)
{
  int8_t datarate;

  if (tiny_sscanf(param, "%hhu", &datarate) != 1)
  {
    return AT_PARAM_ERROR;
  }
  if ((datarate < 0) || (datarate > 15))
  {
    return AT_PARAM_ERROR;
  }

  lora_config_tx_datarate_set(datarate) ;

  return AT_OK;
}

ATEerror_t at_DutyCycle_set(const char *param)
{
  switch (param[0])
  {
    case '0':
      lora_config_duty_cycle_set(LORA_DISABLE);
      break;
    case '1':
      lora_config_duty_cycle_set(LORA_ENABLE);
      break;
    default:
      return AT_PARAM_ERROR;
  }

  return AT_OK;
}

ATEerror_t at_DutyCycle_get(const char *param)
{
  if (lora_config_duty_cycle_get() == LORA_ENABLE)
  {
    AT_PRINTF("1\r\n");
  }
  else
  {
    AT_PRINTF("0\r\n");
  }

  return AT_OK;
}


ATEerror_t at_PublicNetwork_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_PUBLIC_NETWORK;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
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
  print_d((lora_config_otaa_get() == LORA_ENABLE ? 1 : 0));
  return AT_OK;
}

ATEerror_t at_NetworkJoinMode_set(const char *param)
{
  LoraState_t status;

  switch (param[0])
  {
    case '0':
      status = LORA_DISABLE;
      break;
    case '1':
      status = LORA_ENABLE;
      break;
    default:
      return AT_PARAM_ERROR;
  }

  lora_config_otaa_set(status);
  return AT_OK;
}

ATEerror_t at_NetworkID_get(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_NET_ID;
  status = LoRaMacMibGetRequestConfirm(&mib);
  CHECK_STATUS(status);
  print_uint32_as_02x(mib.Param.NetID);

  return AT_OK;
}

ATEerror_t at_NetworkID_set(const char *param)
{
  MibRequestConfirm_t mib;
  LoRaMacStatus_t status;

  mib.Type = MIB_NET_ID;
  if (sscanf_uint32_as_hhx(param, &mib.Param.NetID) != 4)
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

#ifdef LORAMAC_CLASSB_ENABLED
  if (mib.Param.Class == CLASS_A)
  {
    if (ClassBEnableRequest == false)
    {
      AT_PRINTF("%c\r\n", 'A' + mib.Param.Class);
    }
    else
    {
      if (Ctx.BeaconCtx.Ctrl.AcquisitionPending == 1) /*Beacon_Searching on Class B request*/
      {
        AT_PRINTF("%s\r\n", "B,S0");
      }
      else
      {
        if ((Ctx.BeaconCtx.Ctrl.BeaconAcquired == 1) && Ctx.BeaconCtx.Ctrl.BeaconMode == 1) /*Beacon locked */
        {
          AT_PRINTF("%s\r\n", "B,S1");
        }
        else
        {
          AT_PRINTF("%s\r\n", "B,S2");
        }
      }
    }
  }
  else /* we are now either in Class B enable or Class C enable*/
#endif
  {
    AT_PRINTF("%c\r\n", 'A' + mib.Param.Class);
  }

  return AT_OK;
}

ATEerror_t at_DeviceClass_set(const char *param)
{
  switch (param[0])
  {
    case 'A':
#ifdef LORAMAC_CLASSB_ENABLED

      ClassBEnableRequest = false;

#endif
      LORA_RequestClass(CLASS_A);
      break;
    case 'B':
#ifdef LORAMAC_CLASSB_ENABLED
      ClassBEnableRequest = true;
#endif
      LORA_RequestClass(CLASS_B);  /*Class B AT cmd switch Class B not supported cf.[UM2073]*/
      break;
    case 'C':
      LORA_RequestClass(CLASS_C);
      break;
    default:
      return AT_PARAM_ERROR;
  }
  return AT_OK;
}

#ifdef LORAMAC_CLASSB_ENABLED
ATEerror_t at_PingSlot_get(const char *param)
{
  uint8_t Periodicity;
  GetPhyParams_t getPhy;
  PhyParam_t phyParam;
  LoRaMacRegion_t region;

  for (Periodicity = 0; Periodicity <= 7; Periodicity++)
  {
    if (TabPingNbPinPeriod[Periodicity] == Ctx.NvmCtx->PingSlotCtx.PingPeriod)
    {
      break;
    }
  }

  /*Any PingslotInfo.req has been done. it is a default ping slot periodicity & ping slot frequency*/
  if (Periodicity > 7)
  {
    Periodicity =  DefaultPingSlotPeriodicity;
    region = lora_region_get();
    getPhy.Attribute = PHY_BEACON_CHANNEL_FREQ;
    phyParam = RegionGetPhyParam(region, &getPhy);

    AT_PRINTF("PS:%d,%d,%d\r\n", DefaultPingSlotPeriodicity,
              Ctx.NvmCtx->PingSlotCtx.Datarate,
              phyParam.Value);
  }
  else
  {
    if (!Ctx.NvmCtx->PingSlotCtx.Frequency)  /*default pingslot frequency*/
    {
      region = lora_region_get();
      getPhy.Attribute = PHY_BEACON_CHANNEL_FREQ;
      phyParam = RegionGetPhyParam(region, &getPhy);

      AT_PRINTF("PS:%d,%d,%d\r\n", Periodicity,
                Ctx.NvmCtx->PingSlotCtx.Datarate,
                phyParam);
    }
    else
    {
      AT_PRINTF("PS:%d,%d,%d\r\n", Periodicity,
                Ctx.NvmCtx->PingSlotCtx.Datarate,
                Ctx.NvmCtx->PingSlotCtx.Frequency);
    }
  }

  return AT_OK;
}

ATEerror_t at_PingSlot_set(const char *param)
{
  int8_t PinSlotPeriodicity;
  uint8_t PingNb;
  uint16_t PingPeriod;

  if (tiny_sscanf(param, "%hhu", &PinSlotPeriodicity) != 1)
  {
    return AT_PARAM_ERROR;
  }
  if ((PinSlotPeriodicity > 7) || (PinSlotPeriodicity < 0))
  {
    return AT_PARAM_ERROR;
  }

  DefaultPingSlotPeriodicity =  PinSlotPeriodicity;

  return AT_OK;
}


ATEerror_t at_BeaconFreq_get(const char *param)
{
  GetPhyParams_t getPhy;
  PhyParam_t phyParam;
  LoRaMacRegion_t region;

  if (!Ctx.NvmCtx->BeaconCtx.Frequency)  /*default beacon frequency*/
  {
    region = lora_region_get();
    getPhy.Attribute = PHY_BEACON_CHANNEL_FREQ;
    phyParam = RegionGetPhyParam(region, &getPhy);

    AT_PRINTF("BCON:%d\r\n", phyParam.Value);
  }
  else
  {
    AT_PRINTF("BCON:%d\r\n", Ctx.NvmCtx->BeaconCtx.Frequency);
  }
  return AT_OK;
}

ATEerror_t at_BeaconTime_get(const char *param)
{
  MibRequestConfirm_t mib;

  mib.Type = MIB_DEVICE_CLASS;
  LoRaMacMibGetRequestConfirm(&mib);

  if (mib.Param.Class == CLASS_B)
  {
    if ((Ctx.BeaconCtx.Ctrl.BeaconAcquired == 1) && Ctx.BeaconCtx.Ctrl.BeaconMode == 1) /*Beacon locked */
    {
      AT_PRINTF("BTIME:%d\r\n", Ctx.BeaconCtx.BeaconTime);
      return AT_OK;
    }
  }
  else
  {
    return AT_NO_CLASS_B_ENABLE;
  }
  return AT_OK;
}

ATEerror_t at_BeaconGatewayCoordinate_get(const char *param)
{
  MibRequestConfirm_t mib;

  mib.Type = MIB_DEVICE_CLASS;
  LoRaMacMibGetRequestConfirm(&mib);


  if (mib.Param.Class == CLASS_B)
  {
    if ((Ctx.BeaconState == BEACON_STATE_LOCKED) || /*Beacon locked on Gateway*/
        (Ctx.BeaconState == BEACON_STATE_IDLE)   ||
        (Ctx.BeaconState == BEACON_STATE_GUARD)  ||
        (Ctx.BeaconState == BEACON_STATE_RX))
    {
      AT_PRINTF("BGW:%d,%02x%02x%02x,%02x%02x%02x\r\n",
                Ctx.LoRaMacClassBParams.MlmeIndication->BeaconInfo.GwSpecific.InfoDesc,
                Ctx.LoRaMacClassBParams.MlmeIndication->BeaconInfo.GwSpecific.Info[0],
                Ctx.LoRaMacClassBParams.MlmeIndication->BeaconInfo.GwSpecific.Info[1],
                Ctx.LoRaMacClassBParams.MlmeIndication->BeaconInfo.GwSpecific.Info[2],
                Ctx.LoRaMacClassBParams.MlmeIndication->BeaconInfo.GwSpecific.Info[3],
                Ctx.LoRaMacClassBParams.MlmeIndication->BeaconInfo.GwSpecific.Info[4],
                Ctx.LoRaMacClassBParams.MlmeIndication->BeaconInfo.GwSpecific.Info[5]);
      return AT_OK;
    }
  }
  else
  {
    return AT_NO_CLASS_B_ENABLE;
  }
  return AT_OK;
}

ATEerror_t at_LocalTime_get(const char *param)
{
  MibRequestConfirm_t mibReq;

  mibReq.Type = MIB_NETWORK_ACTIVATION;

  LoRaMacMibGetRequestConfirm(&mibReq);

  /*End-node needs to join the network to do a "DeviceTimeReq"*/
  if (mibReq.Param.NetworkActivation == ACTIVATION_TYPE_NONE)
  {
    return AT_NO_NET_JOINED;
  }
  /*Here the systeme time is EPoch synchronized*/
  else
  {
    SysTime_t UnixEpoch = SysTimeGet();
    struct tm localtime;

    UnixEpoch.Seconds -= 18; /*removing leap seconds*/

    UnixEpoch.Seconds += 3600 * 2; /*adding 2 hours*/

    SysTimeLocalTime(UnixEpoch.Seconds,  & localtime);

    AT_PRINTF("LTIME:%02dh%02dm%02ds on %02d/%02d/%04d\r\n",
              localtime.tm_hour, localtime.tm_min, localtime.tm_sec,
              localtime.tm_mday, localtime.tm_mon + 1, localtime.tm_year + 1900);
  }

  return AT_OK;
}
#endif /* LORAMAC_CLASSB_ENABLED */

ATEerror_t at_Join(const char *param)
{

  LoraErrorStatus status = LORA_Join();
  if (status == LORA_SUCCESS)
  {
    return AT_OK;
  }
  else
  {
    return AT_ERROR;
  }
}

ATEerror_t at_NetworkJoinStatus(const char *param)
{
  if (LORA_JoinStatus() == LORA_RESET)
  {
    print_d(0);
  }
  else
  {
    print_d(1);
  }

  return AT_OK;
}

ATEerror_t at_SendBinary(const char *param)
{
  LoraErrorStatus status;
  const char *buf = param;
  unsigned char bufSize = strlen(param);
  uint32_t appPort;
  unsigned size = 0;
  char hex[3];

  /* read and set the application port */
  if (1 != tiny_sscanf(buf, "%u:", &appPort))
  {
    AT_PRINTF("AT+SEND without the application port");
    return AT_PARAM_ERROR;
  }

  /* skip the application port */
  while (('0' <= buf[0]) && (buf[0] <= '9'))
  {
    buf ++;
    bufSize --;
  };

  if (buf[0] != ':')
  {
    AT_PRINTF("AT+SEND without the application port");
    return AT_PARAM_ERROR;
  }
  else
  {
    /*ok skip the char ':' */
    buf ++;
    bufSize --;
  }

  hex[2] = 0;
  while ((size < LORAWAN_APP_DATA_BUFF_SIZE) && (bufSize > 1))
  {
    hex[0] = buf[size * 2];
    hex[1] = buf[size * 2 + 1];
    if (tiny_sscanf(hex, "%hhx", &AppData.Buff[size]) != 1)
    {
      return AT_PARAM_ERROR;
    }
    size++;
    bufSize -= 2;
  }
  if (bufSize != 0)
  {
    return AT_PARAM_ERROR;
  }

  AppData.BuffSize = size;
  AppData.Port = appPort;

  status = LORA_send(&AppData, lora_config_reqack_get());

  if (status == LORA_SUCCESS)
  {
    return AT_OK;
  }
  else
  {
    return AT_ERROR;
  }
}

ATEerror_t at_Send(const char *param)
{
  LoraErrorStatus status;
  const char *buf = param;
  unsigned char bufSize = strlen(param);
  uint32_t appPort;

  /* read and set the application port */
  if (1 != tiny_sscanf(buf, "%u:", &appPort))
  {
    AT_PRINTF("AT+SEND is missing the application port");
    return AT_PARAM_ERROR;
  }

  /* skip the application port */
  while (('0' <= buf[0]) && (buf[0] <= '9'))
  {
    buf ++;
    bufSize --;
  };

  if (bufSize == 0)
  {
    AT_PRINTF("AT+SEND is missing text data");
    return AT_PARAM_ERROR;
  }
  /* skip the char ':' */
  buf ++;
  bufSize --;

  if (bufSize > LORAWAN_APP_DATA_BUFF_SIZE)
  {
    bufSize = LORAWAN_APP_DATA_BUFF_SIZE;
  }
  memcpy1(AppData.Buff, (uint8_t *)buf, bufSize);
  AppData.BuffSize = bufSize;
  AppData.Port = appPort;

  status = LORA_send(&AppData, lora_config_reqack_get());

  if (status == LORA_SUCCESS)
  {
    return AT_OK;
  }
  else
  {
    return AT_ERROR;
  }
}

ATEerror_t at_ReceiveBinary(const char *param)
{
  unsigned i;

  AT_PRINTF("%d:", ReceivedDataPort);
  for (i = 0; i < ReceivedDataSize; i++)
  {
    AT_PRINTF("%02x", ReceivedData[i]);
    ReceivedData[i] = 0;
  }
  AT_PRINTF("\r\n");
  ReceivedDataSize = 0;

  return AT_OK;
}

ATEerror_t at_Receive(const char *param)
{
  AT_PRINTF("%d:", ReceivedDataPort);
  if (ReceivedDataSize)
  {
    AT_PRINTF("%s", ReceivedData);
    memset1((uint8_t *)ReceivedData, 0, ReceivedDataSize);
    ReceivedDataSize = 0;
  }
  AT_PRINTF("\r\n");

  return AT_OK;
}

ATEerror_t at_version_get(const char *param)
{
  AT_PRINTF("APP_VERSION= %02X.%02X.%02X.%02X\r\n", (uint8_t)(__APP_VERSION >> 24), (uint8_t)(__APP_VERSION >> 16), (uint8_t)(__APP_VERSION >> 8), (uint8_t)__APP_VERSION);
  AT_PRINTF("MAC_VERSION= %02X.%02X.%02X.%02X\r\n", (uint8_t)(__LORA_MAC_VERSION >> 24), (uint8_t)(__LORA_MAC_VERSION >> 16), (uint8_t)(__LORA_MAC_VERSION >> 8), (uint8_t)__LORA_MAC_VERSION);
  return AT_OK;
}

ATEerror_t at_ack_set(const char *param)
{
  switch (param[0])
  {
    case '0':
      lora_config_reqack_set(LORAWAN_UNCONFIRMED_MSG);
      break;
    case '1':
      lora_config_reqack_set(LORAWAN_CONFIRMED_MSG);
      break;
    default:
      return AT_PARAM_ERROR;
  }

  return AT_OK;
}

ATEerror_t at_ack_get(const char *param)
{
  print_d(((lora_config_reqack_get() == LORAWAN_CONFIRMED_MSG) ? 1 : 0));
  return AT_OK;
}

ATEerror_t at_isack_get(const char *param)
{
  print_d(((lora_config_isack_get() == LORA_ENABLE) ? 1 : 0));
  return AT_OK;
}

ATEerror_t at_snr_get(const char *param)
{
  print_d(lora_config_snr_get());
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
  const char *buf = param;
  uint32_t nbbytes;

  if (1 != tiny_sscanf(buf, "%u", &nbbytes))
  {
    AT_PRINTF("AT+TTLRA: nb bytes sent is missing");
    return AT_PARAM_ERROR;
  }

  if (nbbytes > LORAWAN_APP_DATA_BUFF_SIZE)
  {
    nbbytes = LORAWAN_APP_DATA_BUFF_SIZE;
  }


  TST_TX_LoraStart(buf, nbbytes);

  return AT_OK;


}

ATEerror_t at_test_rxlora(const char *param)
{
  const char *buf = param;
  uint32_t nbbytes;

  if (1 != tiny_sscanf(buf, "%u", &nbbytes))
  {
    AT_PRINTF("AT+TTLRA: nb bytes sent is missing");
    return AT_PARAM_ERROR;
  }

  if (nbbytes > LORAWAN_APP_DATA_BUFF_SIZE)
  {
    nbbytes = LORAWAN_APP_DATA_BUFF_SIZE;
  }

  TST_RX_LoraStart(buf, nbbytes);

  return AT_OK;
}

ATEerror_t at_test_get_lora_config(const char *param)
{
  return TST_get_lora_config(param, strlen(param));
}

ATEerror_t at_test_set_lora_config(const char *param)
{
  return TST_SET_lora_config(param, strlen(param));
}

ATEerror_t at_test_stop(const char *param)
{
  return TST_stop();
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
  return tiny_sscanf(from, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                     &pt[0], &pt[1], &pt[2], &pt[3], &pt[4], &pt[5], &pt[6],
                     &pt[7], &pt[8], &pt[9], &pt[10], &pt[11], &pt[12], &pt[13],
                     &pt[14], &pt[15]);
}

static void print_16_02x(uint8_t *pt)
{
  AT_PRINTF("%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
            pt[0], pt[1], pt[2], pt[3],
            pt[4], pt[5], pt[6], pt[7],
            pt[8], pt[9], pt[10], pt[11],
            pt[12], pt[13], pt[14], pt[15]);
}

static int sscanf_uint32_as_hhx(const char *from, uint32_t *value)
{
  return tiny_sscanf(from, "%hhx:%hhx:%hhx:%hhx",
                     &((unsigned char *)(value))[3],
                     &((unsigned char *)(value))[2],
                     &((unsigned char *)(value))[1],
                     &((unsigned char *)(value))[0]);
}

static void print_uint32_as_02x(uint32_t value)
{
  AT_PRINTF("%02x:%02x:%02x:%02x\r\n",
            (unsigned)((unsigned char *)(&value))[3],
            (unsigned)((unsigned char *)(&value))[2],
            (unsigned)((unsigned char *)(&value))[1],
            (unsigned)((unsigned char *)(&value))[0]);
}

static void print_8_02x(uint8_t *pt)
{
  AT_PRINTF("%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
            pt[0], pt[1], pt[2], pt[3], pt[4], pt[5], pt[6], pt[7]);
}

static void print_d(int value)
{
  AT_PRINTF("%d\r\n", value);
}

static void print_u(unsigned int value)
{
  AT_PRINTF("%u\r\n", value);
}
