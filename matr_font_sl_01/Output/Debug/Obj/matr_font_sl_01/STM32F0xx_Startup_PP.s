# 0 "X:\\_github\\font_test\\matr_font_sl_01\\STM32F0xx\\Source\\STM32F0xx_Startup.s"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "X:\\_github\\font_test\\matr_font_sl_01\\STM32F0xx\\Source\\STM32F0xx_Startup.s"
# 84 "X:\\_github\\font_test\\matr_font_sl_01\\STM32F0xx\\Source\\STM32F0xx_Startup.s"
        .syntax unified
# 108 "X:\\_github\\font_test\\matr_font_sl_01\\STM32F0xx\\Source\\STM32F0xx_Startup.s"
        .global reset_handler
        .global Reset_Handler
        .equ reset_handler, Reset_Handler
        .section .init.Reset_Handler, "ax"
        .balign 2
        .thumb_func
Reset_Handler:




        bl SystemInit
# 175 "X:\\_github\\font_test\\matr_font_sl_01\\STM32F0xx\\Source\\STM32F0xx_Startup.s"
        bl _start





        .weak SystemInit





        .section .init_array, "aw"
        .balign 4
        .word SystemCoreClockUpdate
# 212 "X:\\_github\\font_test\\matr_font_sl_01\\STM32F0xx\\Source\\STM32F0xx_Startup.s"
        .weak HardFault_Handler
        .section .init.HardFault_Handler, "ax"
        .balign 2
        .thumb_func
HardFault_Handler:



        ldr R1, =0xE000ED2C
        ldr R2, [R1]
        cmp R2, #0

.LHardFault_Handler_hfLoop:
        bmi .LHardFault_Handler_hfLoop




        movs R0, #4
        mov R1, LR
        tst R0, R1
        bne .LHardFault_Handler_Uses_PSP
        mrs R0, MSP
        b .LHardFault_Handler_Pass_StackPtr
.LHardFault_Handler_Uses_PSP:
        mrs R0, PSP
.LHardFault_Handler_Pass_StackPtr:
# 249 "X:\\_github\\font_test\\matr_font_sl_01\\STM32F0xx\\Source\\STM32F0xx_Startup.s"
        movs R3, #1
        lsls R3, R3, #31
        orrs R2, R3
        str R2, [R1]







        ldr R1, [R0, #24]
        adds R1, #2
        str R1, [R0, #24]

        bx LR
