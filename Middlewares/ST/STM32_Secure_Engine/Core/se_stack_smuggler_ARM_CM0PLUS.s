;/******************************************************************************
;* File Name          : se_stack_smuggler_ARM_CM0PLUS.s
;* Author             : MCD Application Team
;* Description        : Switch SP from SB to SE RAM region.
;********************************************************************************
;* @attention
;*
;* Copyright (c) 2017 STMicroelectronics. All rights reserved.
;*
;*  This software component is licensed by ST under Ultimate Liberty license
;*  SLA0044, the "License"; You may not use this file except in compliance with
;*  the License. You may obtain a copy of the License at:
;*                              www.st.com/SLA0044
;*
;******************************************************************************
  AREA |.text|, CODE
  EXPORT SE_SP_SMUGGLE
;SE_SP_SMUGGLE(SE_FunctionIDTypeDef eID, SE_StatusTypeDef *peSE_Status, ...)
;R0 and R1 are used to call with new stack SE_CallGateService
  PRESERVE8    {TRUE}
  IMPORT |Image$$SE_region_SRAM1$$Base|
  IMPORT SE_CallGateService
SE_SP_SMUGGLE
; SP - 8
  PUSH {R6, R7, LR}
; retrieve SP value on R7
  MOV R7, SP
; CHANGE SP
  LDR R6, =|Image$$SE_region_SRAM1$$Base|
  MOV SP, R6
; Let 4 bytes to store appli vector
  SUB SP, SP, #4
; push R7 on new stack
  PUSH {R7}
  BL SE_CallGateService
; retrieve previous stack
  POP {R7}
; put new stack
  MOV SP, R7
; return
  POP {R6, R7, PC}
  ALIGN   4      ; now aligned on 4-byte boundary
  END
