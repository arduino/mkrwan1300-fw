/**
  @page AT_Master Readme file
 
  @verbatim
  ******************** (C) COPYRIGHT 2017 STMicroelectronics *******************
  * @file    AT_Master/readme.txt 
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

This directory contains a set of source files that implements a host AT_Master
application, featuring a complete set of AT commands to drive an external
LoRa modem.

The application provides a way to establish a LoRa Link, to read sensors
(temperature, humidity and pressure) and to transmit associated data to the
LoRa Network.

This application is targeting the NUCLEO-L053R8 board and uses the Cube Low
Layer drivers APIs targeting the STM32L0 (embedded in the module) to optimize
the code size.
  ******************************************************************************



@par Directory contents 


  - AT_Master/Inc/bsp.h                    Header for bsp.c
  - AT_Master/Inc/debug.h                  Interface to debug functionally
  - AT_Master/Inc/hw.h                     Group all hw interface
  - AT_Master/Inc/hw_conf.h                File to manage Cube SW family used and debug switch
  - AT_Master/Inc/hw_gpio.h                Header for hw_gpio.c
  - AT_Master/Inc/hw_msp.h                 Header for driver hw msp module
  - AT_Master/Inc/hw_rtc.h                 Header for hw_rtc.c
  - AT_Master/Inc/hw_usart.h               Header for hw_usart.c
  - AT_Master/Inc/lora_driver.h            Header for lora_driver.c
  - AT_Master/Inc/stm32l0xx_hal_conf.h     Library Configuration file
  - AT_Master/Inc/stm32l0xx_hw_conf.h      Header for stm32l0xx_hw_conf.c
  - AT_Master/Inc/stm32l0xx_it.h           Header for stm32l0xx_it.c
  - AT_Master/Inc/tiny_sscanf.h            Header for tiny_sscanf.c
  - AT_Master/Inc/tiny_vsnprintf.h         Header for tiny_vsnprintf.c
  - AT_Master/Inc/vcom.h                   Interface to vcom.c
  
  - AT_Master/Src/bsp.c                    I_NUCLEO_LRWAN1 and X_NUCLEO_IKS01A1 sensors handlers
  - AT_Master/Src/debug.c                  Debug driver
  - AT_Master/Src/hw_gpio.c                Gpio driver
  - AT_Master/Src/hw_rtc.c                 Rtc driver
  - AT_Master/Src/hw_usart.c               Usart driver
  - AT_Master/Src/lora_driver.c            LoRa API to drive the LoRa modem
  - AT_Master/Src/main.c                   Main program file
  - AT_Master/Src/stm32l0xx_hal_msp.c      stm32l0xx specific hardware HAL code
  - AT_Master/Src/stm32l0xx_hw.c           stm32l0xx specific hardware driver code
  - AT_Master/Src/stm32l0xx_it.c           STM32l0xx Interrupt handlers
  - AT_Master/Src/tiny_sscanf.c            Tiny implementation of sscanf() like function
  - AT_Master/Src/tiny_vsnprintf.c         Tiny implementation of vsnprintf() like function

@par Hardware and Software environment 


  - This example runs on the STM32L053R8 devices and has been tested with
    STMicroelectronics NUCLEO-L053R8 RevC board.

  - NUCLEO-L053R8 board Set-up 
    - When using MDM32L07X01 expansion board, plug the X_NUCLEO_IKS01A1 shield (sensors) 
    - Plug the  expansion board (I-NUCLEO-LRWAN1 or MDM32L07X01) on the NUCLEO-L053R8 board board   
    - Connect the Nucleo board to your PC with a USB cable type A to micro-B 
      to ST-LINK connector (CN1).
    - Please ensure that the ST-LINK connector CN2 jumpers are fitted.


  -Set Up:


     -----------------                      -----------------------  V    V  ----------------------
     |  AT-Master    |                      |      LoRa Modem     |  |    |  |      LoRa Network  |
     |  Application  |                      |                     |  |    |  |                    |
     |               |<--AT_CMD over UART-->|                     |--|    |--|                    |-->Web Server
     |               |                      |                     |          |                    |
     -----------------                      -----------------------          ----------------------

@par How to use it ? 
In order to make the program work, you must do the following :
  - Open your preferred toolchain 
  - Rebuild all files and load your image into target memory (select the right workspace following the expansion board I_NUCLEO_LRWAN1 or MDM32L07X01)
  - Run the example
  - Note : If needed and in order to spy the communication between AT_MAster and LoRa modem 
    - Open an Hyperterminal Terminal and connect it (could be an FTDI cable) to the "D1:LPUART1_RX/D0:LPUART1_TX" of the LoRa Modem
    - UART Config = 115200, 8b, 1 stopbit, no parity, no flow control
    - Terminal Config: Select 'CR+LF' for Transmit New-Line and switch 'Local echo' on
   
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */