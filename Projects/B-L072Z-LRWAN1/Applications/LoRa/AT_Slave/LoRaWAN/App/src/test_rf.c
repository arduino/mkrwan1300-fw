/**
  ******************************************************************************
  * @file    test_rf.c
  * @author  MCD Application Team
  * @brief   manages tx tests
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

#include "hw.h"
#include "vcom.h"
#include <stdarg.h>
#include "radio.h"
#include "sx1276.h"
#include "mlm32l07x01.h"
#include "at.h"
#include "test_rf.h"
#include "command.h"
#include "tiny_sscanf.h"


/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint32_t freqHz;
  uint32_t power;
  uint32_t bandwidth;                                      // [0: 125 kHz,
  //  1: 250 kHz,
  //  2: 500 kHz,
  //  3: Reserved]
  uint32_t sf;                                             // [SF7..SF12]
  uint32_t codingRate;                                     // [1: 4/5,
  //  2: 4/6,
  //  3: 4/7,
  //  4: 4/8]
  uint32_t lna;                                     // 0:off 1:On
  uint32_t paBoost;                                     // 0:off 1:On
} s_loraParameter_t;

typedef enum
{
  BW_125kHz = 0,
  BW_250kHz = 1,
  BW_500kHz = 2,
} e_BandWidth_t;

typedef enum
{
  CR_4o5 = 1,
  CR_4o6 = 2,
  CR_4o7 = 3,
  CR_4o8 = 4,
} e_CodingRates_t;
typedef enum
{
  SF_7  = 7,
  SF_8  = 8,
  SF_9  = 9,
  SF_10 = 10,
  SF_11 = 11,
  SF_12 = 12,
} e_SpreadingFactors_t;

/* Private define ------------------------------------------------------------*/
#define F_868 868000000

#define P_14dBm 14
#define P_22dBm 22
#define EMISSION_POWER P_14dBm
//#define EMISSION_POWER P_22dBm

#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
#define TX_TEST_TONE    1<<0
#define RX_TEST_RSSI    1<<1
#define TX_TEST_LORA  1<<2
#define RX_TEST_LORA  1<<3

#define RX_TIMEOUT_VALUE                            3000//0xFFFFFFFF //RX continuous
#define MIN_16BITS                                  0x8000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t TestState = 0;

static s_loraParameter_t loraParam = { F_868, EMISSION_POWER, BW_125kHz, SF_12, 4, 0, 0};
//static s_loraParameter_t loraParam_lna= { F_868, EMISSION_POWER, BW_125kHz, SF_12, 4, 1, 0};

static uint32_t RadioTXDone_flag = 0;
static uint32_t RadioTXTimeout_flag = 0;
static uint32_t RadioRXDone_flag = 0;
static uint32_t RadioRXTimeout_flag = 0;
static uint32_t RadioError_flag = 0;
static uint32_t count_RXDone = 0;
static uint32_t count_RXTimeout = 0;
static uint32_t PER_ratio = 0;

/*!
 * Radio events function pointer
 */
static RadioEvents_t RadioEvents;

/* Private function prototypes -----------------------------------------------*/

static bool is_in_list(uint32_t in, uint32_t *list, uint32_t list_len);
/*!
 * \brief Function to be executed on Radio Tx Done event
 */
void OnTxDone(void);

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
void OnTxTimeout(void);

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
void OnRxTimeout(void);

/*!
 * \brief Function executed on Radio Rx Error event
 */
void OnRxError(void);

/* Functions Definition ------------------------------------------------------*/

/* receive test functions */

ATEerror_t TST_TxTone(const char *buf, unsigned bufSize)
{
  uint8_t paboost = loraParam.paBoost;

  if ((TestState & TX_TEST_TONE) != TX_TEST_TONE)
  {
    TestState |= TX_TEST_TONE;

    PRINTF("Tx FSK Test\r");

    SX1276SetModem(MODEM_FSK);

    Radio.SetChannel(loraParam.freqHz);

    Radio.Write(REG_FDEVMSB, 0x00);                             // FdevMsb = 0
    Radio.Write(REG_FDEVLSB, 0x00);                             // FdevLsb = 0

    // SX1276 in continuous mode FSK
    Radio.Write(REG_PACKETCONFIG2, (Radio.Read(REG_PACKETCONFIG2) & RF_PACKETCONFIG2_DATAMODE_MASK));

    switch (loraParam.power)
    {
      case 20:
      {
        Radio.Write(REG_PACONFIG, 0xFF);                               // PA_Boost 17 dBm
        Radio.Write(REG_PADAC, (Radio.Read(REG_PADAC) & RF_PADAC_20DBM_MASK) | RF_PADAC_20DBM_ON);        // Enable 20dBm boost
        PRINTF("force PA boost Output\r\n");
        break;
      }
      case 17:
      {
        Radio.Write(REG_PACONFIG, 0xFF);                             // PA_Boost 17 dBm
        Radio.Write(REG_PADAC, (Radio.Read(REG_PADAC) & RF_PADAC_20DBM_MASK) | RF_PADAC_20DBM_OFF);        // Disable 20dBm boost
        PRINTF("force PA boost Output\r\n");
        break;
      }
      case 14 :
      {
        if (paboost == 1)
        {
          Radio.Write(REG_PACONFIG, 0xFB);                             // PA_Boost 14 dBm
          Radio.Write(REG_PADAC, (Radio.Read(REG_PADAC) & RF_PADAC_20DBM_MASK) | RF_PADAC_20DBM_OFF);        // Disable 20dBm boost
        }
        else
        {
          //selects RFO
          Radio.Write(REG_PACONFIG, 0x7F);                             // RFO 14 dBm
        }
        break;
      }
      case 10:
      {
        if (paboost == 1)
        {
          Radio.Write(REG_PACONFIG, 0xF7);                          // PA_Boost 10 dBm
          Radio.Write(REG_PADAC, (Radio.Read(REG_PADAC) & RF_PADAC_20DBM_MASK) | RF_PADAC_20DBM_OFF);        // Disable 20dBm boost
        }
        else
        {
          Radio.Write(REG_PACONFIG, 0x7A);                          // RFO 10 dBm
        }
        break;
      }
      case 7 :
      {
        if (paboost == 1)
        {
          Radio.Write(REG_PACONFIG, 0xF5);                                  // PA_Boost 7 dBm
          Radio.Write(REG_PADAC, (Radio.Read(REG_PADAC) & RF_PADAC_20DBM_MASK) | RF_PADAC_20DBM_OFF);        // Disable 20dBm boost
        }
        else
        {
          Radio.Write(REG_PACONFIG, 0x78);                                  // RFO 7 dBm
        }
        break;
      }
      default:
        break;
    }
    SX1276SetOpMode(RF_OPMODE_TRANSMITTER);
    return AT_OK;
  }
  else
  {
    return AT_BUSY_ERROR;
  }
}


/* receive test functions */

ATEerror_t TST_RxTone(const char *buf, unsigned bufSize)
{
  /* check that test is not already started*/
  if ((TestState & RX_TEST_RSSI) != RX_TEST_RSSI)
  {
    TestState |= RX_TEST_RSSI;
    PRINTF("Rx FSK Test\r");
    SX1276SetModem(MODEM_FSK);

    Radio.SetChannel(loraParam.freqHz);

    Radio.Write(REG_BITRATEMSB, 0x1A);             // bitrate =  4800 bps
    Radio.Write(REG_BITRATELSB, 0x0B);             //

    Radio.Write(REG_FDEVMSB, 0x00);                // Frequency deviation = 5 KHz
    Radio.Write(REG_FDEVLSB, 0x52);                //

    if (loraParam.lna == 0)
    {
      Radio.Write(REG_LR_LNA, 0x20); //LNA off
      PRINTF(">>> LNA is OFF\r\n");
    }
    else// if (lnaState==1)
    {
      PRINTF(">>> LNA is ON\r\n");
      Radio.Write(REG_LR_LNA, 0x23); //LNA on
    }

    // SX1276 in continuous mode FSK
    Radio.Write(REG_PACKETCONFIG2, (Radio.Read(REG_PACKETCONFIG2) & RF_PACKETCONFIG2_DATAMODE_MASK));

    Radio.Write(REG_RSSICONFIG, 0x07);  //256 samples average

    SX1276SetOpMode(RF_OPMODE_RECEIVER);


    Radio.Write(REG_RXCONFIG, 0x40);    //Triggers a manual Restart of the Receiver chain

    return AT_OK;
  }
  else
  {
    return AT_BUSY_ERROR;
  }
}




ATEerror_t TST_SET_lora_config(const char *buf, unsigned bufSize)
{
  s_loraParameter_t loraParamTmp;
  uint32_t sfSet[] = {7, 8, 9, 10, 11, 12};
  uint32_t crDenSet[] = {5, 6, 7, 8};
  uint32_t crNum;

  if (8 != tiny_sscanf(buf, "%d:%d:%d:%d:%d/%d:%d:%d",
                       &loraParamTmp.freqHz,
                       &loraParamTmp.power,
                       &loraParamTmp.bandwidth,
                       &loraParamTmp.sf,
                       &crNum,
                       &loraParamTmp.codingRate,
                       &loraParamTmp.lna,
                       &loraParamTmp.paBoost))
  {
    return AT_PARAM_ERROR;
  }

  if ((loraParamTmp.freqHz >  100) && (loraParamTmp.freqHz <  1000))
  {
    /* User meant MHz */
    loraParamTmp.freqHz *= 1000000;
  }
  if ((loraParamTmp.freqHz > ((uint32_t) 100e6)) && (loraParamTmp.freqHz < ((uint32_t) 1e9)))
  {
    loraParam.freqHz = loraParamTmp.freqHz;
  }
  else
  {
    return AT_PARAM_ERROR;
  }
  loraParam.power = loraParamTmp.power;
  /* check that bandwidth is ok*/
  if (loraParamTmp.bandwidth == 125)
  {
    loraParam.bandwidth = BW_125kHz;
  }
  else if (loraParamTmp.bandwidth == 250)
  {
    loraParam.bandwidth = BW_250kHz;
  }
  else if (loraParamTmp.bandwidth == 500)
  {
    loraParam.bandwidth = BW_500kHz;
  }
  else
  {
    return AT_PARAM_ERROR;
  }
  /* check that spreading factor is ok*/
  if (!is_in_list(loraParamTmp.sf, sfSet, sizeof(sfSet)))
  {
    return AT_PARAM_ERROR;
  }
  else
  {
    loraParam.sf = loraParamTmp.sf;
  }
  /* check coding rate numerator is ok*/
  if (crNum != 4)
  {
    return AT_PARAM_ERROR;
  }
  /* check coding rate denominator is ok*/
  if (!is_in_list(loraParamTmp.codingRate, crDenSet, sizeof(crDenSet)))
  {
    return AT_PARAM_ERROR;
  }
  else
  {
    loraParam.codingRate = loraParamTmp.codingRate - 4;
  }

  loraParam.freqHz = loraParamTmp.freqHz;
  loraParam.lna = loraParamTmp.lna;
  loraParam.paBoost = loraParamTmp.paBoost;

  return AT_OK;
}

ATEerror_t TST_get_lora_config(const char *buf, unsigned bufSize)
{
  uint32_t bwSet[] = {125, 250, 500};
  AT_PRINTF("Freq= %d Hz\r\n", loraParam.freqHz);
  AT_PRINTF("Power= %d dBm\r\n", loraParam.power);
  AT_PRINTF("Bandwidth= %d kHz\r\n", bwSet[loraParam.bandwidth]);
  AT_PRINTF("SF= %d \r\n", loraParam.sf);
  AT_PRINTF("CR= 4/%d \r\n", loraParam.codingRate + 4);
  AT_PRINTF("LNA State =%d  \r\n", loraParam.lna);
  AT_PRINTF("PA Boost State =%d  \r\n", loraParam.paBoost);
  AT_PRINTF("%d:%d:%d:%d:4/%d:%d:%d\r\n",  loraParam.freqHz, loraParam.power, bwSet[loraParam.bandwidth], \
            loraParam.sf, loraParam.codingRate + 4, loraParam.lna, loraParam.paBoost);

  return AT_OK;
}


ATEerror_t TST_stop(void)
{
  uint8_t rssiReg ;
  int16_t rssiValue;

  if ((TestState & RX_TEST_RSSI) == RX_TEST_RSSI)
  {
    rssiReg =  Radio.Read(REG_RSSIVALUE);
    PRINTF("RSSI=%d,%d dBm\r\n", -(rssiReg / 2), rssiReg & 0x1 ? 5 : 0);
  }
  if ((TestState & RX_TEST_LORA) == RX_TEST_LORA)
  {
    rssiValue = SX1276.Settings.LoRaPacketHandler.RssiValue;
    if (rssiValue == (int16_t)MIN_16BITS)
    {
      PRINTF(">>> NO RX LORA\r\n");
    }
    else
    {
      PRINTF("RSSI=%d dBm\r\n", rssiValue);
    }
  }

  TestState = 0;

  PRINTF("Test Stop\r\n");
  /* Set the radio in standBy*/
  SX1276SetOpMode(RF_OPMODE_SLEEP);

  return AT_OK;
}



ATEerror_t TST_TX_LoraStart(const char *buf, unsigned bufSize)
{
  int k;

  uint8_t bufTx[] = {0x00, 0x11, 0x22, 0x33,
                     0x44, 0x55, 0x66, 0x77,
                     0x88, 0x99, 0xAA, 0xBB,
                     0xCC, 0xDD, 0xEE, 0xFF
                    };

  if ((TestState & TX_TEST_LORA) != TX_TEST_LORA)
  {

    TestState |= TX_TEST_LORA;

    PRINTF("\nTx LoRa Test\n\r");

    /* Radio initialization */
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;



    /* Launch several times bufTx: nb times given by user */
    for (k = 0; k < bufSize; k++)
    {
      PRINTF("Tx LoRa Test: Packet %d of %d\r\n", (k + 1), bufSize);

      /* ReInit radio each time after it has been set in Sleep*/
      Radio.Init(&RadioEvents);

      Radio.SetModem(MODEM_LORA);

      Radio.SetChannel(loraParam.freqHz);

      Radio.SetTxConfig(MODEM_LORA, loraParam.power, 0, loraParam.bandwidth,
                        loraParam.sf, loraParam.codingRate,
                        LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                        true, 0, 0, LORA_IQ_INVERSION_ON, 3000);
      /* Send bufTx once*/
      Radio.Send(bufTx, sizeof(bufTx));

      /* Wait Tx done/timeout */
      while ((RadioTXDone_flag == 0) && (RadioTXTimeout_flag == 0) && (RadioError_flag == 0)) {};


      /* Reset TX Done or timeout flags */
      RadioTXDone_flag = 0;
      RadioTXTimeout_flag = 0;
      RadioError_flag = 0;

    }

    return AT_OK;
  }
  else
  {
    return AT_BUSY_ERROR;
  }
}


ATEerror_t TST_RX_LoraStart(const char *buf, unsigned bufSize)
{
  ATEerror_t rf_status_error;
  int i;


  if ((TestState & RX_TEST_LORA) != RX_TEST_LORA)
  {
    TestState |= RX_TEST_LORA;


    /* init of PER counter */
    count_RXDone = 0;
    count_RXTimeout = 0;

    /* Radio initialization */
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;


    for (i = 0; i < bufSize; i++)
    {
      PRINTF("Rx LoRa Test: Packet %d of %d\r\n", (i + 1), bufSize);

      /* ReInit radio each time after it has been set in Sleep*/
      Radio.Init(&RadioEvents);

      /**** Sequence: RX LoRA ****/

      Radio.SetChannel(loraParam.freqHz);

      /* Rx continuous */
      Radio.SetRxConfig(MODEM_LORA, loraParam.bandwidth, loraParam.sf,
                        loraParam.codingRate, 0, LORA_PREAMBLE_LENGTH,
                        LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                        0, true, 0, 0, LORA_IQ_INVERSION_ON, true);


      Radio.Rx(RX_TIMEOUT_VALUE);

      /**** END Sequence: RX LoRA ****/

      /* Wait Rx done/timeout */
      while ((RadioRXDone_flag == 0) && (RadioRXTimeout_flag == 0) && (RadioError_flag == 0)) {};


      if (RadioRXDone_flag == 1)
      {
        count_RXDone++;
      }
      if (RadioRXTimeout_flag == 1)
      {
        count_RXTimeout++;
      }
      if (RadioError_flag == 1)
      {
        count_RXTimeout++;
      }

      /* Reset timeout flag */
      RadioRXDone_flag = 0;
      RadioRXTimeout_flag = 0;

      /* Stop RF */
      rf_status_error = TST_stop();
      com_error(rf_status_error);
    }


    /* Compute PER */
    if (bufSize != 0)
    {
      PER_ratio = (100 * count_RXTimeout) / (count_RXDone + count_RXTimeout);

      PRINTF(">>> PER Value (percent)= %d\r\n", PER_ratio);
    }

    return AT_OK;
  }
  else
  {
    return AT_BUSY_ERROR;
  }
}



static bool is_in_list(uint32_t in, uint32_t *list, uint32_t list_len)
{
  bool status = false;
  for (int i = 0; i < list_len; i++)
  {
    if (list[i] == in)
    {
      status = true;
    }
  }
  return status;
}


void OnTxDone(void)
{
  /* Set the radio in standBy*/
  SX1276SetOpMode(RF_OPMODE_SLEEP);


  /* Set TXdone flag */
  RadioTXDone_flag = 1;

  PRINTF("OnTxDone\n\r");
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
  /* Set the radio in standBy*/
  SX1276SetOpMode(RF_OPMODE_SLEEP);


  /* Set RXdone flag */
  RadioRXDone_flag = 1;

  PRINTF("OnRxDone\r\n");
  PRINTF("RssiValue=%d dBm, SnrValue=%d\n\r", rssi, snr);
}

void OnTxTimeout(void)
{
  /* Set the radio in standBy*/
  SX1276SetOpMode(RF_OPMODE_SLEEP);


  /* Set timeout flag */
  RadioTXTimeout_flag = 1;

  PRINTF("OnTxTimeout\r\n");
}

void OnRxTimeout(void)
{
  /* Set the radio in standBy*/
  SX1276SetOpMode(RF_OPMODE_SLEEP);


  /* Set timeout flag */
  RadioRXTimeout_flag = 1;

  PRINTF("OnRxTimeout\r\n");
}

void OnRxError(void)
{
  /* Set the radio in standBy*/
  SX1276SetOpMode(RF_OPMODE_SLEEP);


  /* Set error flag */
  RadioError_flag = 1;

  PRINTF("OnRxError\r\n");
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

