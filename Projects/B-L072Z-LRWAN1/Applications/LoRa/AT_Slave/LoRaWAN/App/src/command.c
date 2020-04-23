/**
  ******************************************************************************
  * @file    command.c
  * @author  MCD Application Team
  * @brief   main command driver dedicated to command AT
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
#include <stdlib.h>
#include "at.h"
#include "hw.h"
#include "command.h"

/* comment the following to have help message */
/* #define NO_HELP */
/* #define NO_KEY_ADDR_EUI */

/* Private typedef -----------------------------------------------------------*/
/**
 * @brief  Structure defining an AT Command
 */
struct ATCommand_s
{
  const char *string;                       /*< command string, after the "AT" */
  const int size_string;                    /*< size of the command string, not including the final \0 */
  ATEerror_t (*get)(const char *param);     /*< =? after the string to get the current value*/
  ATEerror_t (*set)(const char *param);     /*< = (but not =?\0) after the string to set a value */
  ATEerror_t (*run)(const char *param);     /*< \0 after the string - run the command */
#if !defined(NO_HELP)
  const char *help_string;                  /*< to be printed when ? after the string */
#endif
};

/* Private define ------------------------------------------------------------*/
#define CMD_SIZE 270
#define CIRC_BUFF_SIZE 8
#define HELP_DISPLAY_FLUSH_DELAY 100

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
 * @brief  Array corresponding to the description of each possible AT Error
 */
static const char *const ATError_description[] =
{
  "\r\nOK\r\n",                     /* AT_OK */
  "\r\nAT_ERROR\r\n",               /* AT_ERROR */
  "\r\nAT_PARAM_ERROR\r\n",         /* AT_PARAM_ERROR */
  "\r\nAT_BUSY_ERROR\r\n",          /* AT_BUSY_ERROR */
  "\r\nAT_TEST_PARAM_OVERFLOW\r\n", /* AT_TEST_PARAM_OVERFLOW */
  "\r\nAT_NO_NETWORK_JOINED\r\n",   /* AT_NO_NET_JOINED */
  "\r\nAT_RX_ERROR\r\n",            /* AT_RX_ERROR */
  "\r\nAT_NO_CLASS_B_ENABLE\r\n",  /* AT_NO_CLASS_B_ENABLE */
  "\r\nerror unknown\r\n",          /* AT_MAX */
};

/**
 * @brief  Array of all supported AT Commands
 */
static const struct ATCommand_s ATCommand[] =
{
  {
    .string = AT_RESET,
    .size_string = sizeof(AT_RESET) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RESET ": Trig a reset of the MCU\r\n",
#endif
    .get = at_return_error,
    .set = at_return_error,
    .run = at_reset,
  },

#ifndef NO_KEY_ADDR_EUI
  {
    .string = AT_DEUI,
    .size_string = sizeof(AT_DEUI) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_DEUI ": Get the Device EUI\r\n",
#endif
    .get = at_DevEUI_get,
    .set = at_return_error,
    .run = at_return_error,
  },
#endif

#ifndef NO_KEY_ADDR_EUI
  {
    .string = AT_DADDR,
    .size_string = sizeof(AT_DADDR) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_DADDR ": Get or Set the Device address\r\n",
#endif
    .get = at_DevAddr_get,
    .set = at_DevAddr_set,
    .run = at_return_error,
  },
#endif

#ifndef NO_KEY_ADDR_EUI
  {
    .string = AT_APPKEY,
    .size_string = sizeof(AT_APPKEY) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_APPKEY ": Get or Set the Application Key\r\n",
#endif
    .get = at_AppKey_get,
    .set = at_AppKey_set,
    .run = at_return_error,
  },
#endif

#ifndef NO_KEY_ADDR_EUI
  {
    .string = AT_NWKSKEY,
    .size_string = sizeof(AT_NWKSKEY) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_NWKSKEY ": Set the Network Session Key\r\n",
#endif
    .get = at_return_error,
    .set = at_NwkSKey_set,
    .run = at_return_error,
  },
#endif

#ifndef NO_KEY_ADDR_EUI
  {
    .string = AT_APPSKEY,
    .size_string = sizeof(AT_APPSKEY) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_APPSKEY ": Set the Application Session Key\r\n",
#endif
    .get = at_return_error,
    .set = at_AppSKey_set,
    .run = at_return_error,
  },
#endif

#ifndef NO_KEY_ADDR_EUI
  {
    .string = AT_JOINEUI,
    .size_string = sizeof(AT_JOINEUI) - 1,
#ifndef NO_HELP
    /* .help_string = "AT"AT_JOINEUI ": Get or Set the Join Eui\r\n",*/  /* refer to comment in at.h file*/
    .help_string = "AT"AT_JOINEUI ": Get or Set the App Eui\r\n",
#endif
    .get = at_JoinEUI_get,
    .set = at_JoinEUI_set,
    .run = at_return_error,
  },
#endif

  {
    .string = AT_ADR,
    .size_string = sizeof(AT_ADR) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_ADR ": Get or Set the Adaptive Data Rate setting. (0: off, 1: on)\r\n",
#endif
    .get = at_ADR_get,
    .set = at_ADR_set,
    .run = at_return_error,
  },

  {
    .string = AT_TXP,
    .size_string = sizeof(AT_TXP) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_TXP ": Get or Set the Transmit Power (0-5)\r\n",
#endif
    .get = at_TransmitPower_get,
    .set = at_TransmitPower_set,
    .run = at_return_error,
  },

  {
    .string = AT_DR,
    .size_string = sizeof(AT_DR) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_DR ": Get or Set the Data Rate. (0-7 corresponding to DR_X)\r\n",
#endif
    .get = at_DataRate_get,
    .set = at_DataRate_set,
    .run = at_return_error,
  },

  {
    .string = AT_DCS,
    .size_string = sizeof(AT_DCS) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_DCS ": Get or Set the ETSI Duty Cycle setting - 0=disable, 1=enable - Only for testing\r\n",
#endif
    .get = at_DutyCycle_get,
    .set = at_DutyCycle_set,
    .run = at_return_error,
  },

  {
    .string = AT_PNM,
    .size_string = sizeof(AT_PNM) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_PNM ": Get or Set the public network mode. (0: off, 1: on)\r\n",
#endif
    .get = at_PublicNetwork_get,
    .set = at_PublicNetwork_set,
    .run = at_return_error,
  },

  {
    .string = AT_RX2FQ,
    .size_string = sizeof(AT_RX2FQ) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RX2FQ ": Get or Set the Rx2 window frequency\r\n",
#endif
    .get = at_Rx2Frequency_get,
    .set = at_Rx2Frequency_set,
    .run = at_return_error,
  },

  {
    .string = AT_RX2DR,
    .size_string = sizeof(AT_RX2DR) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RX2DR ": Get or Set the Rx2 window data rate (0-7 corresponding to DR_X)\r\n",
#endif
    .get = at_Rx2DataRate_get,
    .set = at_Rx2DataRate_set,
    .run = at_return_error,
  },

  {
    .string = AT_RX1DL,
    .size_string = sizeof(AT_RX1DL) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RX1DL ": Get or Set the delay between the end of the Tx and the Rx Window 1 in ms\r\n",
#endif
    .get = at_Rx1Delay_get,
    .set = at_Rx1Delay_set,
    .run = at_return_error,
  },

  {
    .string = AT_RX2DL,
    .size_string = sizeof(AT_RX2DL) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RX2DL ": Get or Set the delay between the end of the Tx and the Rx Window 2 in ms\r\n",
#endif
    .get = at_Rx2Delay_get,
    .set = at_Rx2Delay_set,
    .run = at_return_error,
  },

  {
    .string = AT_JN1DL,
    .size_string = sizeof(AT_JN1DL) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_JN1DL ": Get or Set the Join Accept Delay between the end of the Tx and the Join Rx Window 1 in ms\r\n",
#endif
    .get = at_JoinAcceptDelay1_get,
    .set = at_JoinAcceptDelay1_set,
    .run = at_return_error,
  },

  {
    .string = AT_JN2DL,
    .size_string = sizeof(AT_JN2DL) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_JN2DL ": Get or Set the Join Accept Delay between the end of the Tx and the Join Rx Window 2 in ms\r\n",
#endif
    .get = at_JoinAcceptDelay2_get,
    .set = at_JoinAcceptDelay2_set,
    .run = at_return_error,
  },

  {
    .string = AT_NJM,
    .size_string = sizeof(AT_NJM) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_NJM ": Get or Set the Network Join Mode. (0: ABP, 1: OTAA)\r\n",
#endif
    .get = at_NetworkJoinMode_get,
    .set = at_NetworkJoinMode_set,
    .run = at_return_error,
  },

#ifndef NO_KEY_ADDR_EUI
  {
    .string = AT_NWKID,
    .size_string = sizeof(AT_NWKID) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_NWKID ": Get or Set the Network ID\r\n",
#endif
    .get = at_NetworkID_get,
    .set = at_NetworkID_set,
    .run = at_return_error,
  },
#endif

  {
    .string = AT_CLASS,
    .size_string = sizeof(AT_CLASS) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_CLASS ": Get or Set the Device Class\r\n",
#endif
    .get = at_DeviceClass_get,
    .set = at_DeviceClass_set,
    .run = at_return_error,
  },

  {
    .string = AT_JOIN,
    .size_string = sizeof(AT_JOIN) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_JOIN ": Join network\r\n",
#endif
    .get = at_return_error,
    .set = at_return_error,
    .run = at_Join,
  },

  {
    .string = AT_NJS,
    .size_string = sizeof(AT_NJS) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_NJS ": Get the join status\r\n",
#endif
    .get = at_NetworkJoinStatus,
    .set = at_return_error,
    .run = at_return_error,
  },

  {
    .string = AT_SENDB,
    .size_string = sizeof(AT_SENDB) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_SENDB ": Send hexadecimal data along with the application port\r\n",
#endif
    .get = at_return_error,
    .set = at_SendBinary,
    .run = at_return_error,
  },

  {
    .string = AT_SEND,
    .size_string = sizeof(AT_SEND) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_SEND ": Send text data along with the application port\r\n",
#endif
    .get = at_return_error,
    .set = at_Send,
    .run = at_return_error,
  },

  {
    .string = AT_RECVB,
    .size_string = sizeof(AT_RECVB) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RECVB ": print last received data in binary format (with hexadecimal values)\r\n",
#endif
    .get = at_ReceiveBinary,
    .set = at_return_error,
    .run = at_ReceiveBinary,
  },

  {
    .string = AT_RECV,
    .size_string = sizeof(AT_RECV) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RECV ": print last received data in raw format\r\n",
#endif
    .get = at_Receive,
    .set = at_return_error,
    .run = at_Receive,
  },

  {
    .string = AT_VER,
    .size_string = sizeof(AT_VER) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_VER ": Get the version of the AT_Slave FW\r\n",
#endif
    .get = at_version_get,
    .set = at_return_error,
    .run = at_return_error,
  },

  {
    .string = AT_CFM,
    .size_string = sizeof(AT_CFM) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_CFM ": Get or Set the confirmation mode (0-1)\r\n",
#endif
    .get = at_ack_get,
    .set = at_ack_set,
    .run = at_return_error,
  },

  {
    .string = AT_CFS,
    .size_string = sizeof(AT_CFS) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_CFS ": Get confirmation status of the last AT+SEND (0-1)\r\n",
#endif
    .get = at_isack_get,
    .set = at_return_error,
    .run = at_return_error,
  },

  {
    .string = AT_SNR,
    .size_string = sizeof(AT_SNR) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_SNR ": Get the SNR of the last received packet\r\n",
#endif
    .get = at_snr_get,
    .set = at_return_error,
    .run = at_return_error,
  },

  {
    .string = AT_RSSI,
    .size_string = sizeof(AT_RSSI) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RSSI ": Get the RSSI of the last received packet\r\n",
#endif
    .get = at_rssi_get,
    .set = at_return_error,
    .run = at_return_error,
  },

  {
    .string = AT_BAT,
    .size_string = sizeof(AT_BAT) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_BAT ": Get the battery level\r\n",
#endif
    .get = at_bat_get,
    .set = at_return_error,
    .run = at_return_error,
  },
  {
    .string = AT_TRSSI,
    .size_string = sizeof(AT_TRSSI) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_TRSSI ": Starts RF RSSI tone test\r\n",
#endif
    .get = at_return_error,
    .set = at_return_error,
    .run = at_test_rxTone,
  },

  {
    .string = AT_TTONE,
    .size_string = sizeof(AT_TTONE) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_TTONE ": Starts RF Tone test\r\n",
#endif
    .get = at_return_error,
    .set = at_return_error,
    .run = at_test_txTone,
  },
  {
    .string = AT_TTLRA,
    .size_string = sizeof(AT_TTLRA) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_TTLRA ": Set Nb of packets sent with RF Tx LORA test\r\n",
#endif
    .get = at_return_error,
    .set = at_test_txlora,
    .run = at_return_error,
  },
  {
    .string = AT_TRLRA,
    .size_string = sizeof(AT_TRLRA) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_TRLRA ": Set Nb of packets received with RF Rx LORA test\r\n",
#endif
    .get = at_return_error,
    .set = at_test_rxlora,
    .run = at_return_error,
  },
  {
    .string = AT_TCONF,
    .size_string = sizeof(AT_TCONF) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_TCONF ": Config LORA RF test [Freq]:[Power]:[Bandwith]:[SF]:4/[CR]:[Lna]:[PA Boost]\r\n",
#endif
    .get = at_test_get_lora_config,
    .set = at_test_set_lora_config,
    .run = at_return_error,
  },
  {
    .string = AT_TOFF,
    .size_string = sizeof(AT_TOFF) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_TOFF ": Stops on-going RF test\r\n",
#endif
    .get = at_return_error,
    .set = at_return_error,
    .run = at_test_stop,
  },

  {
    .string = AT_CERTIF,
    .size_string = sizeof(AT_CERTIF) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_CERTIF ": Set the module in LoraWan Certification Mode\r\n",
#endif
    .get = at_return_error,
    .set = at_return_error,
    .run = at_Certif,
  },

#ifdef LORAMAC_CLASSB_ENABLED
  {
    .string = AT_PGSLOT,
    .size_string = sizeof(AT_PGSLOT) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_PGSLOT ": Set or Get the unicast ping slot periodicity\r\n",
#endif
    .get = at_PingSlot_get,
    .set = at_PingSlot_set,
    .run = at_return_error,
  },

  {
    .string = AT_BFREQ,
    .size_string = sizeof(AT_BFREQ) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_BFREQ ": Get the Beacon frequency\r\n",
#endif
    .get = at_BeaconFreq_get,
    .set = at_return_error,
    .run = at_return_error,
  },

  {
    .string = AT_BTIME,
    .size_string = sizeof(AT_BTIME) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_BTIME ": Get the Beacon Time (GPS Epoch time)\r\n",
#endif
    .get = at_BeaconTime_get,
    .set = at_return_error,
    .run = at_return_error,
  },

  {
    .string = AT_BGW,
    .size_string = sizeof(AT_BGW) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_BGW ": Get the Gatwway GPS coordinate, NetID and GwID\r\n",
#endif
    .get = at_BeaconGatewayCoordinate_get,
    .set = at_return_error,
    .run = at_return_error,
  },

  {
    .string = AT_LTIME,
    .size_string = sizeof(AT_LTIME) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_LTIME ": Get the local time in UTC format\r\n",
#endif
    .get = at_LocalTime_get,
    .set = at_return_error,
    .run = at_return_error,
  },
#endif /* LORAMAC_CLASSB_ENABLED */
};


/* Private function prototypes -----------------------------------------------*/



/**
 * @brief  Parse a command and process it
 * @param  The command
 * @retval None
 */
static void parse_cmd(const char *cmd);

/* Exported functions ---------------------------------------------------------*/
static void CMD_GetChar(uint8_t *rxChar);

static char circBuffer[CIRC_BUFF_SIZE];
static char command[CMD_SIZE];
static unsigned i = 0;
static uint32_t widx = 0;
static uint32_t ridx = 0;
static uint32_t charCount = 0;
static FlagStatus circBuffOverflow = RESET;

void CMD_Init(void)
{
  vcom_ReceiveInit(CMD_GetChar);
  widx = 0;
  ridx = 0;
  charCount = 0;
  i = 0;
  circBuffOverflow = RESET;
}

static void CMD_GetChar(uint8_t *rxChar)
{
  charCount++;
  if (charCount == (CIRC_BUFF_SIZE + 1))
  {
    circBuffOverflow = SET;
    charCount--;
  }
  else
  {
    circBuffer[widx++] = *rxChar;
    if (widx == CIRC_BUFF_SIZE)
    {
      widx = 0;
    }
  }
}

void CMD_Process(void)
{
  /* Process all commands */
  if (circBuffOverflow == SET)
  {
    com_error(AT_TEST_PARAM_OVERFLOW);
    /*Full flush in case of overflow */
    CRITICAL_SECTION_BEGIN();
    ridx = widx;
    charCount = 0;
    circBuffOverflow = RESET;
    CRITICAL_SECTION_END();
    i = 0;
  }

  while (charCount != 0)
  {
#if 0 /* echo On    */
    PRINTF("%c", circBuffer[ridx]);
#endif

    if (circBuffer[ridx] == AT_ERROR_RX_CHAR)
    {
      ridx++;
      if (ridx == CIRC_BUFF_SIZE)
      {
        ridx = 0;
      }
      CRITICAL_SECTION_BEGIN();
      charCount--;
      CRITICAL_SECTION_END();
      com_error(AT_RX_ERROR);
      i = 0;
    }
    else if ((circBuffer[ridx] == '\r') || (circBuffer[ridx] == '\n'))
    {
      ridx++;
      if (ridx == CIRC_BUFF_SIZE)
      {
        ridx = 0;
      }
      CRITICAL_SECTION_BEGIN();
      charCount--;
      CRITICAL_SECTION_END();

      if (i != 0)
      {
        command[i] = '\0';
        parse_cmd(command);
        i = 0;
      }
    }
    else if (i == (CMD_SIZE - 1))
    {
      i = 0;
      com_error(AT_TEST_PARAM_OVERFLOW);
    }
    else
    {
      command[i++] = circBuffer[ridx++];
      if (ridx == CIRC_BUFF_SIZE)
      {
        ridx = 0;
      }
      CRITICAL_SECTION_BEGIN();
      charCount--;
      CRITICAL_SECTION_END();
    }
  }
}

void com_error(ATEerror_t error_type)
{
  if (error_type > AT_MAX)
  {
    error_type = AT_MAX;
  }
  AT_PRINTF(ATError_description[error_type]);
}

/* Private functions ---------------------------------------------------------*/

static void parse_cmd(const char *cmd)
{
  ATEerror_t status = AT_OK;
  const struct ATCommand_s *Current_ATCommand;
  int i;

  if ((cmd[0] != 'A') || (cmd[1] != 'T'))
  {
    status = AT_ERROR;
  }
  else if (cmd[2] == '\0')
  {
    /* status = AT_OK; */
  }
  else if (cmd[2] == '?')
  {
#ifdef NO_HELP
#else
    PPRINTF("AT+<CMD>?        : Help on <CMD>\r\n"
            "AT+<CMD>         : Run <CMD>\r\n"
            "AT+<CMD>=<value> : Set the value\r\n"
            "AT+<CMD>=?       : Get the value\r\n");
    for (i = 0; i < (sizeof(ATCommand) / sizeof(struct ATCommand_s)); i++)
    {
      PPRINTF(ATCommand[i].help_string);
    }
    /* Wait for the message queue to be flushed in order
       not to disturb following com_error() display */
    DelayMs(HELP_DISPLAY_FLUSH_DELAY);
#endif
  }
  else
  {
    /* point to the start of the command, excluding AT */
    status = AT_ERROR;
    cmd += 2;
    for (i = 0; i < (sizeof(ATCommand) / sizeof(struct ATCommand_s)); i++)
    {
      if (strncmp(cmd, ATCommand[i].string, ATCommand[i].size_string) == 0)
      {
        Current_ATCommand = &(ATCommand[i]);
        /* point to the string after the command to parse it */
        cmd += Current_ATCommand->size_string;

        /* parse after the command */
        switch (cmd[0])
        {
          case '\0':    /* nothing after the command */
            status = Current_ATCommand->run(cmd);
            break;
          case '=':
            if ((cmd[1] == '?') && (cmd[2] == '\0'))
            {
              status = Current_ATCommand->get(cmd + 1);
            }
            else
            {
              status = Current_ATCommand->set(cmd + 1);
            }
            break;
          case '?':
#ifndef NO_HELP
            AT_PRINTF(Current_ATCommand->help_string);
#endif
            status = AT_OK;
            break;
          default:
            /* not recognized */
            break;
        }

        /* we end the loop as the command was found */
        break;
      }
    }
  }

  com_error(status);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
