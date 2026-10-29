;******************************************************************************
; startup_bcr32.s - BCR32 (GD32F405RE) startup for the bare-metal demo
; ARMCC 5 / armasm. The image is started by the BCR32 bootloader, not by the
; hardware reset: the bootloader reads the SW info root at 0x08010000 and calls
; SwInfo.fnSoftwareEntry (Reset_Handler). Reset_Handler therefore sets VTOR and
; MSP itself and quiesces what the bootloader may have left running.
;******************************************************************************

Stack_Size      EQU     0x00000800          ; MSP: main() + all ISRs
Heap_Size       EQU     0x00000000          ; no malloc

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB

;------------------------------------------------------------------------------
; SW info block, checked by the bootloader
;------------------------------------------------------------------------------
                AREA    SWINFO_ROOT, DATA, READONLY, ALIGN=2
                DCD     SwInfo                      ; root pointer at 0x08010000

                AREA    SWINFO, DATA, READONLY, ALIGN=2
SwInfo          DCD     Reset_Handler               ; fnSoftwareEntry
                DCD     0x0B0C3204                  ; ui32FlashState (FLASH_AREA_VALID_CONTENT)
                DCW     0                           ; ui16ProgramCheckSum (PROGRAM_ROM_CHECKSUM)
                DCW     0                           ; ui16SwRevisionNo
                DCB     1, 0, 0, 1                  ; vnDeviceInfo: main, sub, default mode, device num

;------------------------------------------------------------------------------
; Vector table: 98 entries, VTOR needs 512 byte alignment
;------------------------------------------------------------------------------
                AREA    RESET, DATA, READONLY, ALIGN=9
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp                ; Top of Stack
                DCD     Reset_Handler               ; Reset
                DCD     NMI_Handler                 ; NMI
                DCD     HardFault_Handler           ; HardFault
                DCD     MemManage_Handler           ; MemManage
                DCD     BusFault_Handler            ; BusFault
                DCD     UsageFault_Handler          ; UsageFault
                DCD     0                           ; Reserved
                DCD     0                           ; Reserved
                DCD     0                           ; Reserved
                DCD     0                           ; Reserved
                DCD     SVC_Handler                 ; SVCall
                DCD     DebugMon_Handler            ; Debug Monitor
                DCD     0                           ; Reserved
                DCD     PendSV_Handler              ; PendSV
                DCD     SysTick_Handler             ; SysTick
                DCD     WWDGT_IRQHandler            ; 16
                DCD     LVD_IRQHandler              ; 17
                DCD     TAMPER_STAMP_IRQHandler     ; 18
                DCD     RTC_WKUP_IRQHandler         ; 19
                DCD     FMC_IRQHandler              ; 20
                DCD     RCU_CTC_IRQHandler          ; 21
                DCD     EXTI0_IRQHandler            ; 22
                DCD     EXTI1_IRQHandler            ; 23
                DCD     EXTI2_IRQHandler            ; 24
                DCD     EXTI3_IRQHandler            ; 25
                DCD     EXTI4_IRQHandler            ; 26
                DCD     DMA0_Channel0_IRQHandler    ; 27
                DCD     DMA0_Channel1_IRQHandler    ; 28
                DCD     DMA0_Channel2_IRQHandler    ; 29
                DCD     DMA0_Channel3_IRQHandler    ; 30
                DCD     DMA0_Channel4_IRQHandler    ; 31
                DCD     DMA0_Channel5_IRQHandler    ; 32
                DCD     DMA0_Channel6_IRQHandler    ; 33
                DCD     ADC_IRQHandler              ; 34
                DCD     CAN0_TX_IRQHandler          ; 35
                DCD     CAN0_RX0_IRQHandler         ; 36
                DCD     CAN0_RX1_IRQHandler         ; 37
                DCD     CAN0_EWMC_IRQHandler        ; 38
                DCD     EXTI5_9_IRQHandler          ; 39
                DCD     TIMER0_BRK_TIMER8_IRQHandler ; 40
                DCD     TIMER0_UP_TIMER9_IRQHandler ; 41
                DCD     TIMER0_TRG_CMT_TIMER10_IRQHandler ; 42
                DCD     TIMER0_Channel_IRQHandler   ; 43
                DCD     TIMER1_IRQHandler           ; 44
                DCD     TIMER2_IRQHandler           ; 45
                DCD     TIMER3_IRQHandler           ; 46
                DCD     I2C0_EV_IRQHandler          ; 47
                DCD     I2C0_ER_IRQHandler          ; 48
                DCD     I2C1_EV_IRQHandler          ; 49
                DCD     I2C1_ER_IRQHandler          ; 50
                DCD     SPI0_IRQHandler             ; 51
                DCD     SPI1_IRQHandler             ; 52
                DCD     USART0_IRQHandler           ; 53
                DCD     USART1_IRQHandler           ; 54
                DCD     USART2_IRQHandler           ; 55
                DCD     EXTI10_15_IRQHandler        ; 56
                DCD     RTC_Alarm_IRQHandler        ; 57
                DCD     USBFS_WKUP_IRQHandler       ; 58
                DCD     TIMER7_BRK_TIMER11_IRQHandler ; 59
                DCD     TIMER7_UP_TIMER12_IRQHandler ; 60
                DCD     TIMER7_TRG_CMT_TIMER13_IRQHandler ; 61
                DCD     TIMER7_Channel_IRQHandler   ; 62
                DCD     DMA0_Channel7_IRQHandler    ; 63
                DCD     0                           ; 64: reserved
                DCD     SDIO_IRQHandler             ; 65
                DCD     TIMER4_IRQHandler           ; 66
                DCD     SPI2_IRQHandler             ; 67
                DCD     UART3_IRQHandler            ; 68
                DCD     UART4_IRQHandler            ; 69
                DCD     TIMER5_DAC_IRQHandler       ; 70
                DCD     TIMER6_IRQHandler           ; 71
                DCD     DMA1_Channel0_IRQHandler    ; 72
                DCD     DMA1_Channel1_IRQHandler    ; 73
                DCD     DMA1_Channel2_IRQHandler    ; 74
                DCD     DMA1_Channel3_IRQHandler    ; 75
                DCD     DMA1_Channel4_IRQHandler    ; 76
                DCD     0                           ; 77: reserved
                DCD     0                           ; 78: reserved
                DCD     CAN1_TX_IRQHandler          ; 79
                DCD     CAN1_RX0_IRQHandler         ; 80
                DCD     CAN1_RX1_IRQHandler         ; 81
                DCD     CAN1_EWMC_IRQHandler        ; 82
                DCD     USBFS_IRQHandler            ; 83
                DCD     DMA1_Channel5_IRQHandler    ; 84
                DCD     DMA1_Channel6_IRQHandler    ; 85
                DCD     DMA1_Channel7_IRQHandler    ; 86
                DCD     USART5_IRQHandler           ; 87
                DCD     I2C2_EV_IRQHandler          ; 88
                DCD     I2C2_ER_IRQHandler          ; 89
                DCD     USBHS_EP1_Out_IRQHandler    ; 90
                DCD     USBHS_EP1_In_IRQHandler     ; 91
                DCD     USBHS_WKUP_IRQHandler       ; 92
                DCD     USBHS_IRQHandler            ; 93
                DCD     DCI_IRQHandler              ; 94
                DCD     0                           ; 95: reserved
                DCD     TRNG_IRQHandler             ; 96
                DCD     FPU_IRQHandler              ; 97
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

;------------------------------------------------------------------------------
                AREA    |.text|, CODE, READONLY

Reset_Handler   PROC
                EXPORT  Reset_Handler
                IMPORT  SystemInit
                IMPORT  __main
                CPSID   i                           ; main() enables IRQs after SysTick setup
                LDR     r0, =0xE000ED08             ; SCB->VTOR
                LDR     r1, =__Vectors
                STR     r1, [r0]
                LDR     r0, =__initial_sp           ; own stack, not the one left by the bootloader
                MSR     MSP, r0
                MOVS    r0, #0
                MSR     CONTROL, r0                 ; thread mode privileged, MSP, no FPCA
                ISB
                LDR     r0, =0xE000E010             ; SysTick->CTRL: stop the bootloader tick
                MOVS    r1, #0
                STR     r1, [r0]
                MVNS    r1, r1                      ; 0xFFFFFFFF
                LDR     r0, =0xE000E180             ; NVIC->ICER[0..2]: disable all IRQs
                STR     r1, [r0, #0]
                STR     r1, [r0, #4]
                STR     r1, [r0, #8]
                LDR     r0, =0xE000E280             ; NVIC->ICPR[0..2]: clear pending IRQs
                STR     r1, [r0, #0]
                STR     r1, [r0, #4]
                STR     r1, [r0, #8]
                LDR     r0, =0xE000ED88             ; SCB->CPACR: CP10/CP11 full access before
                LDR     r1, [r0]                    ; any FP instruction in __main
                ORR     r1, r1, #0x00F00000
                STR     r1, [r0]
                DSB
                ISB
                LDR     r0, =SystemInit             ; clocks: 120 MHz PLL from 8 MHz HXTAL
                BLX     r0
                LDR     r0, =__main
                BX      r0
                ENDP

; Dummy exception handlers (weak, override in C)
NMI_Handler     PROC
                EXPORT  NMI_Handler                 [WEAK]
                B       .
                ENDP
HardFault_Handler PROC
                EXPORT  HardFault_Handler           [WEAK]
                B       .
                ENDP
MemManage_Handler PROC
                EXPORT  MemManage_Handler           [WEAK]
                B       .
                ENDP
BusFault_Handler PROC
                EXPORT  BusFault_Handler            [WEAK]
                B       .
                ENDP
UsageFault_Handler PROC
                EXPORT  UsageFault_Handler          [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                 [WEAK]
                B       .
                ENDP
DebugMon_Handler PROC
                EXPORT  DebugMon_Handler            [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler              [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler             [WEAK]
                B       .
                ENDP

Default_Handler PROC
                EXPORT  WWDGT_IRQHandler            [WEAK]
                EXPORT  LVD_IRQHandler              [WEAK]
                EXPORT  TAMPER_STAMP_IRQHandler     [WEAK]
                EXPORT  RTC_WKUP_IRQHandler         [WEAK]
                EXPORT  FMC_IRQHandler              [WEAK]
                EXPORT  RCU_CTC_IRQHandler          [WEAK]
                EXPORT  EXTI0_IRQHandler            [WEAK]
                EXPORT  EXTI1_IRQHandler            [WEAK]
                EXPORT  EXTI2_IRQHandler            [WEAK]
                EXPORT  EXTI3_IRQHandler            [WEAK]
                EXPORT  EXTI4_IRQHandler            [WEAK]
                EXPORT  DMA0_Channel0_IRQHandler    [WEAK]
                EXPORT  DMA0_Channel1_IRQHandler    [WEAK]
                EXPORT  DMA0_Channel2_IRQHandler    [WEAK]
                EXPORT  DMA0_Channel3_IRQHandler    [WEAK]
                EXPORT  DMA0_Channel4_IRQHandler    [WEAK]
                EXPORT  DMA0_Channel5_IRQHandler    [WEAK]
                EXPORT  DMA0_Channel6_IRQHandler    [WEAK]
                EXPORT  ADC_IRQHandler              [WEAK]
                EXPORT  CAN0_TX_IRQHandler          [WEAK]
                EXPORT  CAN0_RX0_IRQHandler         [WEAK]
                EXPORT  CAN0_RX1_IRQHandler         [WEAK]
                EXPORT  CAN0_EWMC_IRQHandler        [WEAK]
                EXPORT  EXTI5_9_IRQHandler          [WEAK]
                EXPORT  TIMER0_BRK_TIMER8_IRQHandler [WEAK]
                EXPORT  TIMER0_UP_TIMER9_IRQHandler [WEAK]
                EXPORT  TIMER0_TRG_CMT_TIMER10_IRQHandler [WEAK]
                EXPORT  TIMER0_Channel_IRQHandler   [WEAK]
                EXPORT  TIMER1_IRQHandler           [WEAK]
                EXPORT  TIMER2_IRQHandler           [WEAK]
                EXPORT  TIMER3_IRQHandler           [WEAK]
                EXPORT  I2C0_EV_IRQHandler          [WEAK]
                EXPORT  I2C0_ER_IRQHandler          [WEAK]
                EXPORT  I2C1_EV_IRQHandler          [WEAK]
                EXPORT  I2C1_ER_IRQHandler          [WEAK]
                EXPORT  SPI0_IRQHandler             [WEAK]
                EXPORT  SPI1_IRQHandler             [WEAK]
                EXPORT  USART0_IRQHandler           [WEAK]
                EXPORT  USART1_IRQHandler           [WEAK]
                EXPORT  USART2_IRQHandler           [WEAK]
                EXPORT  EXTI10_15_IRQHandler        [WEAK]
                EXPORT  RTC_Alarm_IRQHandler        [WEAK]
                EXPORT  USBFS_WKUP_IRQHandler       [WEAK]
                EXPORT  TIMER7_BRK_TIMER11_IRQHandler [WEAK]
                EXPORT  TIMER7_UP_TIMER12_IRQHandler [WEAK]
                EXPORT  TIMER7_TRG_CMT_TIMER13_IRQHandler [WEAK]
                EXPORT  TIMER7_Channel_IRQHandler   [WEAK]
                EXPORT  DMA0_Channel7_IRQHandler    [WEAK]
                EXPORT  SDIO_IRQHandler             [WEAK]
                EXPORT  TIMER4_IRQHandler           [WEAK]
                EXPORT  SPI2_IRQHandler             [WEAK]
                EXPORT  UART3_IRQHandler            [WEAK]
                EXPORT  UART4_IRQHandler            [WEAK]
                EXPORT  TIMER5_DAC_IRQHandler       [WEAK]
                EXPORT  TIMER6_IRQHandler           [WEAK]
                EXPORT  DMA1_Channel0_IRQHandler    [WEAK]
                EXPORT  DMA1_Channel1_IRQHandler    [WEAK]
                EXPORT  DMA1_Channel2_IRQHandler    [WEAK]
                EXPORT  DMA1_Channel3_IRQHandler    [WEAK]
                EXPORT  DMA1_Channel4_IRQHandler    [WEAK]
                EXPORT  CAN1_TX_IRQHandler          [WEAK]
                EXPORT  CAN1_RX0_IRQHandler         [WEAK]
                EXPORT  CAN1_RX1_IRQHandler         [WEAK]
                EXPORT  CAN1_EWMC_IRQHandler        [WEAK]
                EXPORT  USBFS_IRQHandler            [WEAK]
                EXPORT  DMA1_Channel5_IRQHandler    [WEAK]
                EXPORT  DMA1_Channel6_IRQHandler    [WEAK]
                EXPORT  DMA1_Channel7_IRQHandler    [WEAK]
                EXPORT  USART5_IRQHandler           [WEAK]
                EXPORT  I2C2_EV_IRQHandler          [WEAK]
                EXPORT  I2C2_ER_IRQHandler          [WEAK]
                EXPORT  USBHS_EP1_Out_IRQHandler    [WEAK]
                EXPORT  USBHS_EP1_In_IRQHandler     [WEAK]
                EXPORT  USBHS_WKUP_IRQHandler       [WEAK]
                EXPORT  USBHS_IRQHandler            [WEAK]
                EXPORT  DCI_IRQHandler              [WEAK]
                EXPORT  TRNG_IRQHandler             [WEAK]
                EXPORT  FPU_IRQHandler              [WEAK]
WWDGT_IRQHandler
LVD_IRQHandler
TAMPER_STAMP_IRQHandler
RTC_WKUP_IRQHandler
FMC_IRQHandler
RCU_CTC_IRQHandler
EXTI0_IRQHandler
EXTI1_IRQHandler
EXTI2_IRQHandler
EXTI3_IRQHandler
EXTI4_IRQHandler
DMA0_Channel0_IRQHandler
DMA0_Channel1_IRQHandler
DMA0_Channel2_IRQHandler
DMA0_Channel3_IRQHandler
DMA0_Channel4_IRQHandler
DMA0_Channel5_IRQHandler
DMA0_Channel6_IRQHandler
ADC_IRQHandler
CAN0_TX_IRQHandler
CAN0_RX0_IRQHandler
CAN0_RX1_IRQHandler
CAN0_EWMC_IRQHandler
EXTI5_9_IRQHandler
TIMER0_BRK_TIMER8_IRQHandler
TIMER0_UP_TIMER9_IRQHandler
TIMER0_TRG_CMT_TIMER10_IRQHandler
TIMER0_Channel_IRQHandler
TIMER1_IRQHandler
TIMER2_IRQHandler
TIMER3_IRQHandler
I2C0_EV_IRQHandler
I2C0_ER_IRQHandler
I2C1_EV_IRQHandler
I2C1_ER_IRQHandler
SPI0_IRQHandler
SPI1_IRQHandler
USART0_IRQHandler
USART1_IRQHandler
USART2_IRQHandler
EXTI10_15_IRQHandler
RTC_Alarm_IRQHandler
USBFS_WKUP_IRQHandler
TIMER7_BRK_TIMER11_IRQHandler
TIMER7_UP_TIMER12_IRQHandler
TIMER7_TRG_CMT_TIMER13_IRQHandler
TIMER7_Channel_IRQHandler
DMA0_Channel7_IRQHandler
SDIO_IRQHandler
TIMER4_IRQHandler
SPI2_IRQHandler
UART3_IRQHandler
UART4_IRQHandler
TIMER5_DAC_IRQHandler
TIMER6_IRQHandler
DMA1_Channel0_IRQHandler
DMA1_Channel1_IRQHandler
DMA1_Channel2_IRQHandler
DMA1_Channel3_IRQHandler
DMA1_Channel4_IRQHandler
CAN1_TX_IRQHandler
CAN1_RX0_IRQHandler
CAN1_RX1_IRQHandler
CAN1_EWMC_IRQHandler
USBFS_IRQHandler
DMA1_Channel5_IRQHandler
DMA1_Channel6_IRQHandler
DMA1_Channel7_IRQHandler
USART5_IRQHandler
I2C2_EV_IRQHandler
I2C2_ER_IRQHandler
USBHS_EP1_Out_IRQHandler
USBHS_EP1_In_IRQHandler
USBHS_WKUP_IRQHandler
USBHS_IRQHandler
DCI_IRQHandler
TRNG_IRQHandler
FPU_IRQHandler
                B       .
                ENDP

                ALIGN

;------------------------------------------------------------------------------
; Stack/heap for the C library (microlib)
;------------------------------------------------------------------------------
                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                END
