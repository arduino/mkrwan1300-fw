;/******************************************************************************
;* File Name          : se_stack_smuggler_ARM.s
;* Author             : MCD Application Team
;* Description        : Switch SP from SB to SE RAM region.
;*******************************************************************************
;*  @attention
;* 
;*  <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
;*  All rights reserved.</center></h2>
;* 
;*  This software component is licensed by ST under Ultimate Liberty license
;*  SLA0044, the "License"; You may not use this file except in compliance with
;*  the License. You may obtain a copy of the License at:
;*                              www.st.com/SLA0044
;* 
;*******************************************************************************
;
  AREA |.text|, CODE
  EXPORT SE_SP_SMUGGLE
;SE_SP_SMUGGLE(SE_FunctionIDTypeDef eID, SE_StatusTypeDef *peSE_Status, ...)
;R0 and R1 are used to call with new stack SE_CallGateService
  PRESERVE8    {TRUE}
  IMPORT |Image$$SE_region_SRAM1$$Base|
  IMPORT SE_CallGateService
SE_SP_SMUGGLE
; SP - 8
  PUSH {R11,LR}
; retrieve SP value on R11
  MOV R11, SP
; CHANGE SP
  LDR SP, =|Image$$SE_region_SRAM1$$Base|
; Let 4 bytes to store appli vector 
  SUB SP, SP, #4
; push R11 on new stack
  PUSH {R11}
  BL SE_CallGateService
; retrieve previous stack
  POP {R11}
; put new stack
  MOV SP, R11
  POP {R11, LR}
; return
  BX LR
  ALIGN   4      ; now aligned on 4-byte boundary
  END
