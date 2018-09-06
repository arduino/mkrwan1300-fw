/**
  @page PingPong Readme file
 
  @verbatim
  ******************** (C) COPYRIGHT 2017 STMicroelectronics *******************
  * @file    PingPong/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    08-September-2017
  * @brief   This application is a simple demo application software between two 
  *          LoRa Objects containing a STM32LXxx-Nucleo board and  LoRA Radio expansion board.
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

This directory contains a set of source files that implements a simple ping-pong application 
between two LoRa Objects. The LoRa Object can be 
   - either a STM32LXxx-Nucleo board and Lora Radio expansion board, optionnally a sensor board. 
   - or a B-L072Z-LRWAN1 (available soon)
It is a simple Rx/Tx RF link between the two LoRa Objects. By default, each LoRa Objects starts 
as a master and will transmit a "Ping" message, and then wait for an answer. The first LoRa Object
receiving a "Ping" message will become a slave and answers the master with a "Pong" message. 
The Ping Pong is then started
  ******************************************************************************



@par Directory contents 


  - PingPong/Inc/hw_conf.h                file to manage Cube SW family used and debug switch
  - PingPong/Inc/stm32lXxx_hal_conf.h     Library Configuration file
  - PingPong/Inc/stm32lXxx_it.h           Header for stm32lXxx_it.c
  - PingPong/Inc/stm32lXxx_hw_conf.h      Header for stm32lXxx_hw_conf.c
  - PingPong/Inc/hw_spi.h                 Header for hw_spi.c
  - PingPong/Inc/hw_rtc.h                 Header for hw_rtc.c
  - PingPong/Inc/hw_gpio.h                Header for hw_gpio.c
  - PingPong/Inc/hw.h                     group all hw interface
  - PingPong/Inc/vcom.h                   interface to vcom.c 
  - PingPong/Inc/debug.h                  interface to debug functionally
  
  - PingPong/Src/main.c                   Main program file
  - PingPong/Src/stm32lXxx_it.c           STM32lXxx Interrupt handlers
  - PingPong/Src/stm32lXxx_hal_msp.c      stm32lXxx specific hardware HAL code
  - PingPong/Src/stm32lXxx_hw.c           stm32lXxx specific hardware driver code
  - PingPong/Src/hw_spi.c                 spi driver
  - PingPong/Src/hw_rtc.c                 rtc driver
  - PingPong/Src/hw_gpio.c                gpio driver
  - PingPong/Src/vcom.c                   virtual com port interface on Terminal
  - PingPong/Src/debug.c                  debug driver
 
@par Hardware and Software environment 


  - This example runs on STM32L053R8, STM32L073RZ, STM32L152RE and STM32L476RG devices.
    
  - This application has been tested with STMicroelectronics:
    NUCLEO-L053R8 RevC
    NUCLEO-L073RZ RevC
    NUCLEO-L152RE RevC
    NUCLEO-L476RG RevC 
    B-L072Z-LRWAN1 RevC
    boards and can be easily tailored to any other supported device 
    and development board.

  - STM32LXxx-Nucleo Set-up    
    - Connect the Nucleo board to your PC with a USB cable type A to mini-B 
      to ST-LINK connector (CN1 / CN7 on B-L072Z-LRWAN1).
    - Please ensure that the ST-LINK connector CN2 (CN8 on B-L072Z-LRWAN1) jumpers are fitted.
  -Set Up:


             --------------------------  V    V  --------------------------
             |      LoRa Object       |  |    |  |      LoRa Object       |
             |                        |  |    |  |                        |
   ComPort<--|                        |--|    |--|                        |-->ComPort
             |                        |          |                        |
             --------------------------          --------------------------

@par How to use it ? 
In order to make the program work, you must do the following :
  - Open your preferred toolchain 
  - Rebuild all files and load your image into target memory
  - Run the example
  - Open two Terminals, each connected the respective LoRa Object
  - Terminal Config = 115200bauds, 8b, 1 stopbit, no parity, no flow control
   
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
