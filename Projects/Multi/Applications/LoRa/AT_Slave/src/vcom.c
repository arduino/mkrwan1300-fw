/******************************************************************************
 * @file    vcom.c
 * @author  MCD Application Team
 * @version V1.1.2
 * @date    08-September-2017
 * @brief   manages virtual com port
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

#include "hw.h"
#include "vcom.h"
#include "hw_gpio.h"
#include <stdarg.h>
#include "tiny_vsnprintf.h"
#include "low_power.h"
#include "command.h"

/* Force include of hal uart in order to inherite HAL_UART_StateTypeDef definition */
#include "stm32l0xx_hal_dma.h"
#include "stm32l0xx_hal_uart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* based on UART_HandleTypeDef */
static struct {
  char buffTx[256];                   /**< buffer to transmit */
  char buffRx[256];                   /**< Circular buffer of received chars */
  int rx_idx_free;                    /**< 1st free index in BuffRx */
  int rx_idx_toread;                  /**< next char to read in buffRx, when not rx_idx_free */
  HW_LockTypeDef Lock;                /**< Locking object */

  __IO HAL_UART_StateTypeDef gState;  /**< UART state information related to global Handle management
                                           and also related to Tx operations. */
  __IO HAL_UART_StateTypeDef RxState; /**< UART state information related to Rx operations. */
} uart_context;

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Transmit uart_context.buffTx from start to len - 1
 * @param  1st index to transmit
 * @param  Last index not to transmit
 * @return Last index not transmitted
 */
static int buffer_transmit(int start, int len);

/**
 * @brief  Takes one character that has been received and save it in uart_context.buffRx
 * @param  received character
 */
static void receive(char rx);


/* Functions Definition ------------------------------------------------------*/

void vcom_Init(void)
{
  LL_LPUART_InitTypeDef LPUART_InitStruct;

  /*## Configure the UART peripheral ######################################*/
  /* Put the UART peripheral in the Asynchronous mode (UART Mode) */
  /* UART1 configured as follow:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = ODD parity
      - BaudRate = 921600 baud
      - Hardware flow control disabled (RTS and CTS signals) */

  /*
   * Clock initialization:
   * - LSE source selection
   * - LPUART clock enable
   * - GPIO clocks are enabled in vcom_IoInit()
   */
  LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_HSI);
  UARTX_CLK_ENABLE();
  vcom_IoInit();

  LPUART_InitStruct.BaudRate = 19200;
  LPUART_InitStruct.DataWidth = LL_LPUART_DATAWIDTH_8B;
  LPUART_InitStruct.StopBits = LL_LPUART_STOPBITS_2;
  LPUART_InitStruct.Parity = LL_LPUART_PARITY_NONE;
  LPUART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
  LPUART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;

  if (LL_LPUART_Init(UARTX, &LPUART_InitStruct) != SUCCESS)
  {
    Error_Handler();
  }

  /* Configuring the LPUART specific LP feature - the wakeup from STOP */
  LL_LPUART_EnableInStopMode(UARTX);

  /* WakeUp from stop mode on start bit detection*/
  LL_LPUART_SetWKUPType(UARTX, LL_LPUART_WAKEUP_ON_STARTBIT);
  //LL_LPUART_SetWKUPType(UARTX, LL_LPUART_WAKEUP_ON_RXNE);
  LL_LPUART_EnableIT_WKUP(UARTX);

  LL_LPUART_Enable(UARTX);
  while (LL_LPUART_IsActiveFlag_TEACK(UARTX) == RESET)
  {
    ;
  }
  while (LL_LPUART_IsActiveFlag_REACK(UARTX) == RESET)
  {
    ;
  }

  uart_context.gState = HAL_UART_STATE_READY;
  uart_context.RxState = HAL_UART_STATE_READY;
}

void vcom_DeInit(void)
{
  LL_LPUART_DeInit(UARTX);

  /*##-1- Reset peripherals ##################################################*/
  UARTX_FORCE_RESET();
  UARTX_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HW_GPIO_DeInit(UARTX_TX_GPIO_PORT, UARTX_TX_PIN);
  /* Configure UART Rx as alternate function  */
  HW_GPIO_DeInit(UARTX_RX_GPIO_PORT, UARTX_RX_PIN);

  /*##-3- Disable the NVIC for UART ##########################################*/
  NVIC_DisableIRQ(UARTX_IRQn);
}

void vcom_Send(const char *format, ...)
{
  va_list args;
  static __IO uint16_t len = 0;
  uint16_t current_len;
  int start;
  int stop;

  va_start(args, format);

  BACKUP_PRIMASK();
  DISABLE_IRQ();
  if (len != 0)
  {
    if (len != sizeof(uart_context.buffTx))
    {
      current_len = len; /* use current_len instead of volatile len in below computation */
      len = current_len + tiny_vsnprintf_like(uart_context.buffTx + current_len, \
                                              sizeof(uart_context.buffTx) - current_len, format, args);
    }
    RESTORE_PRIMASK();
    va_end(args);
    return;
  }
  else
   {
    len = tiny_vsnprintf_like(uart_context.buffTx, sizeof(uart_context.buffTx), format, args);
  }

  current_len = len;
  RESTORE_PRIMASK();

  start = 0;
  
  do
  {
    stop = buffer_transmit(start, current_len);

    {  
      BACKUP_PRIMASK();
      DISABLE_IRQ();
      if (len == stop)
      {
        len = 0;
        RESTORE_PRIMASK();
      }
      else
      {
        start = stop;
        current_len = len;
        RESTORE_PRIMASK();
      }
    }
  } while (current_len != stop);

  va_end(args);
}

void vcom_ReceiveInit(void)
{
  if (uart_context.RxState != HAL_UART_STATE_READY)
  {
    return;
  }

  /* Process Locked */
  HW_LOCK(&uart_context);

  uart_context.RxState = HAL_UART_STATE_BUSY_RX;

  /* Enable the UART Parity Error Interrupt */
  LL_LPUART_EnableIT_PE(UARTX);

  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  LL_LPUART_EnableIT_ERROR(UARTX);

  /* Process Unlocked */
  HW_UNLOCK(&uart_context);
}

void vcom_IoInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStruct.Alternate = UARTX_TX_AF;

  HW_GPIO_Init(UARTX_TX_GPIO_PORT, UARTX_TX_PIN, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Alternate = UARTX_RX_AF;

  HW_GPIO_Init(UARTX_RX_GPIO_PORT, UARTX_RX_PIN, &GPIO_InitStruct);

  /*##-3- Configure the NVIC for UART ########################################*/
  /* NVIC for UART */
  NVIC_SetPriority(UARTX_IRQn, 0);
  NVIC_EnableIRQ(UARTX_IRQn);
  
  /* enable RXNE */
  LL_LPUART_EnableIT_RXNE(UARTX);

}

void vcom_IoDeInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure = {0};

  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;

  HW_GPIO_Init(UARTX_TX_GPIO_PORT, UARTX_TX_PIN, &GPIO_InitStructure);

  HW_GPIO_Init(UARTX_RX_GPIO_PORT, UARTX_RX_PIN, &GPIO_InitStructure);
}

FlagStatus IsNewCharReceived(void)
{
  FlagStatus status;
  
  BACKUP_PRIMASK();
  DISABLE_IRQ();
  
  status = ((uart_context.rx_idx_toread == uart_context.rx_idx_free) ? RESET : SET);
  
  RESTORE_PRIMASK();
  return status;
}

uint8_t GetNewChar(void)
{
  uint8_t NewChar;

  BACKUP_PRIMASK();
  DISABLE_IRQ();

  NewChar = uart_context.buffRx[uart_context.rx_idx_toread];
  uart_context.rx_idx_toread = (uart_context.rx_idx_toread + 1) % sizeof(uart_context.buffRx);

  RESTORE_PRIMASK();
  return NewChar;
}

void vcom_IRQHandler(void)
{
  int rx_ready = 0;
  char rx;
  
  /* UART Wake Up interrupt occured ------------------------------------------*/
  if (LL_LPUART_IsActiveFlag_WKUP(UARTX) && (LL_LPUART_IsEnabledIT_WKUP(UARTX) != RESET))
  {
    LL_LPUART_ClearFlag_WKUP(UARTX);

    /* forbid stop mode */
    LowPower_Disable(e_LOW_POWER_UART);

    /* Enable the UART Data Register not empty Interrupt */
    LL_LPUART_EnableIT_RXNE(UARTX);
  }

  if (LL_LPUART_IsActiveFlag_RXNE(UARTX) && (LL_LPUART_IsEnabledIT_RXNE(UARTX) != RESET))
  {
    /* no need to clear the RXNE flag because it is auto cleared by reading the data*/
    rx = LL_LPUART_ReceiveData8(UARTX);
    rx_ready = 1;
    
    /* allow stop mode*/
    LowPower_Enable(e_LOW_POWER_UART);
  }

  if (LL_LPUART_IsActiveFlag_PE(UARTX) || LL_LPUART_IsActiveFlag_FE(UARTX) || LL_LPUART_IsActiveFlag_ORE(UARTX) || LL_LPUART_IsActiveFlag_NE(UARTX))
  {
    // PRINTF("Error when receiving\n"); Using return code
    /* clear error IT */
    LL_LPUART_ClearFlag_PE(UARTX);
    LL_LPUART_ClearFlag_FE(UARTX);
    LL_LPUART_ClearFlag_ORE(UARTX);
    LL_LPUART_ClearFlag_NE(UARTX);
    
    rx = AT_ERROR_RX_CHAR;
    rx_ready = 1;
  }
  
  if (rx_ready == 1)
  {
    receive(rx);
  }
}

/* Private functions Definition ------------------------------------------------------*/

static int buffer_transmit(int start, int len)
{
  int i;
  for (i = start; i < len; i++)
  {
    LL_LPUART_ClearFlag_TC(UARTX);
    LL_LPUART_TransmitData8(UARTX, uart_context.buffTx[i]);

    while (LL_LPUART_IsActiveFlag_TC(UARTX) != SET)
    {
      ;
    }
  }
  LL_LPUART_ClearFlag_TC(UARTX);
  return len;
}

static void receive(char rx)
{
  int next_free;

  /** no need to clear the RXNE flag because it is auto cleared by reading the data*/
  uart_context.buffRx[uart_context.rx_idx_free] = rx;
  next_free = (uart_context.rx_idx_free + 1) % sizeof(uart_context.buffRx);
  if (next_free != uart_context.rx_idx_toread)
  {
    /* this is ok to read as there is no buffer overflow in input */
    uart_context.rx_idx_free = next_free;
  }
  else
  {
    /* force the end of a command in case of overflow so that we can process it */
    uart_context.buffRx[uart_context.rx_idx_free] = '\r';
    PRINTF("uart_context.buffRx buffer overflow %d\r\n");
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
