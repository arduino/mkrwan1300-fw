/**
  @page AT_Slave Readme file
 
  @verbatim
  ******************************************************************************
  * @file    AT_Slave/readme.txt 
  * @author  MCD Application Team
  * @brief   This application is a simple demo application software of a LoRa
  *          modem controlled though AT command interface over UART by an
  *          external host
  ******************************************************************************
  *
  * Copyright (c) 2018 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                               www.st.com/SLA0044
  *
  ******************************************************************************
   @endverbatim

@par Example Description

This directory contains a set of source files that implements an example of a LoRa
modem that is controlled though AT command interface over UART by an external host,
like a computer executing a terminal.

This application is targeting the B-L072Z-LRWAN1 Discovery board embedding the
CMWX1ZZABZ-091 LoRa module.

This application uses the Cube Low Layer drivers APIs targeting the STM32L0
(embedded in the module) to optimize the code size.
  ******************************************************************************



@par Directory contents 


  - AT_Slave/LoRaWAN/App/inc/at.h                Header for at.c
  - AT_Slave/LoRaWAN/App/inc/command.h           Header for command.c
  - AT_Slave/LoRaWAN/App/inc/Comissioning.h      End device comissioning parameters
  - AT_Slave/LoRaWAN/App/inc/debug.h             interface to debug functionally
  - AT_Slave/LoRaWAN/App/inc/hw.h                group all hw interface
  - AT_Slave/LoRaWAN/App/inc/hw_conf.h           file to manage Cube SW family used and debug switch
  - AT_Slave/LoRaWAN/App/inc/hw_gpio.h           Header for hw_gpio.c
  - AT_Slave/LoRaWAN/App/inc/hw_msp.h            Header for driver hw msp module
  - AT_Slave/LoRaWAN/App/inc/hw_rtc.h            Header for hw_rtc.c
  - AT_Slave/LoRaWAN/App/inc/hw_spi.h            Header for hw_spi.c
  - AT_Slave/LoRaWAN/App/inc/lora.h              Header for lora.c
  - AT_Slave/LoRaWAN/App/inc/test_rf.h           Header for test_rf.c
  - AT_Slave/LoRaWAN/App/inc/tiny_sscanf.h       Header for tiny_sscanf.c
  - AT_Slave/LoRaWAN/App/inc/tiny_vsnprintf.h    Header for tiny_vsnprintf.c
  - AT_Slave/LoRaWAN/App/inc/utilities_conf.h    configuration for utilities
  - AT_Slave/LoRaWAN/App/inc/vcom.h              interface to vcom.c
  - AT_Slave/LoRaWAN/App/inc/version.h           Version file
  - AT_Slave/Core/inc/stm32l0xx_hal_conf.h       Library Configuration file
  - AT_Slave/Core/inc/mlm32l0xx_hw_conf.h        Definitions for mlm32l0xx HW configuration
  - AT_Slave/Core/inc/mlm32l0xx_it.h             Header for mlm32l0xx_it.c
  
  - AT_Slave/LoRaWAN/App/src/at.c                AT commands API
  - AT_Slave/LoRaWAN/App/src/command.c           Definitions of AT commands
  - AT_Slave/LoRaWAN/App/src/debug.c             debug driver
  - AT_Slave/LoRaWAN/App/src/hw_gpio.c           gpio driver
  - AT_Slave/LoRaWAN/App/src/hw_rtc.c            rtc driver
  - AT_Slave/LoRaWAN/App/src/hw_spi.c            spi driver
  - AT_Slave/LoRaWAN/App/src/lora.c              LoRa API to drive the LoRa state machine
  - AT_Slave/LoRaWAN/App/src/main.c              Main program file
  - AT_Slave/LoRaWAN/App/src/test_rf.c           Manages tx tests
  - AT_Slave/LoRaWAN/App/src/tiny_sscanf.c       Tiny implementation of sscanf() like function
  - AT_Slave/LoRaWAN/App/src/tiny_vsnprintf.c    Tiny implementation of vsnprintf() like function
  - AT_Slave/LoRaWAN/App/src/vcom.c              Virtual com port interface on Terminal
  - AT_Slave/Core/src/mlm32l0xx_hal_msp.c        mlm32l0xx specific hardware HAL code
  - AT_Slave/Core/src/mlm32l0xx_hw.c             mlm32l0xx specific hardware driver code
  - AT_Slave/Core/src/mlm32l0xx_it.c             MLM32l0xx Interrupt handlers

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