/*******************************************************************************
 * @file    Lora_driver.c
 * @author  MCD Application Team
 * @version V1.1.2
 * @date    08-September-2017
 * @brief   LoRa module API
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
#include "hw.h"
#include "bsp.h"
#include "Lora_driver.h" 
#include "tiny_sscanf.h"
#include "low_power.h"
#include "timeServer.h"

/* External variables --------------------------------------------------------*/
ATCmd_t gFlagException = AT_END_AT;  
                         /*glogal flag to treat the return value of           */
                         /*Lora_GetFWVersion() function which is the only one */
                         /*that is not preceded by '+" charater. This flag is */
                         /*used in the at_cmd_receive(..) function            */


/* Private typedef -----------------------------------------------------------*/



/* Private variable ----------------------------------------------------------*/
#if USE_MDM32L07X01   
static TimerEvent_t JoinStatusDelayTimer; /*timer to handle the join status delay*/
#endif
static __IO uint8_t JoinTimeOutFlag = 0;  /*to indicate if timeout raised on Join procedure*/


/* Object definition for data to be sent to loRa application server*/
static char DataBinaryBuff[64];
#ifdef USE_MDM32L07X01
static sSendDataBinary_t SendDataBinary={ DataBinaryBuff, 0 , 0};
#elif USE_I_NUCLEO_LRWAN1
static sSendDataBinary_t SendDataBinary={ DataBinaryBuff, 0 , 0 ,0};

#endif

static LoRaDriverCallback_t *LoraDriverCallbacks;
static LoRaDriverParam_t *LoraDriverParam;

static uint8_t PtrValueFromDevice[32]  ;  /*to get back the device address in */
                                          /*11:22:33:44 format before*/
                                          /*to be translated into uint32_t type*/

static uint8_t PtrTempValueFromDevice[16]  ;  /*to get back the device address in */
                                          /*11:22:33;44:55:66:77 format before*/
                                          /*to be translated into and array of */
                                          /*8 uint8_t*/

static uint8_t PtrTempValueFromDeviceKey[47]  ;  /* in relation with the response size*/

#ifdef USE_I_NUCLEO_LRWAN1 
static uint8_t PtrDataFromNetwork[64]  ;      /* Payload size max returned by USI modem*/
#endif

static TimerEvent_t NextSensorMeasureTimer; /*timer to handle next sensor measure*/

static DeviceState_t DeviceState = DEVICE_INIT ;

static DeviceState_t DeviceSubState = DEVICE_INIT ; /* to manage the join transition*/

static TimerEvent_t DemoLedTimer;                  /*timer to handle Demo Led*/

__IO uint8_t SensorTimerWakeup = 0;

/* Private define ------------------------------------------------------------*/ 

  /******************************************************************************/
  /*                    To put USI modem in sleep mode                          */
  /*  From USI FW V2.6, modem sleep mode is only supported on ABP Join mode     */
  /*  From USI FW V3.0, modem sleep mode is supported for ABP and OTAA Join mode*/ 
  /******************************************************************************/

/*put in comment MODEM_IN_SLEEP_MODE defined in "hw_conf.h" */
/*if we do not want to have the USI MCU modem in sleep mode*/  

#if defined (USE_I_NUCLEO_LRWAN1) && defined (MODEM_IN_SLEEP_MODE)
#define USI_FW_V26               26
#define USI_FW_V30               30
#define STRING_USI_FW_V26       "2.6"
#define STRING_USI_FW_V30       "3.0"
static sPowerCtrlSet_t   PowerCtrlSettings= {0,1,0};
uint8_t FWVersion;
#endif


/* Private functions ---------------------------------------------------------*/

#if USE_MDM32L07X01 
static void Lora_OnJoinStatusDelayTimerEvt( void );
#endif

static void Lora_OnNextSensorMeasureTimerEvt( void );

static void Lora_OnLedTimerEvent( void );       /*timer function to handle Demo Led*/
/**************************************************************
 * @brief  Check if the LoRa module is working
 * @param  void
 * @retval status of the module (ready or not ready)
**************************************************************/
RetCode_t Lora_Init(void)
{
ATEerror_t Status;

  /*check if the module is working*/
  Status = Modem_AT_Cmd(AT_CTRL, AT, NULL );

  if (Status == AT_OK) 
  /* received Ok from module*/
    return (MODULE_READY);
  else
    return (MODULE_NO_READY);
}  


/**************************************************************
 * @brief  reset of the LoRa module
 * @param  void
 * @retval void
**************************************************************/
void Lora_Reset(void)
{
  /*reset the lora module*/
  Modem_AT_Cmd(AT_CTRL, AT_RESET, NULL );
  
}



/**************************************************************
 * @brief  Do a request to establish a LoRa Connection with the gateway
 * @param  Mode: by OTAA or by ABP
 * @retval LoRA return code
 * @Nota param is relevant for USI WM_SG_SM_XX modem - Not relevant for MDM32L07X01 modem
**************************************************************/
ATEerror_t Lora_Join(uint8_t Mode)
{
ATEerror_t Status = AT_END_ERROR;

            /******************************************************************/
            /* In OTAA mode wait JOIN_ACCEPT_DELAY1 cf. LoRaWAN Specification */
            /* MDM32L07X01:                                                   */
            /*      - After Join request waits DELAY_FOR_JOIN_STATUS_REQ      */
            /*      - Then do Join status request to know is nwk joined       */
            /* WM_SG_SM_XX:                                                   */
            /*      - Do the Join request                                     */
            /*      - Then waits synchronous JoinAccept event                 */
            /*      - if timeout raised before JoinAccept event               */
            /*      - then Join request Failed                                */
            /* Nota: Lora_Join() does the join request                        */
            /*       afterwhat                                                */
            /*       Lora_JoinAccept() does the waiting on return event       */ 
            /******************************************************************/

#ifdef USE_I_NUCLEO_LRWAN1 

  switch(Mode)
  {
    case ABP_JOIN_MODE:
      /*request a join connection*/
      Status = Modem_AT_Cmd(AT_SET, AT_JOIN, &Mode );
      break;
    case OTAA_JOIN_MODE: 
    {   
      Status = Modem_AT_Cmd(AT_SET, AT_JOIN, &Mode );                  
      /*HW_EnterSleepMode( );*/
      if(Status == AT_OK)
      {
        Status = AT_JOIN_SLEEP_TRANSITION;  /* to go in low power mode idle loop*/
      }         
      break;
    }  
    default:
      break;                           
  }
#elif USE_MDM32L07X01                
  /*request a join connection and whatever the mode DO waiting DELAY_FOR_JOIN_STATUS_REQ seconds*/
  Status = Modem_AT_Cmd(AT_CTRL, AT_JOIN, NULL );
  if (Status == AT_BUSY_ERROR){
    return Status;
  }
  else
  {  
    TimerInit( &JoinStatusDelayTimer, Lora_OnJoinStatusDelayTimerEvt); 
    /*Set and start the Join status requesttimeout */
    TimerSetValue( &JoinStatusDelayTimer, DELAY_FOR_JOIN_STATUS_REQ);
    TimerStart( &JoinStatusDelayTimer ); 
    Status = AT_JOIN_SLEEP_TRANSITION;
  }
#endif         
 
  return(Status);
}



/**************************************************************
 * @brief  Wait for join accept notification either in ABP or OTAA
 * @param  void
 * @retval LoRA return code
 * @Nota this function supports either USI protocol or MDM32L07X01 protocol
**************************************************************/
ATEerror_t Lora_JoinAccept(void)
{
ATEerror_t Status = AT_END_ERROR;
 
#ifdef USE_I_NUCLEO_LRWAN1   
  /*trap the asynchronous accept event (OTAA mode) coming from USI modem*/
  Status = Modem_AT_Cmd(AT_ASYNC_EVENT, AT_JOIN, NULL );  
#elif USE_MDM32L07X01 
uint8_t JoinStatus = 0;      
  /*trap the return code of the join request procedure*/
  if(JoinTimeOutFlag)
  {
    TimerStop( &JoinStatusDelayTimer ); 
    JoinTimeOutFlag = RESET;                     
    /*request the join network status*/
    Status = Modem_AT_Cmd(AT_GET, AT_NJS, &JoinStatus );
    if(Status == AT_OK)
    {
    if (JoinStatus)       /*LoRa Nwk joined*/
      return Status;
    }  
    else
    {                   /*LoRa nwk not joined*/
    return  AT_NO_NET_JOINED;
    }
  }    
#endif  
  return (Status);
  
}



/**************************************************************
 * @brief  Do a request to set the Network join Mode
 * @param  Mode : OTA, ABP
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetJoinMode(uint8_t Mode)
{
ATEerror_t Status;  
   
  /*Set the nwk Join mode */
  Status = Modem_AT_Cmd(AT_SET, AT_NJM, &Mode );
  return(Status);
}


/**************************************************************
 * @brief  Do a request to get the Network join Mode
 * @param  pointer to the Join mode out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetJoinMode(uint8_t *Mode)
{  
ATEerror_t Status;  
  
  /*Get the nwk Join mode */
  Status = Modem_AT_Cmd(AT_GET, AT_NJM, Mode );
  return(Status);   
}


            /********* MiB MananagementLora **************/

/**************************************************************
 * @brief  key configuration
 * @param  KeyType : APPKEY, NWKSKE, APPSKEY
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_SetKey(ATCmd_t KeyType, uint8_t *PtrKey)
{
ATEerror_t Status;  
    
  /*Set a key type to the LoRa device*/
  Status = Modem_AT_Cmd(AT_SET, KeyType, PtrKey );
  return(Status); 
}



/**************************************************************
 * @brief  Request the key type configuration
 * @param  KeyType : APPKEY, NWKSKE, APPSKEY
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetKey(ATCmd_t KeyType,uint8_t *PtrKey)
{
ATEerror_t Status;  

  /*get the key type from the LoRa device*/
    Status = Modem_AT_Cmd(AT_GET, KeyType, PtrTempValueFromDeviceKey );
  if (Status == 0)
  {  
    AT_VSSCANF((char*)PtrTempValueFromDeviceKey+AT_FRAME_KEY_OFFSET, AT_FRAME_KEY,
    &PtrKey[0], &PtrKey[1], &PtrKey[2], &PtrKey[3],
    &PtrKey[4], &PtrKey[5], &PtrKey[6], &PtrKey[7],    
    &PtrKey[8], &PtrKey[9], &PtrKey[10], &PtrKey[11], 
    &PtrKey[12], &PtrKey[13], &PtrKey[14], &PtrKey[15]);                
    return (Status);
  }    
  else
    return (Status);
}


/**************************************************************
 * @brief  Set the Application Identifier
 * @param  pointer to the APPEUI in value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_SetAppID(uint8_t *PtrAppID)
{
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_SET, AT_APPEUI, PtrAppID );
  return(Status);
}


/**************************************************************
 * @brief  Request the Application Identifier
 * @param  pointer to the APPEUI out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetAppID(uint8_t *AppEui)
{
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_GET, AT_APPEUI, PtrTempValueFromDevice );
  if (Status == 0)
  {  
    AT_VSSCANF((char*)PtrTempValueFromDevice, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
    &AppEui[0], &AppEui[1], &AppEui[2], &AppEui[3],
    &AppEui[4], &AppEui[5], &AppEui[6], &AppEui[7]);     
    return (Status);
  }    
  else
    return (Status);
}


/**************************************************************
 * @brief  Set the device extended universal indentifier
 * @param  pointer to the DEUI in value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_SetDeviceID(uint8_t *PtrDeviceID)
{
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_SET, AT_DEUI, PtrDeviceID );
  return(Status);
}

/**************************************************************
 * @brief  Request the device extended universal indentifier
 * @param  pointer to the DEUI out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetDeviceID(uint8_t *PtrDeviceID)
{
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_GET, AT_DEUI, PtrTempValueFromDevice );
  if (Status == 0)
  {  
    AT_VSSCANF((char*)PtrTempValueFromDevice, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
    &PtrDeviceID[0], &PtrDeviceID[1], &PtrDeviceID[2], &PtrDeviceID[3],
    &PtrDeviceID[4], &PtrDeviceID[5], &PtrDeviceID[6], &PtrDeviceID[7]);     
    return (Status);
  } 
  else
    return (Status);
}



/**************************************************************
 * @brief  Set the device address
 * @param  pointer to the DADDR in value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_SetDeviceAddress(uint32_t DeviceAddr)
{
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_SET, AT_DADDR, &DeviceAddr );
  return(Status);
}


/**************************************************************
 * @brief  Request the device address
 * @param  pointer to the DADDR out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetDeviceAddress(uint32_t *Value)
{
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_GET, AT_DADDR, PtrValueFromDevice );
  if (Status == 0)
  {  
      AT_VSSCANF((char*)PtrValueFromDevice, "%hhx:%hhx:%hhx:%hhx",
      &((unsigned char *)(Value))[3],
      &((unsigned char *)(Value))[2],
      &((unsigned char *)(Value))[1],
      &((unsigned char *)(Value))[0]);     
      return (Status);
  }    
  else
      return (Status);
}


/**************************************************************
 * @brief  Set the NetWork ID
 * @param  NWKID in value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_SetNetworkID(uint32_t NetworkID)
{
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_SET, AT_NWKID, &NetworkID );
  return(Status);
}


/**************************************************************
 * @brief  Request the network ID
 * @param  pointer to the NWKID out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetNetworkID(uint32_t *Value)
{
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_GET, AT_NWKID, PtrValueFromDevice );
  if (Status == 0)
  {  
      AT_VSSCANF((char*)PtrValueFromDevice, "%hhx:%hhx:%hhx:%hhx",
      &((unsigned char *)(Value))[0],
      &((unsigned char *)(Value))[1],
      &((unsigned char *)(Value))[2],
      &((unsigned char *)(Value))[3]);     
      return (Status);
  }    
  else
      return (Status);
}


         /*************   Network Management *****************/

/**************************************************************
 * @brief  Do a request to set the adaptive data rate
 * @param  ADR in value 0(off) / 1(on)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetAdaptiveDataRate(uint8_t Rate)
{
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_SET, AT_ADR, &Rate );
  return(Status);  
}


/**************************************************************
 * @brief  Do a request to get the adaptive data rate
 * @param  pointer to ADR out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetAdaptiveDataRate(uint8_t *Rate)
{  
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_GET, AT_ADR, Rate );
  return(Status);;    
}


/**************************************************************
 * @brief  Do a request to set the LoRa Class
 * @param  CLASS in value [0,1,2]
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetClass(uint8_t Class)
{
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_SET, AT_CLASS, &Class );
  return(Status);
}


/**************************************************************
 * @brief  Do a request to get the LoRa class
 * @param  pointer to CLASS out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetClass(uint8_t *Class)
{  
ATEerror_t Status;  
  
  Status = Modem_AT_Cmd(AT_GET, AT_CLASS, Class );
  return(Status);    
}


/**************************************************************
 * @brief  Do a request to set the duty cycle
 * @brief  only used in test mode
 * @param  DCS in value 0(disable) / 1(enable)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetDutyCycle(uint8_t DutyCycle)
{
ATEerror_t Status;  
    
  Status = Modem_AT_Cmd(AT_SET, AT_DCS, &DutyCycle );
  return(Status); 
}


/**************************************************************
 * @brief  Do a request to get the duty cycle
 * @brief  only used in test mode
 * @param  pointer to DCS out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetDutyCycle(uint8_t *DutyCycle)
{  
ATEerror_t Status;  
  
  Status = Modem_AT_Cmd(AT_GET, AT_DCS, DutyCycle );
  return(Status);    
}


/**************************************************************
 * @brief  Do a request to set the data Rate
 * @param  DR in value [0,1,2,3,4,5,6,7]
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetDataRate(uint8_t DataRate)
{
ATEerror_t Status;  
    
  Status = Modem_AT_Cmd(AT_SET, AT_DR, &DataRate );
  return(Status);
}


/**************************************************************
 * @brief  Do a request to get the data Rate
 * @param  pointer to DR out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetDataRate(uint8_t *DataRate)
{  
ATEerror_t Status;  
  
  Status = Modem_AT_Cmd(AT_GET, AT_DR, DataRate );
  return(Status);  
}


/**************************************************************
 * @brief  Do a request to set the frame counter
 * @param  FrameCounterType : FCD, FCU
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_SetFrameCounter(ATCmd_t FrameCounterType, uint32_t FrameCounternumber)
{
ATEerror_t Status;  
    
  Status = Modem_AT_Cmd(AT_SET, FrameCounterType, &FrameCounternumber );
  return(Status);
  
}



/**************************************************************
 * @brief  Request the frame counter number
 * @param  frameCounterType : FCD, FCU
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetFrameCounter(ATCmd_t FrameCounterType,uint32_t *FrameCounternumber)
{
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_GET, FrameCounterType, PtrValueFromDevice );
  if (Status == 0)
  {  
    AT_VSSCANF((char*)PtrValueFromDevice, "%lu",FrameCounternumber);                    
    return (Status);
  }    
  else
    return (Status);
}


/**************************************************************
 * @brief  Do a request to set the join accept delay between
 * @brief  the end of the Tx and the join Rx#n window
 * @param  RxWindowType : JN1DL, JN2DL
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_SetJoinDelayRxWind(ATCmd_t RxWindowType, uint32_t JoinDelayInMs)
{
ATEerror_t Status;  
    
  Status = Modem_AT_Cmd(AT_SET, RxWindowType, &JoinDelayInMs );
  return(Status);
  
}



/**************************************************************
 * @brief  Do a request to get the join accept delay between
 * @brief  the end of the Tx and the join Rx#n window
 * @param  RxWindowType : JN1DL, JN2DL
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetJoinDelayRxWind(ATCmd_t RxWindowType,uint32_t *JoinDelayInMs)
{
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_GET, RxWindowType, PtrValueFromDevice );
  if (Status == 0)
  {  
    AT_VSSCANF((char*)PtrValueFromDevice, "%lu",JoinDelayInMs);                    
    return (Status);
  }    
  else
    return (Status);
}


/**************************************************************
 * @brief  Do a request to set the Public Network mode
 * @param  PNM in value 0(off) / 1(on)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetPublicNetworkMode(uint8_t NetworkMode)
{
ATEerror_t Status;  
    
  Status = Modem_AT_Cmd(AT_SET, AT_PNM, &NetworkMode );
  return(Status);
}


/**************************************************************
 * @brief  Do a request to get the Public Network mode
 * @param  pointer to PNM out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetPublicNetworkMode(uint8_t *NetworkMode)
{  
ATEerror_t Status;  
  
  Status = Modem_AT_Cmd(AT_GET, AT_PNM, NetworkMode );
  return(Status);    
}


/**************************************************************
 * @brief  Do a request to set the delay between the end of the Tx
 * @brief  the end of the Tx and the join Rx#n window
 * @param  RxWindowType : RX1DL, RX2DL
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_SetDelayRxWind(ATCmd_t RxWindowType, uint32_t RxDelayInMs)
{
ATEerror_t Status;  
    
  Status = Modem_AT_Cmd(AT_SET, RxWindowType, &RxDelayInMs );
  return(Status);
  
}



/**************************************************************
 * @brief  Do a request to get the delay between the end of the Tx
 * @brief  the end of the Tx and the join Rx#n window
 * @param  RxWindowType : RX1DL, RX2DL
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetDelayRxWind(ATCmd_t RxWindowType,uint32_t *RxDelayInMs)
{
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_GET, RxWindowType, PtrValueFromDevice );
  if (Status == 0)
  {  
    AT_VSSCANF((char*)PtrValueFromDevice, "%lu",RxDelayInMs);                    
    return (Status);
  }    
  else
    return (Status);
}



/**************************************************************
 * @brief  Set the frequency of the Rx2 window
 * @param  pointer to the RX2FQ in value
 * @retval LoRa return code
************************************************************
**/
ATEerror_t LoRa_SetFreqRxWind2(uint32_t Rx2WindFrequency)
{
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_SET, AT_RX2FQ, &Rx2WindFrequency );
  return(Status);
}


/**************************************************************
 * @brief  Request the frequency of the Rx2 window
 * @param  pointer to the RX2FQ out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetFreqRxWind2(uint32_t *Rx2WindFrequency)
{
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_GET, AT_RX2FQ, PtrValueFromDevice );
  if (Status == 0)
  {  
    AT_VSSCANF((char*)PtrValueFromDevice, "%lu",Rx2WindFrequency);     
    return (Status);
  }    
  else
   return (Status);
}


/**************************************************************
 * @brief  Do a request to set the transmit Tx power
 * @param  TXP in value [0,1,2,3,4,5]
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetTxPower(uint8_t TransmitTxPower)
{
ATEerror_t Status;  
    
  Status = Modem_AT_Cmd(AT_SET, AT_TXP, &TransmitTxPower );
  return(Status);
}


/**************************************************************
 * @brief  Do a request to get the transmit Tx Power
 * @param  pointer to TXP out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetTxPower(uint8_t *TransmitTxPower)
{  
ATEerror_t Status;  
  
  Status = Modem_AT_Cmd(AT_GET, AT_TXP, TransmitTxPower );
  return(Status);  
}


/**************************************************************
 * @brief  Do a request to set the data Rate of Rx2 window
 * @param  RX2DR in value [0,1,2,3,4,5,6,7]
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetDataRateRxWind2(uint8_t DataRateRxWind2)
{
ATEerror_t Status;  
    
  Status = Modem_AT_Cmd(AT_SET, AT_RX2DR, &DataRateRxWind2 );
  return(Status);
}


/**************************************************************
 * @brief  Do a request to get the data Rate of Rx2 window
 * @param  pointer to RX2DR out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetDataRateRxWind2(uint8_t *DataRateRxWind2)
{  
ATEerror_t Status;  
  
  Status = Modem_AT_Cmd(AT_GET, AT_RX2DR, DataRateRxWind2 );
  return(Status);   
}


      /************ Data Path Management ***************/

/**************************************************************
 * @brief  Send text data to a giving prot number
 * @param  SEND in value 
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SendData(sSendDataString_t *PtrStructData)
{
ATEerror_t Status;  
    
  Status = Modem_AT_Cmd(AT_SET, AT_SEND, PtrStructData );
  return(Status); 
}


/**************************************************************
 * @brief  Do a request to get the last data (in raw format) 
 * @brief  received by the Slave
 * @param  pointer to RECV out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_ReceivedData(sReceivedDataString_t *PtrStructData)
{
ATEerror_t Status;  
uint8_t sizebuf;
  
  
  Status = Modem_AT_Cmd(AT_GET, AT_RECV, PtrValueFromDevice );
  if (Status == 0)
  {     
    AT_VSSCANF((char*)PtrValueFromDevice, "%d",&(PtrStructData->Port));                      
    if ((sizebuf=strlen((char*)&PtrValueFromDevice[3])) > DATA_RX_MAX_BUFF_SIZE)     /*shrink the Rx buffer to MAX size*/
    sizebuf = DATA_RX_MAX_BUFF_SIZE -1;
    memcpy1(PtrStructData->Buffer, (uint8_t *)&PtrValueFromDevice[3], sizebuf+1);       
    return (Status);
  }    
  else
    return (Status);
}

#if USE_I_NUCLEO_LRWAN1 
/**************************************************************
 * @brief  Trap an asynchronous event coming from external modem (only USI device)
 * @param  Pointer to RCV out value if any
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_AsyncDownLinkData(sReceivedDataBinary_t *PtrStructData)  
{
ATEerror_t Status;
uint8_t sizebuf;
char *ptrChr;

    Status = Modem_AT_Cmd(AT_ASYNC_EVENT, AT_END_AT, PtrDataFromNetwork);
  if (Status == 0)
  {     
    AT_VSSCANF((char*)PtrDataFromNetwork, "%d,%2d",&(PtrStructData->Port),&(PtrStructData->DataSize));  
    ptrChr = strchr((strchr((char*)&PtrDataFromNetwork[0],',')+1),',');  /*search the second ',' occurence in the return string*/ 
    if ((sizebuf=strlen((char*)ptrChr+1)) > DATA_RX_MAX_BUFF_SIZE)     /*shrink the Rx buffer to MAX size*/
    sizebuf = DATA_RX_MAX_BUFF_SIZE -1;
    memcpy1(PtrStructData->Buffer, (uint8_t *)ptrChr+1, sizebuf-1); 
    *(PtrStructData->Buffer+sizebuf-1) ='\0';
    return (Status);
  }    
  else
    return (Status);
}
#endif          

/**************************************************************
 * @brief  Send binary data to a giving port number
 * @param  SENDB in value ( USE_MDM32L07X01) SEND in value ( USE_I_NUCLEO_LRWAN1)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SendDataBin(sSendDataBinary_t *PtrStructData)
{
ATEerror_t Status;  
    
#ifdef USE_MDM32L07X01 
  Status = Modem_AT_Cmd(AT_SET, AT_SENDB, PtrStructData );
#elif USE_I_NUCLEO_LRWAN1
  Status = Modem_AT_Cmd(AT_SET, AT_SEND, PtrStructData );
#endif            
  return(Status); 
}



/**************************************************************
 * @brief  Do a request to get the last data (in binary format) 
 * @brief  received by the Slave
 * @param  pointer to RECVB out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_ReceivedDataBin(sReceivedDataBinary_t *PtrStructData)
{
ATEerror_t Status;  
uint8_t sizebuf;
uint8_t i;
char TempBuf[3] ={0};


  Status = Modem_AT_Cmd(AT_GET, AT_RECVB, PtrValueFromDevice );
  if (Status == 0)
  {     
    AT_VSSCANF((char*)PtrValueFromDevice, "%d",&(PtrStructData->Port)); 
    
    if ((sizebuf= strlen((char*)&PtrValueFromDevice[3])) > DATA_RX_MAX_BUFF_SIZE)     /*shrink the Rx buffer to MAX size*/
    sizebuf = DATA_RX_MAX_BUFF_SIZE;
    
    for(i=0;i<=((sizebuf/2)-1);i++)
    {
      TempBuf[0] = PtrValueFromDevice[3+(i*2)];
      TempBuf[1] = PtrValueFromDevice[3+(i*2)+1];
      AT_VSSCANF(TempBuf,"%hhx",  &PtrStructData->Buffer[i]);
    }  
    PtrStructData->DataSize = i;   
    return (Status);
  }    
  else
    return (Status);
}


/**************************************************************
 * @brief  Do a request to set the confirmation mode
 * @param  CFM in value 0(unconfirm) / 1(confirm)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetSendMsgConfirm(uint8_t ConfirmMode)
{
ATEerror_t Status;  
  
  Status = Modem_AT_Cmd(AT_SET, AT_CFM, &ConfirmMode );
  return(Status);
}


/**************************************************************
 * @brief  Do a request to get the confirmation mode
 * @param  pointer to CFM out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetSendMsgConfirm(uint8_t *ConfirmMode)
{  
ATEerror_t Status;  
 
  Status = Modem_AT_Cmd(AT_GET, AT_CFM, ConfirmMode );
  return(Status);  
}


/**************************************************************
 * @brief  Do a request to get the msg status of the last send cmd
 * @param  CFS in value 0(unconfirm) / 1(confirm)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetSendMsgStatus(uint8_t *MsgStatus)
{
ATEerror_t Status;  
   
  Status = Modem_AT_Cmd(AT_GET, AT_CFS, MsgStatus );
  return(Status);
}


/**************************************************************
 * @brief  Do a request to get the battery level of the modem (slave)
 * @param  BAT in value  
 *              0:    battery connected to external power supply
 *       [1..254]:    1 being at minimum and 254 being at maximum
*             255:    not able to measure the battery level
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetBatLevel(uint32_t *BatLevel)
{
ATEerror_t Status;  


  Status = Modem_AT_Cmd(AT_GET, AT_BAT, PtrValueFromDevice );
  if (Status == 0)
  {  
    AT_VSSCANF((char*)PtrValueFromDevice, "%ld",BatLevel);     
    return (Status);
  }    
  else
    return (Status);
} 

/**************************************************************
 * @brief  Do a request to get the RSSI of the last received packet
 * @param  RSSI in value [in dbm] 
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetRSSI(int32_t *SigStrengthInd)
{
ATEerror_t Status;  


  Status = Modem_AT_Cmd(AT_GET, AT_RSSI, PtrValueFromDevice );
  if (Status == 0)
  {  
    AT_VSSCANF((char*)PtrValueFromDevice, "%ld",SigStrengthInd);     
    return (Status);
  }    
  else
    return (Status);
} 


/**************************************************************
 * @brief  Do a request to get the SNR of the last received packet
 * @param  SNR in value [in db]  
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetSNR(uint32_t *SigToNoice)
{
ATEerror_t Status;  


  Status = Modem_AT_Cmd(AT_GET, AT_SNR, PtrValueFromDevice );
  if (Status == 0)
  {  
    AT_VSSCANF((char*)PtrValueFromDevice, "%ld",SigToNoice);     
    return (Status);
  }    
  else
    return (Status);
} 






/**************************************************************
 * @brief  Do a request to get the LoRa stack version of the modem (slave) 
 * @param  pointer to VER out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetVersion(uint8_t *PtrVersion)
{
ATEerror_t Status;  

  Status = Modem_AT_Cmd(AT_GET, AT_VER, PtrValueFromDevice );
  if (Status == 0)
  { 
#if USE_MDM32L07X01 
uint8_t sizebuf;                  
    if ((sizebuf=strlen((char*)PtrValueFromDevice)) > DATA_RX_MAX_BUFF_SIZE)     /*shrink the Rx buffer to MAX size*/
    sizebuf = DATA_RX_MAX_BUFF_SIZE -1;
    
    memcpy1(PtrVersion, (uint8_t *)&PtrValueFromDevice[0], sizebuf);  
    return (Status);
#endif
                  
#if USE_I_NUCLEO_LRWAN1 
char *ptrChr;                  
    ptrChr = strchr((char *)&PtrValueFromDevice[0],'v');       /*to skip the "LoRaWAN v"*/
    strcpy((char*)PtrVersion,ptrChr+1);
    return (Status);
#endif                  
  }    
  else
    return (Status);
}


#if USE_I_NUCLEO_LRWAN1  
/**************************************************************
 * @brief  Do a request to get the firmware version of the modem (slave) 
 * @param  pointer to FWVERSION out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetFWVersion(uint8_t *PtrFWVersion)
{
ATEerror_t Status;  
char *ptrChr;

  gFlagException = AT_FWVERSION;

  Status = Modem_AT_Cmd(AT_GET, AT_FWVERSION, PtrValueFromDevice );
  if (Status == 0)
  {                       
    /*to skip the "USI Lora Module Firmware V" prefix*/
    ptrChr = strchr((char *)&PtrValueFromDevice[0],'V');  
    strcpy((char*)PtrFWVersion,ptrChr+1);
                    
    return (Status);
   
  }    
  else
    return (Status);
}



/**************************************************************
 * @brief  Do a request to set the country band code for LoRaWAN
 * @brief  Need to write to DCT and Reset module to enable this setting
 * @param  BAND in value 0(EU-868 Band) / 1(US-Band)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetDeviceBand(uint8_t DeviceBand)
{
ATEerror_t Status;  
    
  Status = Modem_AT_Cmd(AT_SET, AT_BAND, &DeviceBand );
  return(Status); 
}


/**************************************************************
 * @brief  Do a request to get the country band code for LoRaWAN
 * @brief  only used in test mode
 * @param  pointer to BAND out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetDeviceBand(uint8_t *DeviceBand)
{  
ATEerror_t Status;  
  
  Status = Modem_AT_Cmd(AT_GET, AT_BAND, DeviceBand );
  return(Status);    
}


#endif 

      /************ Power Control Commands (for USI board) ***************/

#if USE_I_NUCLEO_LRWAN1
/**************************************************************
 * @brief  Do a request to enter the slave in sleep (MCU STOP mode) 
 * @param  Void
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SleepMode(void)
{
ATEerror_t Status;

  Status = Modem_AT_Cmd(AT_EXCEPT_1, AT_SLEEP, PtrValueFromDevice );     /* under building*/
  return(Status);
}



/**************************************************************
 * @brief  Do a request to set the power control settings of the MCU (slave)
 * @param  Power control IN value 
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetMCUPowerCtrl(sPowerCtrlSet_t *PtrStructData)
{
ATEerror_t Status;  
    
  Status = Modem_AT_Cmd(AT_SET, AT_PS, PtrStructData );
  return(Status); 
}



/**************************************************************
 * @brief  Do a Dumy request to resynchronize the Host and the modem
 * @note   A simple AT cmd where we do not trap the return code 
 * @param  void 
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_DumyRequest(void)
{
ATEerror_t Status;
uint8_t i;
  
  for (i=0; i<=1; i++) 
  {
    /*first iteration to wake-up the mdem*/
    /*second iteration to align Host-Modem interface*/ 
    Status = Modem_AT_Cmd(AT_EXCEPT_1, AT, NULL );
    /*assumption: to be sure that modem is ready*/
    HAL_Delay(1000);
  }    
  return(Status); 
}



/**************************************************************
 * @brief  Do a request to restore DCT content table with default values
 * @param  void
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_RestoreConfigTable(void)
{
ATEerror_t Status;
uint8_t Restore = RESET;
    
  Status = Modem_AT_Cmd(AT_SET, AT_WDCT, &Restore );
  return(Status); 
}


/**************************************************************
 * @brief  Do a request to update the DCT content table with new values
 * @param  void
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_UpdateConfigTable(void)
{  
ATEerror_t Status;  
  
  Status = Modem_AT_Cmd(AT_GET, AT_WDCT, NULL );
  return(Status);    
}

#endif




      /*********** Finate State Machine for LoRa Modem mangement *************/

/******************************************************************************
  * @Brief lora Modem state machine
  * @param void
  * @retval None   
******************************************************************************/
void Lora_fsm( void)
{
RetCode_t LoraModuleRetCode; 
ATEerror_t LoraCmdRetCode;


  switch( DeviceState )
  {
    case DEVICE_INIT:
    {
       /* Check if the LoRa Modem is ready to work*/

       LoraModuleRetCode = Lora_Init();          
   
       if (LoraModuleRetCode == MODULE_READY)
       {  
          DeviceState = DEVICE_READY; 
          DBG_PRINTF("Lora module ready\n");
 
#if USE_MDM32L07X01
          /*Set the modem Join mode following application set-up*/
          LoraCmdRetCode = Lora_SetJoinMode(LoraDriverParam->JoinMode);
#endif
          
#if defined (USE_I_NUCLEO_LRWAN1) && defined (MODEM_IN_SLEEP_MODE)
          /*Set the low power control - Modem MCU will enter stop mode if get command at+sleep */
          if(((JOIN_MODE == ABP_JOIN_MODE) && (FWVersion == USI_FW_V26)) || (FWVersion == USI_FW_V30))
          LoraCmdRetCode = Lora_SetMCUPowerCtrl(&PowerCtrlSettings);
#endif          
            
          /*to adapt the data rate during transmission*/
          LoraCmdRetCode = Lora_SetAdaptiveDataRate(ADAPT_DATA_RATE_ENABLE); 
          
          /* Timer for sensor occurence measure*/
          TimerInit( &NextSensorMeasureTimer, Lora_OnNextSensorMeasureTimerEvt );
       }   
       else
       {  
          DBG_PRINTF("Lora module not ready\n");   /*we stay in Init state and redo Lora_Init*/
       }
       break;
    }
    case DEVICE_READY:
    {
      /* Do a join request to the LoRaWAN network - can be ABP or OTAA  Join mode*/
      /* Nota : Join Mode parameter relevant for USI modem - For Murata not releavant cf.User manual*/


#ifdef USE_I_NUCLEO_LRWAN1             /* Led on Modem slave device to indicate a Join request*/   
      BSP_LED_Modem_On(LED_GREEN);
#elif USE_MDM32L07X01                  /* Led on Nucleo Master device to indicate a Join request*/
      BSP_LED_On(LED2);
#endif  
      
      TimerInit( &DemoLedTimer,  Lora_OnLedTimerEvent);  
      TimerSetValue( &DemoLedTimer,  200);  
      TimerStart( &DemoLedTimer);          
      
      LoraCmdRetCode = Lora_Join(LoraDriverParam->JoinMode);   
      switch(LoraCmdRetCode)
      {
        case AT_OK:
        {
          DeviceState = DEVICE_JOINED;          
          DBG_PRINTF("Nwk Joined\n");
          break;
        }  
        case AT_JOIN_SLEEP_TRANSITION:    /*waiting the return fron modem*/
        {
          DeviceState = DEVICE_SLEEP; 
          DeviceSubState = DEVICE_JOIN_ON_GOING;
          break;
        }  
        default:
          DBG_PRINTF("Nwk Not Joined\n");   /* we stay in ready state and redo LoRa_Join*/
          break;
      }
      break;
    } 
    case DEVICE_JOINED:      
    {
       DBG_PRINTF("Nwk Joined - waiting\n");        
      /*Schedule the first mesaure */
      TimerSetValue( &NextSensorMeasureTimer, LoraDriverParam->SensorCycleMeasure /*SENSORS_MEASURE_CYCLE*/);      /* every #n ms*/
      TimerStart( &NextSensorMeasureTimer ); 
      
#if defined (USE_I_NUCLEO_LRWAN1) && defined (MODEM_IN_SLEEP_MODE)       
      /*put the modem MCU in stop mode*/
      if(((JOIN_MODE == ABP_JOIN_MODE) && (FWVersion == USI_FW_V26)) || (FWVersion == USI_FW_V30))
      LoraCmdRetCode = Lora_SleepMode();      
#endif      
      DeviceState = DEVICE_SLEEP;  
      break;
    }    
    case DEVICE_SLEEP:
    {
      /* Wake up through RTC events or asynchronous event coming from modem*/
      if (DeviceSubState == DEVICE_JOIN_ON_GOING)
      {
        LoraCmdRetCode = Lora_JoinAccept();
        if (LoraCmdRetCode == AT_OK)
        {  
          DeviceState = DEVICE_JOINED; 
          DeviceSubState = DEVICE_INIT;  /* Reset the substate. We are Joined*/    
          DBG_PRINTF("Nwk Joined\n");         
        }  
        else
        {  
          DeviceState = DEVICE_READY;         
          DBG_PRINTF("Nwk Not Joined\n");   /* we stay in ready state and redo LoRa_Join*/
        }          
      }  
      break;
    }    
    case DEVICE_SEND:
    {

#if defined (USE_I_NUCLEO_LRWAN1)     /*only applicable for USI LoRA modem*/ 
      if(SensorTimerWakeup)  /*the wakeup comes from Timer*/
      {      
        SensorTimerWakeup = 0;
#endif         
         
#if defined (USE_I_NUCLEO_LRWAN1) && defined (MODEM_IN_SLEEP_MODE)
        /*Dumy Cmd after wakeup to resynchronize host/modem*/
        if(((JOIN_MODE == ABP_JOIN_MODE) && (FWVersion == USI_FW_V26)) || (FWVersion == USI_FW_V30))      
        LoRa_DumyRequest();
#endif 
              
        /*Sensor reading on slave device*/
        LoraDriverCallbacks->SensorMeasureData(&SendDataBinary);

#ifdef USE_I_NUCLEO_LRWAN1              /* Led on Modem slave device to indicate a send request*/
        BSP_LED_Modem_On(LED_GREEN);       
#elif USE_MDM32L07X01                   /* Led on Nucleo Master device to indicate a send request*/
        BSP_LED_On(LED2);        
#endif        
 
        TimerInit( &DemoLedTimer,  Lora_OnLedTimerEvent);  
        TimerSetValue( &DemoLedTimer,  200);  
        TimerStart( &DemoLedTimer);         

        /*Send data to Slave device  */        
        LoraCmdRetCode = Lora_SendDataBin(&SendDataBinary);
        if (LoraCmdRetCode == AT_OK)
          DBG_PRINTF("Data binary send on port = %d --> OK\n",SendDataBinary.Port);
        else
          DBG_PRINTF("Data binary Send on port = %d --> KO\n",SendDataBinary.Port);  
      
        /*Schedule the next measure */
        TimerSetValue( &NextSensorMeasureTimer,  LoraDriverParam->SensorCycleMeasure/*SENSORS_MEASURE_CYCLE*/);
        TimerStart( &NextSensorMeasureTimer ); 
      
#if defined (USE_I_NUCLEO_LRWAN1) && defined (MODEM_IN_SLEEP_MODE)          
        /*put the modem MCU in stop mode*/
        if(((JOIN_MODE == ABP_JOIN_MODE) && (FWVersion == USI_FW_V26)) || (FWVersion == USI_FW_V30))        
          LoraCmdRetCode = Lora_SleepMode();        
#endif 
        
#if defined (USE_I_NUCLEO_LRWAN1)     /*only applicable for USI LoRA modem*/         
      } /*end of SensorTimerWakeup*/
      else  /*the wakeup comes from network Event downlink data*/
      {

uint8_t ReceivedDownlinkData[64];  
sReceivedDataBinary_t StructDownLinkData ={ ReceivedDownlinkData, 0 , 0}; 


        LoraCmdRetCode = Lora_AsyncDownLinkData(&StructDownLinkData);
        DBG_PRINTF("downlink data\n");
        /************************************************************************/
        /* following the use case,                                              */
        /*   - switch on LED                                                    */
        /*   - activator setting                                                */
        /*   - LoRa mdeom setting                                               */
        /*   - .................                                                */
        /* user has to manage the received donwlink data                        */
        /************************************************************************/
      } 
#endif        
        
      DeviceState = DEVICE_SLEEP;
      break;
    }     
    default:
    {
      DeviceState = DEVICE_INIT;
      break;
    }
  }   
}  



/******************************************************************************
  * @Brief Context InitialModem following the lora device modem used
  * @param Periode to do sensors measurement
  * @retval None   
******************************************************************************/
void Lora_Ctx_Init(LoRaDriverCallback_t  *PtrLoRaDriverCallbacks, 
                              LoRaDriverParam_t *PtrLoRaDriverParam)
{
  
  /* init the main call backs*/
  LoraDriverCallbacks = PtrLoRaDriverCallbacks; 
  LoraDriverParam = PtrLoRaDriverParam;
  
#ifdef USE_I_NUCLEO_LRWAN1  
  BSP_LED_Modem_Init(LED_GREEN);   /*Led indicator on Modem slave device*/
#elif USE_MDM32L07X01    
  BSP_LED_Init(LED2);              /*Led indicator on Nucleo master board*/
#endif 


  /*Do a Modem reset in a hidden way in order to synchronize Host and Modem*/
  /*Lora_Reset();*/

  /***************************************************************************/
  /*          disabling the echo mode which is by default active             */
  /*        from USI FW V2.6, echo mode will be disable by default.          */
  /*        Then this line could be removed                                  */
  /***************************************************************************/
  
#ifdef USE_I_NUCLEO_LRWAN1 
uint8_t   Mode = 0; 
  Modem_AT_Cmd(AT_EXCEPT, AT_ATE, &Mode );
#endif   
   
   
  /***************************************************************************/
  /*    To discriminate the FW version where Sleep mode works correctly      */
  /***************************************************************************/   
#if defined (USE_I_NUCLEO_LRWAN1) && defined (MODEM_IN_SLEEP_MODE)
uint8_t   RetCode;
uint8_t PtrFWVersion[6];

  Lora_GetFWVersion(PtrFWVersion);   
  RetCode = strncmp((char*)PtrFWVersion, STRING_USI_FW_V26, sizeof(STRING_USI_FW_V26)-1);   
  if (RetCode == 0)
    FWVersion = USI_FW_V26;    /* FW version where OTAA bug prevents the sleep mode (MCU slave stop mode)*/
  else
    FWVersion = USI_FW_V30;    /* FW version where OTAA bug has been fixed*/
#endif
  
}

/******************************************************************************
  * @Brief get the current device finate state
  * @param void
  * @retval deviceState  
******************************************************************************/
DeviceState_t lora_getDeviceState( void )
{
  return DeviceState;
}


      /*********** private (static) Lora functions *************/


/******************************************************************************
 * @brief Function executed on NextSensorMeeasure Timeout event
 * @param none
 * @return none
******************************************************************************/
static void Lora_OnNextSensorMeasureTimerEvt( void )
{
  TimerStop( &NextSensorMeasureTimer );
  SensorTimerWakeup = 1;
  DeviceState = DEVICE_SEND;     
} 

/******************************************************************************
 * @brief Function executed on JoinStatusDelayTimerEvt Timeout event
 * @param none
 * @return none
******************************************************************************/
#if USE_MDM32L07X01
static void Lora_OnJoinStatusDelayTimerEvt( void )
{
  TimerStop( &JoinStatusDelayTimer );
  JoinTimeOutFlag = SET;
   
}
#endif


/******************************************************************************
 * @brief Function executed on LedTimer Timeout event
 * @param none
 * @return none
******************************************************************************/
static void Lora_OnLedTimerEvent( void )
{ 
#ifdef USE_I_NUCLEO_LRWAN1   
  BSP_LED_Modem_Off(LED_GREEN);
#elif  USE_MDM32L07X01
  BSP_LED_Off(LED2);
#endif  
}  

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
