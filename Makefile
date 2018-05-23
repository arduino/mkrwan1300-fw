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

NAME = main

CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE = $(CROSS_COMPILE)size

FLASH = st-flash
FLASHFLAGS = --reset --format ihex
BOOTLOADER = dfu-util
BOOTLOADER_FLAGS = -a 0 -s 0x08000000:leave

LINKER_SCRIPT = Projects/Multi/Applications/LoRa/AT_Slave/SW4STM32/B-L072Z-LRWAN1/mlm32l07x01/STM32L072CZYx_FLASH.ld

CFLAGS  = -g -Os -Wall -Wextra -Wno-unused-parameter \
	  -mcpu=cortex-m0plus -mthumb \
	  -std=c99 -ffunction-sections -fdata-sections -march=armv6-m -mthumb -mabi=aapcs -mfloat-abi=soft
LDFLAGS = \
	  -Wl,--gc-sections,--no-undefined \
	  -T$(LINKER_SCRIPT) --specs=nosys.specs \
	  -lm -lc -lgcc

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
	  -DREGION_US915_HYBRID

INCLUDES = \
	   -IProjects/Multi/Applications/LoRa/AT_Slave/inc \
	   -IDrivers/BSP/MLM32L07X01 \
	   -IDrivers/STM32L0xx_HAL_Driver/Inc \
	   -IDrivers/CMSIS/Device/ST/STM32L0xx/Include \
	   -IDrivers/CMSIS/Include \
	   -IMiddlewares/Third_Party/Lora/Crypto \
	   -IMiddlewares/Third_Party/Lora/Mac \
	   -IMiddlewares/Third_Party/Lora/Phy \
	   -IMiddlewares/Third_Party/Lora/Utilities \
	   -IMiddlewares/Third_Party/Lora/Core \
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
       Drivers/BSP/Components/sx1276/sx1276.o \
       Drivers/BSP/MLM32L07X01/mlm32l07x01.o \
       Middlewares/Third_Party/Lora/Crypto/aes.o \
       Middlewares/Third_Party/Lora/Crypto/cmac.o \
       Middlewares/Third_Party/Lora/Mac/LoRaMac.o \
       Middlewares/Third_Party/Lora/Mac/LoRaMacCrypto.o \
       Middlewares/Third_Party/Lora/Mac/region/Region.o \
       Middlewares/Third_Party/Lora/Mac/region/RegionAS923.o \
       Middlewares/Third_Party/Lora/Mac/region/RegionAU915.o \
       Middlewares/Third_Party/Lora/Mac/region/RegionCN470.o \
       Middlewares/Third_Party/Lora/Mac/region/RegionCN779.o \
       Middlewares/Third_Party/Lora/Mac/region/RegionCommon.o \
       Middlewares/Third_Party/Lora/Mac/region/RegionEU433.o \
       Middlewares/Third_Party/Lora/Mac/region/RegionEU868.o \
       Middlewares/Third_Party/Lora/Mac/region/RegionIN865.o \
       Middlewares/Third_Party/Lora/Mac/region/RegionKR920.o \
       Middlewares/Third_Party/Lora/Mac/region/RegionUS915-Hybrid.o \
       Middlewares/Third_Party/Lora/Mac/region/RegionUS915.o \
       Middlewares/Third_Party/Lora/Utilities/delay.o \
       Middlewares/Third_Party/Lora/Utilities/low_power.o \
       Middlewares/Third_Party/Lora/Utilities/timeServer.o \
       Middlewares/Third_Party/Lora/Utilities/utilities.o \
       Projects/Multi/Applications/LoRa/AT_Slave/src/at.o \
       Projects/Multi/Applications/LoRa/AT_Slave/src/command.o \
       Projects/Multi/Applications/LoRa/AT_Slave/src/debug.o \
       Projects/Multi/Applications/LoRa/AT_Slave/src/hw_gpio.o \
       Projects/Multi/Applications/LoRa/AT_Slave/src/hw_rtc.o \
       Projects/Multi/Applications/LoRa/AT_Slave/src/hw_spi.o \
       Projects/Multi/Applications/LoRa/AT_Slave/src/lora.o \
       Projects/Multi/Applications/LoRa/AT_Slave/src/main.o \
       Projects/Multi/Applications/LoRa/AT_Slave/src/mlm32l0xx_hw.o \
       Projects/Multi/Applications/LoRa/AT_Slave/src/mlm32l0xx_it.o \
       Projects/Multi/Applications/LoRa/AT_Slave/src/test_rf.o \
       Projects/Multi/Applications/LoRa/AT_Slave/src/tiny_sscanf.o \
       Projects/Multi/Applications/LoRa/AT_Slave/src/tiny_vsnprintf.o \
       Projects/Multi/Applications/LoRa/AT_Slave/src/vcom.o \
       Projects/Multi/Applications/LoRa/AT_Slave/SW4STM32/B-L072Z-LRWAN1/startup_stm32l072xx.o


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

all:		$(NAME)_text.bin $(NAME)_text.hex

$(NAME).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)
	$(SIZE) $@

%_text.bin: %.elf
	$(BUILD) $(OBJCOPY) -j .text -j .data -O binary $< $@

%_text.hex: %.elf
	$(BUILD) $(OBJCOPY) -j .text -j .data -O ihex $< $@

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

load: $(NAME)_text.hex
	$(FLASH) $(FLASHFLAGS) write $(NAME)_text.hex

boot: $(NAME)_text.bin
	$(BOOTLOADER) $(BOOTLOADER_FLAGS) -D $(NAME)_text.bin
