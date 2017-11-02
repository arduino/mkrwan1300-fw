/**
  @page AT_Slave Readme file
 
  @verbatim
  ******************** (C) COPYRIGHT 2017 STMicroelectronics *******************
  * @file    AT_Slave/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    08-September-2017
  * @brief   This application is a simple demo application software of a LoRa
  *          modem controlled though AT command interface over UART by an
  *          external host
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
   @endverbatim

@par Description

This directory contains a set of source files that implements an example of a LoRa
modem that is controlled though AT command interface over UART by an external host,
like a computer executing a terminal.

This application is targeting the B-L072Z-LRWAN1 Discovery board embedding the
CMWX1ZZABZ-091 LoRa module.

This application uses the Cube Low Layer drivers APIs targeting the STM32L0
(embedded in the module) to optimize the code size.
  ******************************************************************************



@par Directory contents 


  - AT_Slave/Inc/at.h                     Header for at.c
  - AT_Slave/Inc/Comissioning.h           End device comissioning parameters
  - AT_Slave/Inc/command.h                Header for command.c
  - AT_Slave/Inc/debug.h                  Interface to debug functionally
  - AT_Slave/Inc/hw.h                     Group all hw interface
  - AT_Slave/Inc/hw_conf.h                File to manage Cube SW family used and debug switch
  - AT_Slave/Inc/hw_gpio.h                Header for hw_gpio.c
  - AT_Slave/Inc/hw_msp.h                 Header for driver hw msp module
  - AT_Slave/Inc/hw_rtc.h                 Header for hw_rtc.c
  - AT_Slave/Inc/hw_spi.h                 Header for hw_spi.c
  - AT_Slave/Inc/lora.h                   Header for lora.c
  - AT_Slave/Inc/mlm32l0xx_hw_conf.h      Definitions for mlm32l0xx HW configuration
  - AT_Slave/Inc/mlm32l0xx_it.h           Header for mlm32l0xx_it.c
  - AT_Slave/Inc/stm32l0xx_hal_conf.h     Library Configuration file
  - AT_Slave/Inc/tiny_sscanf.h            Header for tiny_sscanf.c
  - AT_Slave/Inc/tiny_vsnprintf.h         Header for tiny_vsnprintf.c
  - AT_Slave/Inc/vcom.h                   Interface to vcom.c
  - AT_Slave/Inc/version.h                Version file
  
  - AT_Slave/Src/at.c                     AT commands API
  - AT_Slave/Src/command.c                Definitions of AT commands
  - AT_Slave/Src/debug.c                  Debug driver
  - AT_Slave/Src/hw_gpio.c                Gpio driver
  - AT_Slave/Src/hw_rtc.c                 Rtc driver
  - AT_Slave/Src/hw_spi.c                 Spi driver
  - AT_Slave/Src/lora.c                   LoRa API to drive the LoRa state machine
  - AT_Slave/Src/main.c                   Main program file
  - AT_Slave/Src/mlm32l0xx_hw.c           MLM32l0xx specific hardware driver code
  - AT_Slave/Src/mlm32l0xx_it.c           MLM32l0xx Interrupt handlers
  - AT_Slave/Src/tiny_sscanf.c            Tiny implementation of sscanf() like function
  - AT_Slave/Src/tiny_vsnprintf.c         Tiny implementation of vsnprintf() like function
  - AT_Slave/Src/vcom.c                   Virtual com port interface on Terminal

@par Hardware and Software environment 


  - This example runs on the B-L072Z-LRWAN1 Discovery board embedding the
    CMWX1ZZABZ-091 LoRa module, and has been tested with STMicroelectronics
    B-L072Z-LRWAN1 RevC board.

  - B-L072Z-LRWAN1 Discovery board Set-up    
    - Connect the Nucleo board to your PC with a USB cable type A to micro-B 
      to ST-LINK connector (CN7).
    - Please ensure that the ST-LINK connector CN8 jumpers are fitted.
  -Set Up:


             --------------------------  V    V  --------------------------
             |      LoRa Object       |  |    |  |      LoRa Network      |
             |                        |  |    |  |                        |
   ComPort<--|                        |--|    |--|                        |-->Web Server
             |                        |          |                        |
             --------------------------          --------------------------

@par How to use it ? 
In order to make the program work, you must do the following :
  - Open your preferred toolchain 
  - Rebuild all files and load your image into target memory
  - Run the example
  - Open a Terminal, connected the LoRa Object
  - UART Config = 9600, 8b, 1 stopbit, no parity, no flow control ( in src/vcom.c)
  - Terminal Config: Select 'CR+LF' for Transmit New-Line and switch 'Local echo' on
  - Send your AT commands by typing them in the terminal
   
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */