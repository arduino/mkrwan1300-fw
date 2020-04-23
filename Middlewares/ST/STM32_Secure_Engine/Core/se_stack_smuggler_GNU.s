/**
  ******************************************************************************
  * @file      se_stack_smuggler_GNU.s
  * @author    MCD Application Team
  * @brief     Switch SP from SB to SE RAM region.
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
  .section .text
  .global SE_SP_SMUGGLE
  .syntax unified
  .thumb
//SE_SP_SMUGGLE(SE_FunctionIDTypeDef eID, SE_StatusTypeDef *peSE_Status, ...)
//R0 and R1 are used to call with new stack SE_CallGateService
  .global __ICFEDIT_SE_region_SRAM1_stack_top__
  .global SE_CallGateService
SE_SP_SMUGGLE:
// SP - 8
  PUSH {R11,LR}
// retrieve SP value on R11
  MOV R11, SP
// CHANGE SP
  LDR SP, =__ICFEDIT_SE_region_SRAM1_stack_top__
// Let 4 byte to store appli vector addres
  SUB SP, SP, #4  
// push R11 on new stack
  PUSH {R11}
  BLX SE_CallGateService
// retrieve previous stack
  POP {R11}
// put new stack
  MOV SP, R11
  POP {R11, LR}
// return
  BX LR
  .end
