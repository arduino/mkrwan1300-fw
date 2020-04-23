/**
  ******************************************************************************
  * @file    se_hardware.h
  * @author  MCD Application Team / MCU Embedded SW Team
  * @brief   This file contains definitions for Secure Engine Interface module
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SE_INTRINSICS_H
#define SE_INTRINSICS_H

#ifdef __cplusplus
extern "C" {
#endif

#if   defined (__CC_ARM)
#define __root __attribute__((used))
#elif defined (__GNUC__)
#define __root __attribute__((used))
#endif /* __CC_ARM */

#if defined(__CC_ARM)
#define __get_SP __current_sp
#elif defined(__GNUC__)
static inline unsigned int __get_SP(void)
{
  register unsigned sp asm("sp");
  return sp;
}
#endif /* __CC_ARM */

#if defined(__CC_ARM)
#define __get_LR __return_address
#elif defined(__GNUC__)
static inline unsigned int __get_LR(void)
{
  register uint32_t result;
  asm volatile("MOV %0, LR\n" : "=r"(result));
  return (result);
}
#endif /* __CC_ARM */

#if defined(__ICCARM__)
extern void *__vector_table;
#elif defined(__CC_ARM)
extern void *__Vectors;
#define __vector_table __Vectors
#elif defined(__GNUC__)
extern void *g_pfnVectors[];
#define __vector_table g_pfnVectors
#endif /* __ICCARM__ */

#ifdef __cplusplus
}
#endif

#endif /* SE_INTRINSICS */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

