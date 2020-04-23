/**
  ******************************************************************************
  * @file    tiny_vsnprintf.h
  * @author  MCD Application Team
  * @brief   Header for tiny_vsnprintf.c module
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
#ifndef __TINY_VSNPRINTF_H__
#define __TINY_VSNPRINTF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
 * @brief  Tiny implementation of vsnprintf() like function
 *
 *         It has been adapted so that:
 *         - Tiny implementation, when defining TINY_PRINTF, is available. In such as case,
 *           not all the format are available. Instead, only %02X, %x, %d, %u, %s and %c are available.
 *           %f,, %+, %#, %- and others are excluded
 *         - Provide a snprintf like implementation. The size of the buffer is provided,
 *           and the length of the filled buffer is returned (not including the final '\0' char).
 *         The string may be truncated
 * @param  Pointer to a buffer where the resulting C-string is stored. The buffer should have a size of
 *         at least n characters.
 * @param  Maximum number of bytes to be used in the buffer. The generated string has a length of at
 *         most n-1, leaving space for the additional terminating null character.
 * @param  C string that contains a format string that follows the same specifications as format
 *         in printf (see printf for details).
 * @param  A value identifying a variable arguments list initialized with va_start.
 * @retval The number of written char (note that this is different from vsnprintf()
 */
int tiny_vsnprintf_like(char *buf, const int size, const char *fmt, va_list args);

#ifdef __cplusplus
}
#endif

#endif /* __TINY_VSNPRINTF_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
