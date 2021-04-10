/**
  ******************************************************************************
  * @file    lora.c
  * @author  MCD Application Team
  * @brief   lora API to drive the lora state Machine
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
#include "hw.h"
#include "timeServer.h"
#include "LoRaMac.h"
#include "lora.h"
#include "lora-test.h"
#include "tiny_sscanf.h"


/**
  * Lora Configuration
  */
typedef struct
{
  LoraState_t otaa;        /*< ENABLE if over the air activation, DISABLE otherwise */
  LoraState_t duty_cycle;  /*< ENABLE if dutycyle is on, DISABLE otherwise */
  uint8_t DevEui[8];           /*< Device EUI */
  uint8_t JoinEui[8];           /*< Join Eui */
  uint8_t AppKey[16];          /*< Application Key */
  uint8_t NwkKey[16];          /*< Application Key */
  uint8_t NwkSEncKey[16];         /*< Network Session Key */
  uint8_t AppSKey[16];         /*< Application Session Key */
  uint8_t FNwkSIntKey[16];         /*< Application Session Key */
  uint8_t SNwkSIntKey[16];         /*< Application Session Key */
  int16_t Rssi;                /*< Rssi of the received packet */
  int8_t Snr;                 /*< Snr of the received packet */
  uint8_t application_port;    /*< Application port we will receive to */
  LoraConfirm_t ReqAck;      /*< ENABLE if acknowledge is requested */
  McpsConfirm_t *McpsConfirm;  /*< pointer to the confirm structure */
  int8_t TxDatarate;
} lora_configuration_t;


static lora_configuration_t lora_config =
{
  .otaa = LORA_ENABLE,
  .duty_cycle = LORA_DISABLE,
  .DevEui = LORAWAN_DEVICE_EUI,
  .JoinEui = LORAWAN_JOIN_EUI,
  .AppKey = LORAWAN_APP_KEY,
  .NwkKey = LORAWAN_NWK_KEY,
  .NwkSEncKey = LORAWAN_NWK_S_ENC_KEY,
  .AppSKey = LORAWAN_APP_S_KEY,
  .FNwkSIntKey = LORAWAN_F_NWK_S_INT_KEY,
  .SNwkSIntKey = LORAWAN_S_NWK_S_INT_KEY,
  .Rssi = 0,
  .Snr = 0,
  .ReqAck = LORAWAN_UNCONFIRMED_MSG,
  .McpsConfirm = NULL,
  .TxDatarate = 0
};

/*!
 *  Select either Device_Time_req or Beacon_Time_Req following LoRaWAN version
 *  - Device_Time_req   Available for V1.0.3 or later
 *  - Beacon_time_Req   Available for V1.0.2 and before
 */
#define USE_DEVICE_TIMING

/*!
 * Join requests trials duty cycle.
 */
#define OVER_THE_AIR_ACTIVATION_DUTYCYCLE           10000  // 10 [s] value in ms

#include "LoRaMacTest.h"

/*!
 * LoRaWAN ETSI duty cycle control enable/disable
 *
 * \remark Please note that ETSI mandates duty cycled transmissions. Use only for test purposes
 */
#define LORAWAN_DUTYCYCLE_ON                        true


#ifdef LORAMAC_CLASSB_ENABLED
/*!
 * Default ping slots periodicity
 *
 * \remark periodicity is equal to 2^LORAWAN_DEFAULT_PING_SLOT_PERIODICITY seconds
 *         example: 2^3 = 8 seconds. The end-device will open an Rx slot every 8 seconds.
 */
#define LORAWAN_DEFAULT_PING_SLOT_PERIODICITY       0
uint8_t  DefaultPingSlotPeriodicity;

#endif
#define HEX16(X)  X[0],X[1], X[2],X[3], X[4],X[5], X[6],X[7],X[8],X[9], X[10],X[11], X[12],X[13], X[14],X[15]
#define HEX8(X)   X[0],X[1], X[2],X[3], X[4],X[5], X[6],X[7]

static MlmeReqJoin_t JoinParameters;


static uint32_t DevAddr = LORAWAN_DEVICE_ADDRESS;

#ifdef LORAMAC_CLASSB_ENABLED
static LoraErrorStatus LORA_BeaconReq(void);
static LoraErrorStatus LORA_PingSlotReq(void);

#if defined( USE_DEVICE_TIMING )
static LoraErrorStatus LORA_DeviceTimeReq(void);
#else
static LoraErrorStatus LORA_BeaconTimeReq(void);
#endif /* USE_DEVICE_TIMING */
#endif /* LORAMAC_CLASSB_ENABLED */

/*!
 * Defines the LoRa parameters at Init
 */
static LoRaParam_t *LoRaParamInit;
static LoRaMacPrimitives_t LoRaMacPrimitives;
static LoRaMacCallback_t LoRaMacCallbacks;
static MibRequestConfirm_t mibReq;
static LoRaMacRegion_t LoRaRegion;

static LoRaMainCallback_t *LoRaMainCallbacks;
/*!
 * \brief   MCPS-Confirm event function
 *
 * \param   [IN] McpsConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void McpsConfirm(McpsConfirm_t *mcpsConfirm)
{
  lora_config.McpsConfirm = mcpsConfirm;
  if (mcpsConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
  {
    switch (mcpsConfirm->McpsRequest)
    {
      case MCPS_UNCONFIRMED:
      {
        // Check Datarate
        // Check TxPower
        break;
      }
      case MCPS_CONFIRMED:
      {
        // Check Datarate
        // Check TxPower
        // Check AckReceived
        if (mcpsConfirm->AckReceived)
        {
          LoRaMainCallbacks->LORA_McpsDataConfirm();
        }
        // Check NbTrials
        break;
      }
      case MCPS_PROPRIETARY:
      {
        break;
      }
      default:
        break;
    }
  }
}

/*!
 * \brief   MCPS-Indication event function
 *
 * \param   [IN] mcpsIndication - Pointer to the indication structure,
 *               containing indication attributes.
 */
static void McpsIndication(McpsIndication_t *mcpsIndication)
{
  lora_AppData_t AppData;

  if (mcpsIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK)
  {
    return;
  }

  switch (mcpsIndication->McpsIndication)
  {
    case MCPS_UNCONFIRMED:
    {
      break;
    }
    case MCPS_CONFIRMED:
    {
      break;
    }
    case MCPS_PROPRIETARY:
    {
      break;
    }
    case MCPS_MULTICAST:
    {
      break;
    }
    default:
      break;
  }

  // Check Multicast
  // Check Port
  // Check Datarate
  // Check FramePending
  if (mcpsIndication->FramePending == true)
  {
    // The server signals that it has pending data to be sent.
    // We schedule an uplink as soon as possible to flush the server.
    LoRaMainCallbacks->LORA_TxNeeded();
  }
  // Check Buffer
  // Check BufferSize
  // Check Rssi
  // Check Snr
  // Check RxSlot
  if (certif_running() == true)
  {
    certif_DownLinkIncrement();
  }

  if (mcpsIndication->RxData == true)
  {
    switch (mcpsIndication->Port)
    {
      case CERTIF_PORT:
        certif_rx(mcpsIndication, &JoinParameters);
        break;
      default:

        AppData.Port = mcpsIndication->Port;
        AppData.BuffSize = mcpsIndication->BufferSize;
        AppData.Buff = mcpsIndication->Buffer;
        lora_config.Rssi = mcpsIndication->Rssi;
        lora_config.Snr  = mcpsIndication->Snr;
        LoRaMainCallbacks->LORA_RxData(&AppData);
        break;
    }
  }
}

/*!
 * \brief   MLME-Confirm event function
 *
 * \param   [IN] MlmeConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void MlmeConfirm(MlmeConfirm_t *mlmeConfirm)
{
#ifdef LORAMAC_CLASSB_ENABLED
  MibRequestConfirm_t mibReq;
#endif /* LORAMAC_CLASSB_ENABLED */

  switch (mlmeConfirm->MlmeRequest)
  {
    case MLME_JOIN:
    {
      if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
      {
        // Status is OK, node has joined the network
        LoRaMainCallbacks->LORA_HasJoined();
        lora_config_tx_datarate_set(lora_config.TxDatarate);
#ifdef LORAMAC_CLASSB_ENABLED
#if defined( USE_DEVICE_TIMING )
        LORA_DeviceTimeReq();
#else
        LORA_BeaconTimeReq();
#endif /* USE_DEVICE_TIMING */
#endif /* LORAMAC_CLASSB_ENABLED */
      }
      else
      {
        // Join was not successful. Try to join again
        LORA_Join();
      }
      break;
    }
    case MLME_LINK_CHECK:
    {
      if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
      {
        // Check DemodMargin
        // Check NbGateways
        if (certif_running() == true)
        {
          certif_linkCheck(mlmeConfirm);
        }
      }
      break;
    }
#ifdef LORAMAC_CLASSB_ENABLED
    case MLME_BEACON_ACQUISITION:
    {
      if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
      {
        /* Beacon has been acquired */
        /* REquest Server for Ping Slot */
        LORA_PingSlotReq();
      }
      else
      {
        /* Beacon not acquired */
        /* Search again */
        /* we can check if the MAC has received a time reference for the beacon*/
        /* in this case do either a Device_Time_Req  or a Beacon_Timing_req*/
        LORA_BeaconReq();
      }
      break;
    }
    case MLME_PING_SLOT_INFO:
    {
      if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
      {
        /* class B is now ativated*/
        mibReq.Type = MIB_DEVICE_CLASS;
        mibReq.Param.Class = CLASS_B;
        LoRaMacMibSetRequestConfirm(&mibReq);

        if ((LoRaRegion == LORAMAC_REGION_AU915) || (LoRaRegion == LORAMAC_REGION_US915)) {
          mibReq.Type = MIB_PING_SLOT_DATARATE;
          mibReq.Param.PingSlotDatarate = DR_8;
          LoRaMacMibSetRequestConfirm(&mibReq);
        }

        /*notify upper layer*/
        LoRaMainCallbacks->LORA_ConfirmClass(CLASS_B);
      }
      else
      {
        LORA_PingSlotReq();
      }
      break;
    }
#if defined( USE_DEVICE_TIMING )
    case MLME_DEVICE_TIME:
    {
      if (mlmeConfirm->Status != LORAMAC_EVENT_INFO_STATUS_OK)
      {
        LORA_DeviceTimeReq();
      }
    }
#endif /* USE_DEVICE_TIMING */
#endif /* LORAMAC_CLASSB_ENABLED */
    default:
      break;
  }
}

/*!
 * \brief   MLME-Indication event function
 *
 * \param   [IN] MlmeIndication - Pointer to the indication structure.
 */
static void MlmeIndication(MlmeIndication_t *MlmeIndication)
{
#ifdef LORAMAC_CLASSB_ENABLED
  MibRequestConfirm_t mibReq;
#endif /* LORAMAC_CLASSB_ENABLED */


  switch (MlmeIndication->MlmeIndication)
  {
    case MLME_SCHEDULE_UPLINK:
    {
      // The MAC signals that we shall provide an uplink as soon as possible
      LoRaMainCallbacks->LORA_TxNeeded();
      break;
    }
#ifdef LORAMAC_CLASSB_ENABLED
    case MLME_BEACON_LOST:
    {
      // Switch to class A again
      mibReq.Type = MIB_DEVICE_CLASS;
      mibReq.Param.Class = CLASS_A;
      LoRaMacMibSetRequestConfirm(&mibReq);

      LORA_BeaconReq();
      break;
    }
    case MLME_BEACON:
    {
      if (MlmeIndication->Status == LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED)
      {
        TVL2(PRINTF("BEACON RECEIVED\n\r");)
      }
      else
      {
        TVL2(PRINTF("BEACON NOT RECEIVED\n\r");)
      }
      break;

    }
#endif /* LORAMAC_CLASSB_ENABLED */
    default:
      break;
  }
}

#define HYBRID
/**
 *  lora Init
 */
void LORA_Init(LoRaMainCallback_t *callbacks, LoRaParam_t *LoRaParam, LoRaMacRegion_t region)
{
  /* init the Tx Duty Cycle*/
  LoRaParamInit = LoRaParam;

  /* init the main call backs*/
  LoRaMainCallbacks = callbacks;

#if (STATIC_DEVICE_EUI != 1)
  LoRaMainCallbacks->BoardGetUniqueId(lora_config.DevEui);
#endif

#if (STATIC_DEVICE_ADDRESS != 1)
  // Random seed initialization
  srand1(LoRaMainCallbacks->BoardGetRandomSeed());
  // Choose a random device address
  DevAddr = randr(0, 0x01FFFFFF);
#endif

  LoRaMacPrimitives.MacMcpsConfirm = McpsConfirm;
  LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
  LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm;
  LoRaMacPrimitives.MacMlmeIndication = MlmeIndication;
  LoRaMacCallbacks.GetBatteryLevel = LoRaMainCallbacks->BoardGetBatteryLevel;
  LoRaMacCallbacks.GetTemperatureLevel = LoRaMainCallbacks->BoardGetTemperatureLevel;
  LoRaMacCallbacks.MacProcessNotify = LoRaMainCallbacks->MacProcessNotify;
  LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, region);
  LoRaRegion = region;

#if defined( HYBRID )
  if ((LoRaRegion == LORAMAC_REGION_US915) || (LoRaRegion == LORAMAC_REGION_AU915)) {
	  uint16_t channelMask[] = { 0x00FF, 0x0000, 0x0000, 0x0000, 0x0001, 0x0000};
	  mibReq.Type = MIB_CHANNELS_MASK;
	  mibReq.Param.ChannelsMask = channelMask;
	  LoRaMacMibSetRequestConfirm(&mibReq);
	  mibReq.Type = MIB_CHANNELS_DEFAULT_MASK;
	  mibReq.Param.ChannelsDefaultMask = channelMask;
	  LoRaMacMibSetRequestConfirm(&mibReq);
  }
  if (LoRaRegion == LORAMAC_REGION_CN470) {
	  uint16_t channelMask[] = { 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};
	  mibReq.Type = MIB_CHANNELS_MASK;
	  mibReq.Param.ChannelsMask = channelMask;
	  LoRaMacMibSetRequestConfirm(&mibReq);
	  mibReq.Type = MIB_CHANNELS_DEFAULT_MASK;
	  mibReq.Param.ChannelsDefaultMask = channelMask;
	  LoRaMacMibSetRequestConfirm(&mibReq);
  }
#endif

  lora_config_otaa_set(LORA_ENABLE);

  mibReq.Type = MIB_DEV_EUI;
  mibReq.Param.DevEui = lora_config.DevEui;
  LoRaMacMibSetRequestConfirm(&mibReq);

  mibReq.Type = MIB_JOIN_EUI;
  mibReq.Param.JoinEui = lora_config.JoinEui;
  LoRaMacMibSetRequestConfirm(&mibReq);

  mibReq.Type = MIB_ADR;
  mibReq.Param.AdrEnable = LoRaParamInit->AdrEnable;
  LoRaMacMibSetRequestConfirm(&mibReq);

  mibReq.Type = MIB_PUBLIC_NETWORK;
  mibReq.Param.EnablePublicNetwork = LoRaParamInit->EnablePublicNetwork;
  LoRaMacMibSetRequestConfirm(&mibReq);

  mibReq.Type = MIB_APP_KEY;
  mibReq.Param.AppKey = lora_config.AppKey;
  LoRaMacMibSetRequestConfirm(&mibReq);

  mibReq.Type = MIB_NWK_KEY;
  mibReq.Param.NwkKey = lora_config.NwkKey;
  LoRaMacMibSetRequestConfirm(&mibReq);

  mibReq.Type = MIB_DEVICE_CLASS;
  mibReq.Param.Class = CLASS_A;
  LoRaMacMibSetRequestConfirm(&mibReq);

  if ((LoRaRegion == LORAMAC_REGION_EU868) || (LoRaRegion == LORAMAC_REGION_RU864) || (LoRaRegion == LORAMAC_REGION_CN779) || (LoRaRegion == LORAMAC_REGION_EU433)) {
	  LoRaMacTestSetDutyCycleOn(LORAWAN_DUTYCYCLE_ON);

	  lora_config.duty_cycle = LORA_ENABLE;
  }
  else {
	  lora_config.duty_cycle = LORA_DISABLE;
  }

  mibReq.Type = MIB_SYSTEM_MAX_RX_ERROR;
  mibReq.Param.SystemMaxRxError = 20;
  LoRaMacMibSetRequestConfirm(&mibReq);

#ifdef LORAMAC_CLASSB_ENABLED
  DefaultPingSlotPeriodicity =  LORAWAN_DEFAULT_PING_SLOT_PERIODICITY;
#endif /* LORAMAC_CLASSB_ENABLED */

  /*set Mac statein Idle*/
  LoRaMacStart();
}


LoraErrorStatus LORA_Join(void)
{
  LoraErrorStatus status;
  if (lora_config.otaa == LORA_ENABLE)
  {
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_JOIN;
    mlmeReq.Req.Join.Datarate = LoRaParamInit->TxDatarate;

    JoinParameters = mlmeReq.Req.Join;

    LoRaMacMlmeRequest(&mlmeReq);

    status = LORA_SUCCESS;
  }
  else
  {
    /*no Join in abp*/
    status = LORA_ERROR;
  }

  return status;
}

LoraFlagStatus LORA_JoinStatus(void)
{
  MibRequestConfirm_t mibReq;

  mibReq.Type = MIB_NETWORK_ACTIVATION;

  LoRaMacMibGetRequestConfirm(&mibReq);

  if (mibReq.Param.NetworkActivation == ACTIVATION_TYPE_NONE)
  {
    return LORA_RESET;
  }
  else
  {
    return LORA_SET;
  }
}

LoraErrorStatus LORA_send(lora_AppData_t *AppData, LoraConfirm_t IsTxConfirmed)
{
  McpsReq_t mcpsReq;
  LoRaMacTxInfo_t txInfo;

  /*if certification test are on going, application data is not sent*/
  if (certif_running() == true)
  {
    return LORA_ERROR;
  }

  if (LoRaMacQueryTxPossible(AppData->BuffSize, &txInfo) != LORAMAC_STATUS_OK)
  {
    // Send empty frame in order to flush MAC commands
    mcpsReq.Type = MCPS_UNCONFIRMED;
    mcpsReq.Req.Unconfirmed.fBuffer = NULL;
    mcpsReq.Req.Unconfirmed.fBufferSize = 0;
    mcpsReq.Req.Unconfirmed.Datarate = lora_config_tx_datarate_get() ;
  }
  else
  {
    if (IsTxConfirmed == LORAWAN_UNCONFIRMED_MSG)
    {
      mcpsReq.Type = MCPS_UNCONFIRMED;
      mcpsReq.Req.Unconfirmed.fPort = AppData->Port;
      mcpsReq.Req.Unconfirmed.fBufferSize = AppData->BuffSize;
      mcpsReq.Req.Unconfirmed.fBuffer = AppData->Buff;
      mcpsReq.Req.Unconfirmed.Datarate = lora_config_tx_datarate_get() ;
    }
    else
    {
      mcpsReq.Type = MCPS_CONFIRMED;
      mcpsReq.Req.Confirmed.fPort = AppData->Port;
      mcpsReq.Req.Confirmed.fBufferSize = AppData->BuffSize;
      mcpsReq.Req.Confirmed.fBuffer = AppData->Buff;
      mcpsReq.Req.Confirmed.NbTrials = 8;
      mcpsReq.Req.Confirmed.Datarate = lora_config_tx_datarate_get() ;
    }
  }
  if (LoRaMacMcpsRequest(&mcpsReq) == LORAMAC_STATUS_OK)
  {
    return LORA_SUCCESS;
  }
  return LORA_ERROR;
}

#ifdef LORAMAC_CLASSB_ENABLED
#if defined( USE_DEVICE_TIMING )
static LoraErrorStatus LORA_DeviceTimeReq(void)
{
  MlmeReq_t mlmeReq;

  mlmeReq.Type = MLME_DEVICE_TIME;

  if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK)
  {
    return LORA_SUCCESS;
  }
  else
  {
    return LORA_ERROR;
  }
}
#else
static LoraErrorStatus LORA_BeaconTimeReq(void)
{
  MlmeReq_t mlmeReq;

  mlmeReq.Type = MLME_BEACON_TIMING;

  if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK)
  {
    return LORA_SUCCESS;
  }
  else
  {
    return LORA_ERROR;
  }
}
#endif

static LoraErrorStatus LORA_BeaconReq(void)
{
  MlmeReq_t mlmeReq;

  mlmeReq.Type = MLME_BEACON_ACQUISITION;

  if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK)
  {
    return LORA_SUCCESS;
  }
  else
  {
    return LORA_ERROR;
  }
}

static LoraErrorStatus LORA_PingSlotReq(void)
{

  MlmeReq_t mlmeReq;

  mlmeReq.Type = MLME_LINK_CHECK;
  LoRaMacMlmeRequest(&mlmeReq);

  mlmeReq.Type = MLME_PING_SLOT_INFO;
  mlmeReq.Req.PingSlotInfo.PingSlot.Fields.Periodicity = DefaultPingSlotPeriodicity;
  mlmeReq.Req.PingSlotInfo.PingSlot.Fields.RFU = 0;

  if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK)
  {
    return LORA_SUCCESS;
  }
  else
  {
    return LORA_ERROR;
  }
}
#endif /* LORAMAC_CLASSB_ENABLED */

LoraErrorStatus LORA_RequestClass(DeviceClass_t newClass)
{
  LoraErrorStatus Errorstatus = LORA_SUCCESS;
  MibRequestConfirm_t mibReq;
  DeviceClass_t currentClass;

  mibReq.Type = MIB_DEVICE_CLASS;
  LoRaMacMibGetRequestConfirm(&mibReq);

  currentClass = mibReq.Param.Class;
  /*attempt to swicth only if class update*/
  if (currentClass != newClass)
  {
    switch (newClass)
    {
      case CLASS_A:
      {
        mibReq.Param.Class = CLASS_A;
        if (LoRaMacMibSetRequestConfirm(&mibReq) == LORAMAC_STATUS_OK)
        {
          /*switch is instantanuous*/
          LoRaMainCallbacks->LORA_ConfirmClass(CLASS_A);
        }
        else
        {
          Errorstatus = LORA_ERROR;
        }
        break;
      }
      case CLASS_B:
      {
#ifdef LORAMAC_CLASSB_ENABLED
        if (currentClass != CLASS_A)
        {
          Errorstatus = LORA_ERROR;
        }
        /*switch is not instantanuous*/
        Errorstatus = LORA_BeaconReq();
#else
        PRINTF("warning: LORAMAC_CLASSB_ENABLED has not been defined at compilation\n\r");
#endif /* LORAMAC_CLASSB_ENABLED */
        break;
      }
      case CLASS_C:
      {
        if (currentClass != CLASS_A)
        {
          Errorstatus = LORA_ERROR;
        }
        /*switch is instantanuous*/
        mibReq.Param.Class = CLASS_C;
        if (LoRaMacMibSetRequestConfirm(&mibReq) == LORAMAC_STATUS_OK)
        {
          LoRaMainCallbacks->LORA_ConfirmClass(CLASS_C);
        }
        else
        {
          Errorstatus = LORA_ERROR;
        }
        break;
      }
      default:
        break;
    }
  }
  return Errorstatus;
}

void LORA_GetCurrentClass(DeviceClass_t *currentClass)
{
  MibRequestConfirm_t mibReq;

  mibReq.Type = MIB_DEVICE_CLASS;
  LoRaMacMibGetRequestConfirm(&mibReq);

  *currentClass = mibReq.Param.Class;
}


void lora_config_otaa_set(LoraState_t otaa)
{
  lora_config.otaa = otaa;

  if (lora_config.otaa == LORA_ENABLE)
  {
    PPRINTF("OTAA Mode enabled\n\r");
    PPRINTF("DevEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n\r", HEX8(lora_config.DevEui));
    PPRINTF("JoinEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n\r", HEX8(lora_config.JoinEui));
    PPRINTF("AppKey= %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n\r", HEX16(lora_config.AppKey));

    mibReq.Type = MIB_NETWORK_ACTIVATION;
    mibReq.Param.NetworkActivation = ACTIVATION_TYPE_NONE;
    LoRaMacMibSetRequestConfirm(&mibReq);
  }
  else
  {
    PPRINTF("ABP Mode enabled\n\r");
    PPRINTF("DevEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n\r", HEX8(lora_config.DevEui));
    PPRINTF("DevAdd=  %08X\n\r", DevAddr) ;
    PPRINTF("NwkSKey= %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n\r", HEX16(lora_config.NwkSEncKey));
    PPRINTF("AppSKey= %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n\r", HEX16(lora_config.AppSKey));

    mibReq.Type = MIB_NET_ID;
    mibReq.Param.NetID = LORAWAN_NETWORK_ID;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_DEV_ADDR;
    mibReq.Param.DevAddr = DevAddr;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_F_NWK_S_INT_KEY;
    mibReq.Param.FNwkSIntKey = lora_config.FNwkSIntKey;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_S_NWK_S_INT_KEY;
    mibReq.Param.SNwkSIntKey = lora_config.SNwkSIntKey;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_NWK_S_ENC_KEY;
    mibReq.Param.NwkSEncKey = lora_config.NwkSEncKey;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_APP_S_KEY;
    mibReq.Param.AppSKey = lora_config.AppSKey;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_NETWORK_ACTIVATION;
    mibReq.Param.NetworkActivation = ACTIVATION_TYPE_ABP;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // Enable legacy mode to operate according to LoRaWAN Spec. 1.0.3
    Version_t abpLrWanVersion;

    abpLrWanVersion.Fields.Major    = 1;
    abpLrWanVersion.Fields.Minor    = 0;
    abpLrWanVersion.Fields.Revision = 3;
    abpLrWanVersion.Fields.Rfu      = 0;

    mibReq.Type = MIB_ABP_LORAWAN_VERSION;
    mibReq.Param.AbpLrWanVersion = abpLrWanVersion;
    LoRaMacMibSetRequestConfirm(&mibReq);
  }
}


LoraState_t lora_config_otaa_get(void)
{
  return lora_config.otaa;
}

void lora_config_duty_cycle_set(LoraState_t duty_cycle)
{
  lora_config.duty_cycle = duty_cycle;
  LoRaMacTestSetDutyCycleOn((duty_cycle == LORA_ENABLE) ? 1 : 0);
}

LoraState_t lora_config_duty_cycle_get(void)
{
  return lora_config.duty_cycle;
}

uint8_t *lora_config_deveui_get(void)
{
  return lora_config.DevEui;
}

uint8_t *lora_config_joineui_get(void)
{
  return lora_config.JoinEui;
}

void lora_config_joineui_set(uint8_t joineui[8])
{
  memcpy1(lora_config.JoinEui, joineui, sizeof(lora_config.JoinEui));
}

uint8_t *lora_config_appkey_get(void)
{
  return lora_config.AppKey;
}

void lora_config_appkey_set(uint8_t appkey[16])
{
  memcpy1(lora_config.AppKey, appkey, sizeof(lora_config.AppKey));
  memcpy1(lora_config.NwkKey, appkey, sizeof(lora_config.NwkKey));

  mibReq.Type = MIB_APP_KEY;
  mibReq.Param.AppKey = lora_config.AppKey;
  LoRaMacMibSetRequestConfirm(&mibReq);

  mibReq.Type = MIB_NWK_KEY;
  mibReq.Param.NwkKey = lora_config.NwkKey;
  LoRaMacMibSetRequestConfirm(&mibReq);
}

void lora_config_reqack_set(LoraConfirm_t reqack)
{
  lora_config.ReqAck = reqack;
}

LoraConfirm_t lora_config_reqack_get(void)
{
  return lora_config.ReqAck;
}

int8_t lora_config_snr_get(void)
{
  return lora_config.Snr;
}

int16_t lora_config_rssi_get(void)
{
  return lora_config.Rssi;
}

void lora_config_tx_datarate_set(int8_t TxDataRate)
{
  mibReq.Type = MIB_CHANNELS_DATARATE;
  mibReq.Param.ChannelsDatarate = TxDataRate;
  if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_PARAMETER_INVALID) {
	lora_config.TxDatarate = TxDataRate;
  }
}

int8_t lora_config_tx_datarate_get(void)
{
  return lora_config.TxDatarate;
}

LoraState_t lora_config_isack_get(void)
{
  if (lora_config.McpsConfirm == NULL)
  {
    return LORA_DISABLE;
  }
  else
  {
    return (lora_config.McpsConfirm->AckReceived ? LORA_ENABLE : LORA_DISABLE);
  }
}


/* Dummy data sent periodically to let the tester respond with start test command*/
static TimerEvent_t TxcertifTimer;

void OnCertifTimer(void *context)
{
  uint8_t Dummy[1] = {1};
  lora_AppData_t AppData;
  AppData.Buff = Dummy;
  AppData.BuffSize = sizeof(Dummy);
  AppData.Port = 224;

  LORA_send(&AppData, LORAWAN_UNCONFIRMED_MSG);
}

void lora_wan_certif(void)
{
  LoRaMacTestSetDutyCycleOn(false);
  LORA_Join();
  TimerInit(&TxcertifTimer,  OnCertifTimer);  /* 8s */
  TimerSetValue(&TxcertifTimer,  8000);  /* 8s */
  TimerStart(&TxcertifTimer);

}

void TriggerReinit( LoRaMacRegion_t region ) {
	LORA_Init(LoRaMainCallbacks, LoRaParamInit, region);
}

LoRaMacRegion_t lora_region_get(void)
{
  return LoRaRegion;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

