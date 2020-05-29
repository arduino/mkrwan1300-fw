#
# Copyright 2018 Fabio Baltieri (fabio.baltieri@gmail.com)
#
# Based on the original ben-wpan code written by:
#   Werner Almesberger, Copyright 2010-2011
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
SHELL := /bin/bash

NAME ?= mlm32l07x01

CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE = $(CROSS_COMPILE)size

FLASH = st-flash
FLASHFLAGS = --reset --format ihex
BOOTLOADER = dfu-util
BOOTLOADER_FLAGS = -a 0 -s 0x08000000:leave

LINKER_SCRIPT = Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/SW4STM32/mlm32l07x01/STM32L072CZYx_FLASH.ld

CFLAGS  = -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -Os

LDFLAGS = -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -specs=nosys.specs -specs=nano.specs -T$(LINKER_SCRIPT) -lm

DEFINES = \
	  -DSTM32L072xx \
	  -DUSE_B_L072Z_LRWAN1 \
	  -DUSE_FULL_LL_DRIVER \
	  -DREGION_EU868 \
	  -DREGION_AS923 \
	  -DREGION_AU915 \
	  -DREGION_CN470 \
	  -DREGION_CN779 \
	  -DREGION_EU433 \
	  -DREGION_IN865 \
	  -DREGION_KR920 \
	  -DREGION_US915 \
	  -DREGION_RU864 \
    -DLORAMAC_CLASSB_ENABLED

INCLUDES = \
     -IProjects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/LoRaWAN/App/LoRaWAN/App/inc \
     -IProjects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/LoRaWAN/App/Core/inc \
     -IProjects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/LoRaWAN/App/inc \
     -IProjects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/Core/inc \
	   -IDrivers/BSP/CMWX1ZZABZ-0xx \
	   -IDrivers/STM32L0xx_HAL_Driver/Inc \
	   -IDrivers/CMSIS/Device/ST/STM32L0xx/Include \
	   -IDrivers/CMSIS/Include \
	   -IMiddlewares/Third_Party/LoRaWAN/Crypto \
	   -IMiddlewares/Third_Party/LoRaWAN/Mac \
	   -IMiddlewares/Third_Party/LoRaWAN/Phy \
	   -IMiddlewares/Third_Party/LoRaWAN/Utilities \
	   -IMiddlewares/Third_Party/LoRaWAN/Core \
     -IMiddlewares/Third_Party/LoRaWAN/Patterns/Basic \
	   -IDrivers/BSP/Components/Common \
	   -IDrivers/BSP/Components/sx1276 \
	   -IDrivers/BSP/B-L072Z-LRWAN1

OBJS = \
       Drivers/CMSIS/Device/ST/STM32L0xx/Source/Templates/system_stm32l0xx.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_adc.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_gpio.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_lpuart.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_rcc.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_rtc.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_spi.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_dma.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart_ex.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc_ex.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_gpio.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_adc.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_adc_ex.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr_ex.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_spi.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rtc.o \
       Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rtc_ex.o \
       Drivers/BSP/Components/sx1276/sx1276.o \
       Drivers/BSP/CMWX1ZZABZ-0xx/mlm32l07x01.o \
       Drivers/BSP/B-L072Z-LRWAN1/b-l072z-lrwan1.o \
       Middlewares/Third_Party/LoRaWAN/Crypto/soft-se.o \
       Middlewares/Third_Party/LoRaWAN/Crypto/aes.o \
       Middlewares/Third_Party/LoRaWAN/Crypto/cmac.o \
       Middlewares/Third_Party/LoRaWAN/Mac/LoRaMac.o \
       Middlewares/Third_Party/LoRaWAN/Mac/LoRaMacCrypto.o \
       Middlewares/Third_Party/LoRaWAN/Mac/LoRaMacClassB.o \
       Middlewares/Third_Party/LoRaWAN/Mac/LoRaMacConfirmQueue.o \
       Middlewares/Third_Party/LoRaWAN/Mac/region/Region.o \
       Middlewares/Third_Party/LoRaWAN/Mac/region/RegionAS923.o \
       Middlewares/Third_Party/LoRaWAN/Mac/region/RegionAU915.o \
       Middlewares/Third_Party/LoRaWAN/Mac/region/RegionCN470.o \
       Middlewares/Third_Party/LoRaWAN/Mac/region/RegionCN779.o \
       Middlewares/Third_Party/LoRaWAN/Mac/region/RegionCommon.o \
       Middlewares/Third_Party/LoRaWAN/Mac/region/RegionEU433.o \
       Middlewares/Third_Party/LoRaWAN/Mac/region/RegionEU868.o \
       Middlewares/Third_Party/LoRaWAN/Mac/region/RegionIN865.o \
       Middlewares/Third_Party/LoRaWAN/Mac/region/RegionKR920.o \
       Middlewares/Third_Party/LoRaWAN/Mac/region/RegionUS915.o \
       Middlewares/Third_Party/LoRaWAN/Mac/region/RegionRU864.o \
       Middlewares/Third_Party/LoRaWAN/Mac/LoRaMacParser.o \
       Middlewares/Third_Party/LoRaWAN/Mac/LoRaMacSerializer.o \
       Middlewares/Third_Party/LoRaWAN/Mac/LoRaMacCommands.o \
       Middlewares/Third_Party/LoRaWAN/Mac/LoRaMacAdr.o \
       Middlewares/Third_Party/LoRaWAN/Utilities/low_power_manager.o \
       Middlewares/Third_Party/LoRaWAN/Utilities/timeServer.o \
       Middlewares/Third_Party/LoRaWAN/Utilities/utilities.o \
       Middlewares/Third_Party/LoRaWAN/Utilities/trace.o \
       Middlewares/Third_Party/LoRaWAN/Utilities/queue.o \
       Middlewares/Third_Party/LoRaWAN/Utilities/systime.o \
       Middlewares/Third_Party/LoRaWAN/Patterns/Basic/lora-test.o \
       Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/LoRaWAN/App/src/at.o \
       Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/LoRaWAN/App/src/command.o \
       Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/LoRaWAN/App/src/debug.o \
       Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/LoRaWAN/App/src/hw_gpio.o \
       Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/LoRaWAN/App/src/hw_rtc.o \
       Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/LoRaWAN/App/src/hw_spi.o \
       Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/LoRaWAN/App/src/lora.o \
       Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/LoRaWAN/App/src/main.o \
       Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/Core/src/mlm32l0xx_hw.o \
       Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/Core/src/mlm32l0xx_hal_msp.o \
       Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/Core/src/mlm32l0xx_it.o \
       Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/LoRaWAN/App/src/test_rf.o \
       Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/LoRaWAN/App/src/tiny_sscanf.o \
       Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/LoRaWAN/App/src/tiny_vsnprintf.o \
       Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/LoRaWAN/App/src/vcom.o \
       Projects/B-L072Z-LRWAN1/Applications/LoRa/AT_Slave/SW4STM32/startup_stm32l072xx.o


# ----- Verbosity control -----------------------------------------------------

CC_normal	:= $(CC)
BUILD_normal	:=
DEPEND_normal	:= $(CC) -MM -MG

CC_quiet	= @echo "  CC       " $@ && $(CC_normal)
BUILD_quiet	= @echo "  BUILD    " $@ && $(BUILD_normal)
DEPEND_quiet	= @$(DEPEND_normal)

ifeq ($(V),1)
    CC		= $(CC_normal)
    BUILD	= $(BUILD_normal)
    DEPEND	= $(DEPEND_normal)
else
    CC		= $(CC_quiet)
    BUILD	= $(BUILD_quiet)
    DEPEND	= $(DEPEND_quiet)
endif

# ----- Rules -----------------------------------------------------------------

.PHONY:		all clean

all:		$(NAME).bin $(NAME).hex fw.h

$(NAME).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)
	$(SIZE) $@

%.bin: %.elf
	$(BUILD) $(OBJCOPY) -j .text -j .data -O binary $< $@

%.hex: %.elf
	$(BUILD) $(OBJCOPY) -j .text -j .data -O ihex $< $@

fw.h: $(NAME).bin
	echo -n "const " > fw.h && xxd -i $(NAME).bin >> fw.h

# ----- Cleanup ---------------------------------------------------------------

clean:
		rm -f $(NAME).bin $(NAME).elf $(NAME).hex
		rm -f $(NAME)_text.{bin,hex}
		rm -f $(OBJS) $(OBJS:.o=.d)
		rm -f *~

# ----- Dependencies ----------------------------------------------------------

MKDEP =									\
	$(DEPEND) $(CFLAGS) $(DEFINES) $(INCLUDES) $< |							\
	  sed 								\
	    -e 's|^$(basename $(notdir $<)).o:|$@:|'			\
	    -e '/^\(.*:\)\? */{p;s///;s/ *\\\?$$/ /;s/  */:\n/g;H;}'	\
	    -e '$${g;p;}'						\
	    -e d >$(basename $@).d;					\
	  [ "$${PIPESTATUS[*]}" = "0 0" ] ||				\
	  { rm -f $(basename $@).d; exit 1; }

%.o: %.c
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@
	$(MKDEP)

%.o: %.S
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__ASSEMBLY__ -c $< -o $@
	$(MKDEP)

-include $(OBJS:.o=.d)

# ----- Programming and device control ----------------------------------------

.PHONY: load boot

load: $(NAME).hex
	$(FLASH) $(FLASHFLAGS) write $(NAME).hex

boot: $(NAME).bin
	$(BOOTLOADER) $(BOOTLOADER_FLAGS) -D $(NAME).bin
