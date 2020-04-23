/**
  ******************************************************************************
  * @file    se_bootinfo.c
  * @author  MCD Application Team
  * @brief   Secure Engine BOOTINFO module.
  *          This file provides set of firmware functions to manage SE BootInfo
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
/* Includes ------------------------------------------------------------------*/
#include "se_bootinfo.h"
#include "se_utils.h"
#include "se_low_level.h"
#include "string.h"

/** @addtogroup SE Secure Engine
  * @{
  */

/** @addtogroup SE_CORE SE Core
  * @brief Secure Engine core code compiled in SE_CoreBin.
  * @{
  */

/** @defgroup  SE_BOOTINFO SE BootInfo
  *  @brief Boot Information management (system status used at boot time)
  * @{
  */


/** @defgroup SE_BOOTINFO_Private_Defines Private Defines
  * @{
  */
#define SE_INFO_NONE                    0x0U         /*!< No information on last execution */

/**
  * @}
  */

/** @defgroup SE_BOOTINFO_Private_Variables Private Variables
  * @brief WARNING: All the private variable of modules used by the SecureEngine must be
  *        not initialized. if these variables are used inside Secure Engine APIs
  *        exported to the UserApp, you can't count on the compiler/linker variable
  *        initialization at reset
  * @{
  */

static SE_INFO_StatusTypedef eBootInfoStatus;        /*!< Status of boot info : SE_INFO_VALID, SE_INFO_RECOVERED ... */

/* placing data in a specific section named BOOTINFO_DATA */
#if defined(__ICCARM__)
#pragma default_variable_attributes = @ "BOOTINFO_DATA"
static SE_BootInfoTypeDef xBootInfo;                 /*!< Boot information for last execution */
static SE_BootInfoTypeDef xBootInfoBackUp;           /*!< Backup values in case in BootInfo invalidity : CRC check KO */
#pragma default_variable_attributes =
#else
__attribute__((section("BOOTINFO_DATA")))
static SE_BootInfoTypeDef xBootInfo;                 /*!< Boot information for last execution */
__attribute__((section("BOOTINFO_DATA")))
static SE_BootInfoTypeDef xBootInfoBackUp;           /*!< Backup values in case in BootInfo invalidity : CRC check KO */
#endif /* __ICCARM__ */
/* Stop placing data in a specific section named BOOTINFO_DATA */

/**
  * @}
  */

/** @defgroup SE_BOOTINFO_Private_Macros Private Macros
  * @{
  */

/**
  * @brief Check validity of STATUS parameter
  */
#define IS_SE_INFO_STATUS(STATUS)      (((STATUS) == SE_INFO_NOT_ACCESSED) || \
                                        ((STATUS) == SE_INFO_VALID) || \
                                        ((STATUS) == SE_INFO_RECOVERED) || \
                                        ((STATUS) == SE_INFO_NOT_FOUND) || \
                                        ((STATUS) == SE_INFO_NOT_VALID))
/**
  * @}
  */

/** @defgroup SE_BOOTINFO_Private_Functions Private Functions
  * @{
  */

/**
  * @brief  Check the CRC of the BootInfo given as a parameter.
  * @param  x: pointer to the BootInfo to be checked.
  * @retval SE_SUCCESS if it succeeds, SE_ERROR otherwise
  */
static SE_ErrorStatus SE_LL_CRC_Check(SE_BootInfoTypeDef *x)
{
  /*
   * The CRC is computed with the structure without its CRC field and the length is provided to SE_LL_CRC_Calculate in
   * 32-bit word.
   * Please note that this works only if the CRC field is kept as the last uint32_t of the SE_BootInfoTypeDef structure.
   */
  uint32_t crc = SE_LL_CRC_Calculate((uint32_t *)(x), (sizeof(SE_BootInfoTypeDef) - sizeof(uint32_t)) \
                                     / sizeof(uint32_t));

  if ((x)->CRC32 == crc)
  {
    /* CRC ok */
    return (SE_SUCCESS);
  }
  else
  {
    /* CRC ko */
    return (SE_ERROR);
  }
}

/**
  * @}
  */

/** @defgroup SE_BOOTINFO_Exported_Functions Exported Functions
  * @{
  */

/** @defgroup SE_BOOTINFO_Initialization_Functions Initialization Functions
  * @{
  */

/**
  * @brief  Initialize the global variable used by this module and the
  *         BootInfoArea Hardware (CRC, etc.).
  * @param  peBootInfoStatus: output param to be filled with the BootInfoArea status
  * @note   The specific global variable initialization is mandatory for all the modules part
  *         of the Secure Engine and cannot be delegated to the compiler/linker.
  *         When the secure engine is exported to an external application , only
  *         the SE APIs are exported while the global variable initialization (normally executed
  *         during the startup of this project) will not be executed. So without calling this function,
  *         all the global variables used by this module should be declared not-initialized and must be
  *         initialized here!
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise
  */
SE_ErrorStatus SE_INFO_BootInfoAreaInit(SE_INFO_StatusTypedef *peBootInfoStatus)
{
  SE_ErrorStatus e_ret_status;

  /* Initialize the BootInfoArea Status */
  eBootInfoStatus = SE_INFO_NOT_ACCESSED;

  /* Initialize the Hardware: CRC */
  if (SE_LL_CRC_Init() != SE_SUCCESS)
  {
    assert_param((IS_SE_INFO_STATUS(eBootInfoStatus)));
    *peBootInfoStatus = eBootInfoStatus;
    return SE_ERROR;
  }

  /*
   * Check the xBootInfo CRC. if CRC not correct then reset the structure to factory reset values - A bug or a possible
   * threat could have changed it
   */
  if (SE_ERROR == SE_LL_CRC_Check(&xBootInfo))
  {
    /*
     * XbootInfo values not valid so check the xBootInfoBackUp CRC to determine if back-up values in RAM are valid.
     * A possible threat or a power off could have changed them.
     * The CRC is computed with the structure without its CRC field and the length is provided to SE_LL_CRC_Calculate in
     * 32-bit word.
     * Please note that this works only if the CRC field is kept as the last uint32_t of the SE_BootInfoTypeDef
     * structure.
     */
    if (SE_ERROR == SE_LL_CRC_Check(&xBootInfoBackUp))
    {
      /* update BootInfoArea Status to trig a factory reset */
      eBootInfoStatus = SE_INFO_NOT_VALID;
      e_ret_status = SE_ERROR;
    }
    else
    {
      /* as xBootInfoBackUp values are valid - update XbootInfo values with xBootInfoBackUp values */
      (void) memcpy(&xBootInfo, &xBootInfoBackUp, sizeof(SE_BootInfoTypeDef));
      eBootInfoStatus = SE_INFO_RECOVERED;
      e_ret_status = SE_SUCCESS;
    }
  }
  else
  {
    /* keep xBootInfo values as they are as data from previous execution are correct*/
    eBootInfoStatus = SE_INFO_VALID;
    e_ret_status = SE_SUCCESS;
  }

  assert_param((IS_SE_INFO_STATUS(eBootInfoStatus)));
  *peBootInfoStatus = eBootInfoStatus;
  return e_ret_status;
}

/**
  * @brief  DeInitialize the BootInfoArea structure cleaning the SRAM,
  *         and the xBootInfo area access and the related Hw.
  * @param  None.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise
  */
SE_ErrorStatus SE_INFO_BootInfoAreaDeInit(void)
{
  /* DeInitialize the Hardware: CRC */
  if (SE_LL_CRC_DeInit() != SE_SUCCESS)
  {
    return SE_ERROR;
  }

  return SE_SUCCESS;
}
/**
  * @}
  */

/** @defgroup SE_BOOTINFO_Control_Functions Control Functions
  * @{
  */

/**
  * @brief  Reset the BootInfo shared Area to factory value.
  * @note   This function must be called in case it's not possible to read a valid BootInfoArea even after an attempted
  *         recovery .
  *         This can happen if the MCU is rebooting the 1st time after this code has been flashed,
  *         or if for some reason the BootInfoArea and its backup copy are corrupted
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise
  */
SE_ErrorStatus SE_INFO_BootInfoAreaFactoryReset(void)
{
  SE_ErrorStatus        e_ret_status;
  SE_INFO_StatusTypedef boot_info_status;
  SE_BootInfoTypeDef    boot_shared_info_empty;
  uint32_t              crc;


  assert_param((IS_SE_INFO_STATUS(eBootInfoStatus)));

  /* Check if BootInfo Area has already been accessed via initialization function - otherwise it is not a normal
   * sequence
   */
  if (eBootInfoStatus != SE_INFO_NOT_ACCESSED)
  {
    /* Initialize an Empty/Default BootInfoArea structure */
    boot_shared_info_empty.LastExecStatus = SE_INFO_NONE;
    boot_shared_info_empty.LastExecError = SE_INFO_NONE;
    boot_shared_info_empty.ConsecutiveBootOnErrorCounter = 0U;
    /*
     * It could be also possible to use an already calculated value instead of calculate it at runtime at this stage
     * The CRC is computed with the structure without its CRC field and the length is provided to SE_LL_CRC_Calculate in
     * 32-bit word.
     * Please note that this works only if the CRC field is kept as the last uint32_t of the SE_BootInfoTypeDef
     * structure.
     */
    crc = SE_LL_CRC_Calculate((uint32_t *)(&boot_shared_info_empty), (sizeof(SE_BootInfoTypeDef) - sizeof(uint32_t)) \
                              / sizeof(uint32_t));
    boot_shared_info_empty.CRC32 = crc;
    e_ret_status = SE_INFO_WriteBootInfoArea(&boot_shared_info_empty, &boot_info_status);
  }
  else
  {
    /*
     * return an error as it not normal to call FactoryReset function if BootInfo Area has not been first accessed via
     * initialization function
     */
    e_ret_status = SE_ERROR;
  }

  return e_ret_status;
}

/**
  * @brief  Read the xBootInfo area and return the BootInfoArea  and the related status.
  * @param  pxBootInfo: output param to be filled with the BootInfoArea structure
  * @param  peBootInfoStatus: output param to be filled with the BootInfoArea status
  * @note   This function is supposed to be called from inside the protected area (when available in the MCU).
  *         SE_INFO_BootInfoAreaInit must have been called before using this function;
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise
  */
SE_ErrorStatus SE_INFO_ReadBootInfoArea(SE_BootInfoTypeDef *pxBootInfo, SE_INFO_StatusTypedef *peBootInfoStatus)
{
  SE_ErrorStatus e_ret_status;

  /* Check the pointers allocation */
  if ((pxBootInfo == NULL) || (peBootInfoStatus == NULL))
  {
    return SE_ERROR;
  }

  /*
   * Check the xBootInfo CRC. if CRC not correct then reset the structure to factory reset values - A bug or a possible
   * threat could have changed it
   */
  if (SE_ERROR == SE_LL_CRC_Check(&xBootInfo))
  {
    /*
     * Update BootInfoArea Status to indicate that data can't be read as they are not valid - error should be managed
     * by caller
     */
    eBootInfoStatus = SE_INFO_NOT_VALID;
    e_ret_status = SE_ERROR;
    /*
     * xBootInfoBackUp values are not checked/used as this case shall never occur as data are initialized during
     * initialization phase - could be a bug or an attcak
     */
  }
  else
  {
    (void) memcpy(pxBootInfo, &xBootInfo, sizeof(SE_BootInfoTypeDef));

    eBootInfoStatus = SE_INFO_VALID;

    e_ret_status = SE_SUCCESS;
  }

  assert_param((IS_SE_INFO_STATUS(eBootInfoStatus)));
  *peBootInfoStatus = eBootInfoStatus;

  return e_ret_status;
}

/**
  * @brief  Write the passed BootInfo pointer into the xBootInfo area and return related status.
  * @param  pxBootInfo: input param containing the BootInfoArea structure to be written
  * @param  peBootInfoStatus: output param to be filled with the BootInfoArea status
  * @note   This function is supposed to be called from inside the protected area (when available in the MCU)
  *         SE_INFO_BootInfoAreaInit must have been called before using this function;
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise
  */
SE_ErrorStatus SE_INFO_WriteBootInfoArea(SE_BootInfoTypeDef *pxBootInfo, SE_INFO_StatusTypedef *peBootInfoStatus)
{
  SE_ErrorStatus e_ret_status;

  /* Check the pointers allocation */
  if ((pxBootInfo == NULL) || (peBootInfoStatus == NULL))
  {
    return SE_ERROR;
  }

  /*
   * Check the pxBootInfo CRC. if CRC not correct then reset the structure to factory reset values.
   * A bug or a possible threat could have changed it
   */
  if (SE_ERROR == SE_LL_CRC_Check(pxBootInfo))
  {
    eBootInfoStatus = SE_INFO_NOT_VALID;
    e_ret_status = SE_ERROR;
  }
  else /* the CRC is valid so let's store it */
  {
    /* update xBootInfo structure in RAM */
    (void) memcpy(&xBootInfo, pxBootInfo, sizeof(SE_BootInfoTypeDef));
    /* since xBootInfo structure has correclty been updated, update xBootInfoBackUp Structure */
    (void) memcpy(&xBootInfoBackUp, pxBootInfo, sizeof(SE_BootInfoTypeDef));
    eBootInfoStatus = SE_INFO_VALID;
    e_ret_status = SE_SUCCESS;
  }

  assert_param(IS_SE_INFO_STATUS(eBootInfoStatus));
  *peBootInfoStatus = eBootInfoStatus;

  return e_ret_status;
}

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
