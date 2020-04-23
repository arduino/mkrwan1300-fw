/**
  ******************************************************************************
  * @file    se_callgate.c
  * @author  MCD Application Team
  * @brief   Secure Engine CALLGATE module.
  *          This file provides set of firmware functions to manage SE Callgate
  *          functionalities.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Place code in a specific section*/

/* Includes ------------------------------------------------------------------*/
#include <stdarg.h>
#ifndef KMS_ENABLED
#include "se_crypto_bootloader.h" /* crypto services dedicated to bootloader */
#endif /* !KMS_ENABLED */
#include "se_crypto_common.h"     /* common crypto services for bootloader services and user application services */
#include "se_user_application.h"  /* user application services called by the User Application */
#include "se_callgate.h"
#include "se_key.h"
#include "se_bootinfo.h"
#include "se_utils.h"
#include "se_fwimg.h"
#include "se_low_level.h"
#include "se_startup.h"
#include "string.h"
#include "se_intrinsics.h"
#if defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#elif defined(__CC_ARM)
#include "mapping_sbsfu.h"
#endif /* __ICCARM__ || __GNUC__ */

#ifdef KMS_ENABLED
#include "kms_config.h"
#include "kms_entry.h"
#include "kms_objects.h"
#include "kms_platf_objects_interface.h"
#endif /* KMS_ENABLED */

/** @addtogroup SE Secure Engine
  * @{
  */

/** @addtogroup SE_CORE SE Core
  * @{
  */

/** @defgroup  SE_CALLGATE SE CallGate
  * @brief Implementation of the call gate API.
  *        The call gate allows the execution of code inside the protected area.
  * @{
  */

/** @defgroup SE_CALLGATE_Private_Constants Private Constants
  * @{
  */

/**
  * @brief Secure Engine Lock Status enum definition
  */
typedef enum
{
  SE_UNLOCKED = 0x55555555U,
  SE_LOCKED = 0x7AAAAAAAU
} SE_LockStatus;


/** @defgroup SE_CALLGATE_Private_Macros Private Macros
  * @{
  */

/**
  * @brief Check if the caller is located in SE Interface region
  */
#define __IS_CALLER_SE_IF() \
  do{ \
    if (LR< SE_IF_REGION_ROM_START){\
      NVIC_SystemReset();}\
    if (LR> SE_IF_REGION_ROM_END){\
      NVIC_SystemReset();}\
  }while(0)

/**
  * @brief If lock restriction service enabled, execution is forbidden
  */

#define __IS_SE_LOCKED_SERVICES() \
  do{ \
    if (SE_LockRestrictedServices != SE_UNLOCKED){\
      NVIC_SystemReset();}\
  }while(0)

/**
  * @}
  */

SE_ErrorStatus SE_CallGateService(SE_FunctionIDTypeDef eID, SE_StatusTypeDef *peSE_Status, va_list arguments);
/**
  * @brief Switch stack pointer from SB RAM region to SE RAM region then call SE_CallGateService
  */
SE_ErrorStatus SE_SP_SMUGGLE(SE_FunctionIDTypeDef eID, SE_StatusTypeDef *peSE_Status, va_list arguments);

/* Place code in a specific section*/
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".SE_CallGate_Code"
#pragma optimize=none
#elif defined(__CC_ARM)
#pragma arm section code = ".SE_CallGate_Code"
#pragma O0
#else
__attribute__((section(".SE_CallGate_Code")))
__attribute__((optimize("O0")))
#endif /* __ICCARM__ */

/** @defgroup SE_CALLGATE_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief Secure Engine CallGate function.
  *        It is the only access/exit point to code inside protected area.
  *        In order to call others functions included in the protected area, the specific ID
  *        has to be specified.
  * @note  It is a variable argument function. The correct number and order of parameters
  *        has to be used in order to call internal function in the right way.
  * @note DO NOT MODIFY THIS FUNCTION.
  *       New services can be implemented in @ref SE_CallGateService.
  * @param eID: Secure Engine protected function ID.
  * @param peSE_Status: Secure Engine Status.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_CallGate(SE_FunctionIDTypeDef eID, SE_StatusTypeDef *peSE_Status, ...)
{
#if defined(SFU_ISOLATE_SE_WITH_FIREWALL) || defined(CKS_ENABLED)
  /*
   * When Firewall isolation is used, SE_CoreBin can have its own vectors. When MPU isolation is used this is not
   * possible.
   */
  /* Variable used to store Appli vector position */
  uint32_t *p_appli_vector_addr = (uint32_t *)(SE_REGION_SRAM1_STACK_TOP - 4U);
#endif /* SFU_ISOLATE_SE_WITH_FIREWALL || CKS_ENABLED */
#if defined(__GNUC__)
  register unsigned lr asm("lr");
  uint32_t LR = lr;
#else
  uint32_t LR;
#endif /* __GNUC__ */
  SE_ErrorStatus e_ret_status;
  va_list arguments;

  /*
   * Warning : It is mandatory to call NVIC_SystemReset() in case of error
   * instead of return(SE_ERROR) to avoid any attempt of attack by modifying
   * the call stack (LR) in order to execute code inside secure enclave
   */

  /* Check the Callgate was called only from SE Interface */
#if defined(__ICCARM__) || defined (__CC_ARM)
  LR = __get_LR();
#endif /* __ICCARM__ || __CC_ARM */
  __IS_CALLER_SE_IF();

  /* Check the pointers allocation */
  if (SE_LL_Buffer_in_ram(peSE_Status, sizeof(*peSE_Status)) != SE_SUCCESS)
  {
    NVIC_SystemReset();
  }
  if (SE_LL_Buffer_part_of_SE_ram(peSE_Status, sizeof(*peSE_Status)) == SE_SUCCESS)
  {
    NVIC_SystemReset();
  }

  /* Double Check to avoid basic fault injection : the Callgate was called only from SE Interface */
  __IS_CALLER_SE_IF();

  /* Double Check to avoid basic fault injection : Check the pointers allocation */
  if (SE_LL_Buffer_in_ram(peSE_Status, sizeof(*peSE_Status)) != SE_SUCCESS)
  {
    NVIC_SystemReset();
  }
  if (SE_LL_Buffer_part_of_SE_ram(peSE_Status, sizeof(*peSE_Status)) == SE_SUCCESS)
  {
    NVIC_SystemReset();
  }

#if defined(SFU_ISOLATE_SE_WITH_FIREWALL) || defined(CKS_ENABLED)
  /*  retriev Appli Vector Value   */
  *p_appli_vector_addr = SCB->VTOR;
  /*  set SE vector */
  SCB->VTOR = (uint32_t)&__vector_table;
#endif /* SFU_ISOLATE_SE_WITH_FIREWALL || CKS_ENABLED */
  /* Enter the protected area */
  ENTER_PROTECTED_AREA();

  *peSE_Status =  SE_OK;

  /*Initializing arguments to store all values after peSE_Status*/
  va_start(arguments, peSE_Status);
  /*  call service implementation , this is split to have a fixed size */
#if defined(SFU_ISOLATE_SE_WITH_FIREWALL)
  /*  set SE specific stack before executing SE service */
  e_ret_status =  SE_SP_SMUGGLE(eID, peSE_Status, arguments);
#else
  /*  no need to use a specific Stack */
  e_ret_status =  SE_CallGateService(eID, peSE_Status, arguments);
#endif /* SFU_ISOLATE_SE_WITH_FIREWALL */
  /*Clean up arguments list*/
  va_end(arguments);

  /*  restore Appli Vector Value   */
#if defined(CKS_ENABLED)
  HAL_NVIC_DisableIRQ(IPCC_C1_RX_IRQn);
  HAL_NVIC_DisableIRQ(IPCC_C1_TX_IRQn);
  __ISB();
  SCB->VTOR = *p_appli_vector_addr;
#endif /* CKS_ENABLED */
#if defined(SFU_ISOLATE_SE_WITH_FIREWALL)
  SCB->VTOR = *p_appli_vector_addr;
#endif /* SFU_ISOLATE_SE_WITH_FIREWALL */
  /* Exit the protected area */
  EXIT_PROTECTED_AREA();

  return e_ret_status;
}
/* Stop placing data in specified section*/
#if defined(__ICCARM__)
#pragma default_function_attributes =
#pragma optimize=none
#elif defined(__CC_ARM)
#pragma arm section code
#endif /* __ICCARM__ */
/**
  * @brief Dispatch function used by the Secure Engine CallGate function.
  *        Calls other functions included in the protected area based on the eID parameter.
  * @note  It is a variable argument function. The correct number and order of parameters
  *        has to be used in order to call internal function in the right way.
  * @param eID: Secure Engine protected function ID.
  * @param peSE_Status: Secure Engine Status.  
  * @param arguments: argument list to be extracted. Depends on each protected function ID.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_CallGateService(SE_FunctionIDTypeDef eID, SE_StatusTypeDef *peSE_Status, va_list arguments)
{

  /*
   * For the time being we consider that the user keys can be handled in SE_CallGate.
   * Nevertheless, if this becomes too crypto specific then it will have to be moved to the user application code.
   */
  static SE_LockStatus SE_LockRestrictedServices = SE_UNLOCKED;

  SE_ErrorStatus e_ret_status = SE_ERROR;

  switch (eID)
  {
    /* ============================ */
    /* ===== BOOTLOADER PART  ===== */
    /* ============================ */
    case SE_INIT_ID:
    {
      uint32_t se_system_core_clock;
      SE_INFO_StatusTypedef e_boot_info_status;

      /* Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* retrieve argument(s) */
      se_system_core_clock = va_arg(arguments, uint32_t);

      *peSE_Status = SE_INIT_ERR;

      /* CRC configuration may have been changed by application */
      if (SE_LL_CRC_Config() == SE_ERROR)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Double Check to avoid basic fault injection : Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /*Initialization of SystemCoreClock variable in Secure Engine binary*/
      SE_SetSystemCoreClock(se_system_core_clock);

      /*Initialization of Shared Info. If BootInfoArea hw/Fw initialization not possible,
      the only option is to try a Factory Reset */
      e_ret_status = SE_INFO_BootInfoAreaInit(&e_boot_info_status);
      if (e_ret_status != SE_SUCCESS)
      {
        e_ret_status = SE_INFO_BootInfoAreaFactoryReset();
        if (e_ret_status == SE_SUCCESS)
        {
          *peSE_Status = SE_BOOT_INFO_ERR_FACTORY_RESET;
        }
        else
        {
          *peSE_Status = SE_BOOT_INFO_ERR;
        }
      }
      else
      {
        *peSE_Status = SE_OK;
      }

      /* NOTE : Other initialization may be added here. */
      break;
    }

    case SE_CRYPTO_LL_DECRYPT_INIT_ID:
    {
#ifndef KMS_ENABLED
      SE_FwRawHeaderTypeDef *p_x_se_Metadata;
      int32_t se_FwType;

      /* Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* retrieve argument(s) */
      p_x_se_Metadata = va_arg(arguments, SE_FwRawHeaderTypeDef *);
      se_FwType = va_arg(arguments, int32_t);

      /* CRC configuration may have been changed by application */
      if (SE_LL_CRC_Config() == SE_ERROR)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Check the Init structure allocation */
      if (SE_LL_Buffer_in_SBSFU_ram(p_x_se_Metadata, sizeof(*p_x_se_Metadata)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Check the Fw Type parameter value */
      if ((se_FwType != SE_FW_IMAGE_COMPLETE) && (se_FwType != SE_FW_IMAGE_PARTIAL))
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Double Check to avoid basic fault injection : Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* Double Check to avoid basic fault injection : Check the Init structure allocation */
      if (SE_LL_Buffer_in_SBSFU_ram(p_x_se_Metadata, sizeof(*p_x_se_Metadata)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      /*
       * The se_callgate code must be crypto-agnostic.
       * But, when it comes to decrypt, we can use:
       * - either a real decrypt operation (AES GCM or AES CBC FW encryption)
       * - or no decrypt (clear FW)
       * As a consequence, retrieving the key cannot be done at the call gate stage.
       * This is implemented in the SE_CRYPTO_Decrypt_Init code.
       */

      /* Call SE CRYPTO function*/
      e_ret_status = SE_CRYPTO_Decrypt_Init(p_x_se_Metadata, se_FwType);
#endif /* !KMS_ENABLED */
      break;
    }

    case SE_CRYPTO_LL_DECRYPT_APPEND_ID:
    {
#ifndef KMS_ENABLED
      const uint8_t *input_buffer;
      int32_t      input_size;
      uint8_t      *output_buffer;
      int32_t      *output_size;

      /* Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* retrieve argument(s) */
      input_buffer = va_arg(arguments, const uint8_t *);
      input_size = va_arg(arguments, int32_t);
      output_buffer = va_arg(arguments, uint8_t *);
      output_size = va_arg(arguments, int32_t *);

      /* CRC configuration may have been changed by application */
      if (SE_LL_CRC_Config() == SE_ERROR)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Check the pointers allocation */
      if (input_size <= 0)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      if (SE_LL_Buffer_in_SBSFU_ram(input_buffer, (uint32_t)input_size) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      if (SE_LL_Buffer_in_SBSFU_ram(output_size, sizeof(*output_size)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      if (SE_LL_Buffer_in_SBSFU_ram(output_buffer, (uint32_t)input_size) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Double Check to avoid basic fault injection : Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* Double Check to avoid basic fault injection : Check the pointers allocation */
      if (input_size <= 0)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      if (SE_LL_Buffer_in_SBSFU_ram(input_buffer, (uint32_t)input_size) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      if (SE_LL_Buffer_in_SBSFU_ram(output_size, sizeof(*output_size)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      if (SE_LL_Buffer_in_SBSFU_ram(output_buffer, (uint32_t)input_size) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Call SE CRYPTO function*/
      e_ret_status = SE_CRYPTO_Decrypt_Append(input_buffer, input_size, output_buffer, output_size);
#endif /* !KMS_ENABLED */
      break;
    }

    case SE_CRYPTO_LL_DECRYPT_FINISH_ID:
    {
#ifndef KMS_ENABLED
      uint8_t      *output_buffer;
      int32_t      *output_size;

      /* Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* retrieve argument(s) */
      output_buffer = va_arg(arguments, uint8_t *);
      output_size = va_arg(arguments, int32_t *);

      /* CRC configuration may have been changed by application */
      if (SE_LL_CRC_Config() == SE_ERROR)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Check the pointers allocation */
      if (SE_LL_Buffer_in_SBSFU_ram(output_size, sizeof(*output_size)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      /*  in AES-GCM 16 bytes can be written  */
      if (SE_LL_Buffer_in_SBSFU_ram(output_buffer, (uint32_t)16U) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Double Check to avoid basic fault injection : Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* Double Check to avoid basic fault injection : Check the pointers allocation */
      if (SE_LL_Buffer_in_SBSFU_ram(output_size, sizeof(*output_size)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /*  in AES-GCM 16 bytes can be written  */
      if (SE_LL_Buffer_in_SBSFU_ram(output_buffer, (uint32_t)16U) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Call SE CRYPTO function*/
      e_ret_status =  SE_CRYPTO_Decrypt_Finish(output_buffer, output_size);
#endif /* !KMS_ENABLED */
      break;
    }

    case SE_CRYPTO_LL_AUTHENTICATE_FW_INIT_ID:
    {
#ifndef KMS_ENABLED
      SE_FwRawHeaderTypeDef *p_x_se_Metadata;
      int32_t se_FwType;

      /* Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* retrieve argument(s) */
      p_x_se_Metadata = va_arg(arguments, SE_FwRawHeaderTypeDef *);
      se_FwType = va_arg(arguments, int32_t);

      /* CRC configuration may have been changed by application */
      if (SE_LL_CRC_Config() == SE_ERROR)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Check the Init structure allocation */
      if (SE_LL_Buffer_in_SBSFU_ram(p_x_se_Metadata, sizeof(*p_x_se_Metadata)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Check the Fw Type parameter value */
      if ((se_FwType != SE_FW_IMAGE_COMPLETE) && (se_FwType != SE_FW_IMAGE_PARTIAL))
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Double Check to avoid basic fault injection : Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* Double Check to avoid basic fault injection : Check the Init structure allocation */
      if (SE_LL_Buffer_in_SBSFU_ram(p_x_se_Metadata, sizeof(*p_x_se_Metadata)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /*
       * The se_callgate code must be crypto-agnostic.
       * But, when it comes to FW authentication, we can use:
       * - either AES GCM authentication
       * - or SHA256 (stored in an authenticated FW header)
       * So this service can rely:
       * - either on the symmetric key
       * - or a SHA256
       * As a consequence, retrieving the key cannot be done at the call gate stage.
       * This is implemented in the SE_CRYPTO_AuthenticateFW_Init code.
       */

      /* Call SE CRYPTO function*/
      e_ret_status = SE_CRYPTO_AuthenticateFW_Init(p_x_se_Metadata, se_FwType);
#endif /* !KMS_ENABLED */
      break;
    }

    case SE_CRYPTO_LL_AUTHENTICATE_FW_APPEND_ID:
    {
#ifndef KMS_ENABLED
      const uint8_t *input_buffer;
      int32_t      input_size;
      uint8_t       *output_buffer;
      int32_t      *output_size;

      /* Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* retrieve argument(s) */
      input_buffer = va_arg(arguments, const uint8_t *);
      input_size = va_arg(arguments, int32_t);
      output_buffer = va_arg(arguments, uint8_t *);
      output_size = va_arg(arguments, int32_t *);

      /* CRC configuration may have been changed by application */
      if (SE_LL_CRC_Config() == SE_ERROR)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Check the pointers allocation */
      if (input_size <= 0)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      if (SE_LL_Buffer_in_SBSFU_ram(input_buffer, (uint32_t)input_size) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      if (SE_LL_Buffer_in_SBSFU_ram(output_size, sizeof(*output_size)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      if (SE_LL_Buffer_in_SBSFU_ram(output_buffer, (uint32_t)input_size) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Double Check to avoid basic fault injection : Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* Double Check to avoid basic fault injection : Check the pointers allocation */
      if (input_size <= 0)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      if (SE_LL_Buffer_in_SBSFU_ram(input_buffer, (uint32_t)input_size) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      if (SE_LL_Buffer_in_SBSFU_ram(output_size, sizeof(*output_size)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      if (SE_LL_Buffer_in_SBSFU_ram(output_buffer, (uint32_t)input_size) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Call SE CRYPTO function*/
      e_ret_status = SE_CRYPTO_AuthenticateFW_Append(input_buffer, input_size, output_buffer, output_size);
#endif /* !KMS_ENABLED */
      break;
    }

    case SE_CRYPTO_LL_AUTHENTICATE_FW_FINISH_ID:
    {
#ifndef KMS_ENABLED
      uint8_t       *output_buffer;
      int32_t       *output_size;

      /* Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* retrieve argument(s) */
      output_buffer = va_arg(arguments, uint8_t *);
      output_size = va_arg(arguments, int32_t *);

      /* CRC configuration may have been changed by application */
      if (SE_LL_CRC_Config() == SE_ERROR)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Check the pointers allocation */
      if (SE_LL_Buffer_in_SBSFU_ram(output_size, sizeof(*output_size)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      /*  in AES-GCM 16 bytes can be written  */
      if (SE_LL_Buffer_in_SBSFU_ram(output_buffer, (uint32_t)16U) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Double Check to avoid basic fault injection : Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* Double Check to avoid basic fault injection : Check the pointers allocation */
      if (SE_LL_Buffer_in_SBSFU_ram(output_size, sizeof(*output_size)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      /*  in AES-GCM 16 bytes can be written  */
      if (SE_LL_Buffer_in_SBSFU_ram(output_buffer, (uint32_t)16U) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Call SE CRYPTO function*/
      e_ret_status = SE_CRYPTO_AuthenticateFW_Finish(output_buffer, output_size);
#endif /* !KMS_ENABLED */
      break;
    }

    case SE_CRYPTO_HL_AUTHENTICATE_METADATA:
    {
#ifndef KMS_ENABLED
      /*
       * The se_callgate code must be crypto-agnostic.
       * But, when it comes to metadata authentication, we can use:
       * - either AES GCM authentication
       * - or SHA256 signed with ECCDSA authentication
       * So this service can rely:
       * - either on the symmetric key
       * - or on the asymmetric keys
       * As a consequence, retrieving the appropriate key cannot be done at the call gate stage.
       * This is implemented in the SE_CRYPTO_Authenticate_Metadata code.
       */
      SE_FwRawHeaderTypeDef *p_x_se_Metadata;

      /* Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* retrieve argument(s) */
      p_x_se_Metadata = va_arg(arguments, SE_FwRawHeaderTypeDef *);

      /* CRC configuration may have been changed by application */
      if (SE_LL_CRC_Config() == SE_ERROR)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Check the metadata structure allocation */
      if (SE_LL_Buffer_in_SBSFU_ram(p_x_se_Metadata, sizeof(*p_x_se_Metadata)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Double Check to avoid basic fault injection : Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* Double Check to avoid basic fault injection : Check the metadata structure allocation */
      if (SE_LL_Buffer_in_SBSFU_ram(p_x_se_Metadata, sizeof(*p_x_se_Metadata)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      /* Call SE CRYPTO function*/
      e_ret_status = SE_CRYPTO_Authenticate_Metadata(p_x_se_Metadata);
#endif /* !KMS_ENABLED */
      break;
    }

    case SE_BOOT_INFO_READ_ALL_ID:
    {
      SE_BootInfoTypeDef    *p_boot_info;
      SE_INFO_StatusTypedef e_boot_info_status;

      /* Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* retrieve argument(s) */
      p_boot_info = va_arg(arguments, SE_BootInfoTypeDef *);

      /* CRC configuration may have been changed by application */
      if (SE_LL_CRC_Config() == SE_ERROR)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Check structure allocation */
      if (SE_LL_Buffer_in_SBSFU_ram(p_boot_info, sizeof(*p_boot_info)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Double Check to avoid basic fault injection : Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* Double Check to avoid basic fault injection : Check structure allocation */
      if (SE_LL_Buffer_in_SBSFU_ram(p_boot_info, sizeof(*p_boot_info)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Read BootInfo */
      if (SE_INFO_ReadBootInfoArea(p_boot_info, &e_boot_info_status) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        *peSE_Status = SE_BOOT_INFO_ERR;
      }
      else
      {
        e_ret_status = SE_SUCCESS;
        *peSE_Status = SE_OK;
      }

      break;
    }

    case SE_BOOT_INFO_WRITE_ALL_ID:
    {
      SE_BootInfoTypeDef    *p_boot_info;
      SE_INFO_StatusTypedef e_boot_info_status;

      /* Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* retrieve argument(s) */
      p_boot_info = va_arg(arguments, SE_BootInfoTypeDef *);

      /* CRC configuration may have been changed by application */
      if (SE_LL_CRC_Config() == SE_ERROR)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Check structure allocation */
      if (SE_LL_Buffer_in_SBSFU_ram(p_boot_info, sizeof(*p_boot_info)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Double Check to avoid basic fault injection : Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* Double Check to avoid basic fault injection : Check structure allocation */
      if (SE_LL_Buffer_in_SBSFU_ram(p_boot_info, sizeof(*p_boot_info)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /*
       * Add CRC.
       * The CRC is computed with the structure without its CRC field and the length is provided to SE_HAL_CRC_Calculate
       * in 32-bit word.
       * Please note that this works only if the CRC field is kept as the last uint32_t of the SE_BootInfoTypeDef
       * structure.
       */
      p_boot_info->CRC32 = SE_LL_CRC_Calculate((uint32_t *)(p_boot_info),
                                               (sizeof(SE_BootInfoTypeDef) - sizeof(uint32_t)) / sizeof(uint32_t));

      /* Write BootInfo */
      if (SE_INFO_WriteBootInfoArea(p_boot_info, &e_boot_info_status) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        *peSE_Status = SE_BOOT_INFO_ERR;
      }
      else
      {
        e_ret_status = SE_SUCCESS;
        *peSE_Status = SE_OK;
      }

      break;
    }

    case SE_IMG_READ:
    {
      void *p_destination;
      const void *p_source;
      uint32_t length;

      /* Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* retrieve argument(s) */
      p_destination = va_arg(arguments, void *);
      p_source = va_arg(arguments, const void *);
      length = va_arg(arguments, uint32_t);
      /* check the destination buffer */
      if (SE_LL_Buffer_in_SBSFU_ram(p_destination, length) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Double Check to avoid basic fault injection : Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* Double Check to avoid basic fault injection : check the destination buffer */
      if (SE_LL_Buffer_in_SBSFU_ram(p_destination, length) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Read Flash memory */
      e_ret_status = SE_IMG_Read(p_destination, p_source, length);
      break;
    }

    case SE_IMG_WRITE:
    {
      void *p_destination;
      const void *p_source;
      uint32_t length;

      /* Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* retrieve argument(s) */
      p_destination = va_arg(arguments, void *);
      p_source = va_arg(arguments, const void *);
      length = va_arg(arguments, uint32_t);

      /* check the source buffer */
      if (SE_LL_Buffer_in_SBSFU_ram(p_source, length) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Double Check to avoid basic fault injection : Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* Double Check to avoid basic fault injection : check the source buffer */
      if (SE_LL_Buffer_in_SBSFU_ram(p_source, length) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Write Flash memory */
      e_ret_status = SE_IMG_Write(p_destination, p_source, length);
      break;
    }

    case SE_IMG_ERASE:
    {
      void *p_destination;
      uint32_t length;

      /* Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* retrieve argument(s) */
      p_destination = va_arg(arguments, void *);
      length = va_arg(arguments, uint32_t);

      /* Double Check to avoid basic fault injection : Check that the Secure Engine services are not locked */
      __IS_SE_LOCKED_SERVICES();

      /* Erase Flash memory */
      e_ret_status = SE_IMG_Erase(p_destination, length);
      break;
    }

    case SE_LOCK_RESTRICT_SERVICES:
    {
      /* Check that the Secure Engine services are not locked, LOCK shall be called only once */
      __IS_SE_LOCKED_SERVICES();

      /*
       * Clean-up secure engine RAM area for series with secure memory isolation because Flash is hidden when secure
       * memory is activated but RAM is still accessible
       */
      SE_LL_CORE_Cleanup();

      /* Lock restricted services */
      SE_LockRestrictedServices = SE_LOCKED;
      e_ret_status = SE_SUCCESS;

      /* Double instruction to avoid basic fault injection */
      SE_LockRestrictedServices = SE_LOCKED;
      
#ifdef KMS_ENABLED
     /* As soon as the SBSFU is done, we can LOCK the keys */
#ifdef KMS_SBSFU_KEY_AES128_OBJECT_HANDLE
      KMS_LockKeyHandle(KMS_SBSFU_KEY_AES128_OBJECT_HANDLE);
#endif /* KMS_SBSFU_KEY_AES128_OBJECT_HANDLE */
#ifdef KMS_SBSFU_KEY_ECDSA_OBJECT_HANDLE
      KMS_LockKeyHandle(KMS_SBSFU_KEY_ECDSA_OBJECT_HANDLE);
#endif /* KMS_SBSFU_KEY_ECDSA_OBJECT_HANDLE */

      /* Double instruction to avoid basic fault injection */
#ifdef KMS_SBSFU_KEY_AES128_OBJECT_HANDLE
      KMS_LockKeyHandle(KMS_SBSFU_KEY_AES128_OBJECT_HANDLE);
#endif /* KMS_SBSFU_KEY_AES128_OBJECT_HANDLE */
#ifdef KMS_SBSFU_KEY_ECDSA_OBJECT_HANDLE
      KMS_LockKeyHandle(KMS_SBSFU_KEY_ECDSA_OBJECT_HANDLE);
#endif /* KMS_SBSFU_KEY_ECDSA_OBJECT_HANDLE */
#endif /* KMS_ENABLED */
      break;
    }

    /* ============================ */
    /* ===== APPLICATION PART ===== */
    /* ============================ */

    /* --------------------------------- */
    /* FIRMWARE IMAGES HANDLING SERVICES */
    /* --------------------------------- */

    /* No protected service needed for this */

    /* --------------------------------- */
    /* USER APPLICATION SERVICES         */
    /* --------------------------------- */
    case SE_APP_GET_ACTIVE_FW_INFO:
    {
      SE_APP_ActiveFwInfo_t *p_FwInfo;

      /* retrieve argument(s) */
      p_FwInfo = va_arg(arguments, SE_APP_ActiveFwInfo_t *);

      /* Check structure allocation */
      if (SE_LL_Buffer_in_ram((void *)p_FwInfo, sizeof(*p_FwInfo)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      if (SE_LL_Buffer_part_of_SE_ram((void *)p_FwInfo, sizeof(*p_FwInfo)) == SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      /* Double Check to avoid basic fault injection : Check structure allocation */
      if (SE_LL_Buffer_in_ram((void *)p_FwInfo, sizeof(*p_FwInfo)) != SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }
      if (SE_LL_Buffer_part_of_SE_ram((void *)p_FwInfo, sizeof(*p_FwInfo)) == SE_SUCCESS)
      {
        e_ret_status = SE_ERROR;
        break;
      }

      e_ret_status = SE_APPLI_GetActiveFwInfo(p_FwInfo);
      break;
    }

    default:
    {
#ifdef KMS_ENABLED
      /* Is it a request to the KMS lib */
      if ((eID & SE_MW_ADDON_MSB_MASK) == SE_MW_ADDON_KMS_MSB)
      {
        CK_RV    l_rv;

        /* To limit the number of passed parameters, we consider that */
        /* KMS_Entry() returns the CK_RV error. */
        l_rv = KMS_Entry((KMS_FunctionIDTypeDef)eID, arguments);

        /*
         * SE_StatusTypeDef is used to forward the CK_RV result to upper layers
         */
        *peSE_Status = l_rv;

        if (l_rv == CKR_OK)
        {
          e_ret_status = SE_SUCCESS;
        }
        else
        {
          e_ret_status = SE_ERROR;
        }
        break;
      }
#endif /* KMS_ENABLED */
      /* Unspecified function ID -> Reset */
      NVIC_SystemReset();

      break;
    }
  }
  if ((e_ret_status == SE_ERROR) && (*peSE_Status == SE_OK))
  {
    *peSE_Status = SE_KO;
  }
  return e_ret_status;
}

/* Stop placing data in specified section*/


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
