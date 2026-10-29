/*
 * startup_bcr32_gcc.S - BCR32 startup for GCC (GNU as), same layout as
 * Board/startup_bcr32.s (Keil / ARMCC): keep both in step when one changes.
 *
 * The image is started by the BCR32 bootloader, not by the hardware reset: the bootloader
 * reads the SW info root at 0x08010000 and calls SwInfo.fnSoftwareEntry (Reset_Handler).
 * Reset_Handler therefore sets VTOR and MSP itself and quiesces what the bootloader may
 * have left running.
 */
    .syntax unified
    .cpu cortex-m4
    .fpu fpv4-sp-d16
    .thumb

/* ---- SW info (checked by the bootloader) ---------------------------------------- */
    .section .swinfo_root, "a"
    .align 2
    .word   SwInfo                      /* root pointer at 0x08010000 */

    .section .swinfo, "a"
    .align 2
SwInfo:
    .word   Reset_Handler               /* fnSoftwareEntry */
    .word   0x0B0C3204                  /* ui32FlashState: valid application */
    .short  0                           /* ui16ProgramCheckSum */
    .short  0                           /* ui16SwRevisionNo */
    .byte   1, 0, 0, 1                  /* vnDeviceInfo: main, sub, default mode, device num */

/* ---- Vector table: 98 entries, VTOR needs 512 byte alignment ---------------------- */
    .section .isr_vector, "a"
    .align 9
    .global __Vectors
__Vectors:
    .word   _estack                              /* 0 */
    .word   Reset_Handler                        /* 1 */
    .word   NMI_Handler                          /* 2 */
    .word   HardFault_Handler                    /* 3 */
    .word   MemManage_Handler                    /* 4 */
    .word   BusFault_Handler                     /* 5 */
    .word   UsageFault_Handler                   /* 6 */
    .word   0                                    /* 7 */
    .word   0                                    /* 8 */
    .word   0                                    /* 9 */
    .word   0                                    /* 10 */
    .word   SVC_Handler                          /* 11 */
    .word   DebugMon_Handler                     /* 12 */
    .word   0                                    /* 13 */
    .word   PendSV_Handler                       /* 14 */
    .word   SysTick_Handler                      /* 15 */
    .word   WWDGT_IRQHandler                     /* 16 */
    .word   LVD_IRQHandler                       /* 17 */
    .word   TAMPER_STAMP_IRQHandler              /* 18 */
    .word   RTC_WKUP_IRQHandler                  /* 19 */
    .word   FMC_IRQHandler                       /* 20 */
    .word   RCU_CTC_IRQHandler                   /* 21 */
    .word   EXTI0_IRQHandler                     /* 22 */
    .word   EXTI1_IRQHandler                     /* 23 */
    .word   EXTI2_IRQHandler                     /* 24 */
    .word   EXTI3_IRQHandler                     /* 25 */
    .word   EXTI4_IRQHandler                     /* 26 */
    .word   DMA0_Channel0_IRQHandler             /* 27 */
    .word   DMA0_Channel1_IRQHandler             /* 28 */
    .word   DMA0_Channel2_IRQHandler             /* 29 */
    .word   DMA0_Channel3_IRQHandler             /* 30 */
    .word   DMA0_Channel4_IRQHandler             /* 31 */
    .word   DMA0_Channel5_IRQHandler             /* 32 */
    .word   DMA0_Channel6_IRQHandler             /* 33 */
    .word   ADC_IRQHandler                       /* 34 */
    .word   CAN0_TX_IRQHandler                   /* 35 */
    .word   CAN0_RX0_IRQHandler                  /* 36 */
    .word   CAN0_RX1_IRQHandler                  /* 37 */
    .word   CAN0_EWMC_IRQHandler                 /* 38 */
    .word   EXTI5_9_IRQHandler                   /* 39 */
    .word   TIMER0_BRK_TIMER8_IRQHandler         /* 40 */
    .word   TIMER0_UP_TIMER9_IRQHandler          /* 41 */
    .word   TIMER0_TRG_CMT_TIMER10_IRQHandler    /* 42 */
    .word   TIMER0_Channel_IRQHandler            /* 43 */
    .word   TIMER1_IRQHandler                    /* 44 */
    .word   TIMER2_IRQHandler                    /* 45 */
    .word   TIMER3_IRQHandler                    /* 46 */
    .word   I2C0_EV_IRQHandler                   /* 47 */
    .word   I2C0_ER_IRQHandler                   /* 48 */
    .word   I2C1_EV_IRQHandler                   /* 49 */
    .word   I2C1_ER_IRQHandler                   /* 50 */
    .word   SPI0_IRQHandler                      /* 51 */
    .word   SPI1_IRQHandler                      /* 52 */
    .word   USART0_IRQHandler                    /* 53 */
    .word   USART1_IRQHandler                    /* 54 */
    .word   USART2_IRQHandler                    /* 55 */
    .word   EXTI10_15_IRQHandler                 /* 56 */
    .word   RTC_Alarm_IRQHandler                 /* 57 */
    .word   USBFS_WKUP_IRQHandler                /* 58 */
    .word   TIMER7_BRK_TIMER11_IRQHandler        /* 59 */
    .word   TIMER7_UP_TIMER12_IRQHandler         /* 60 */
    .word   TIMER7_TRG_CMT_TIMER13_IRQHandler    /* 61 */
    .word   TIMER7_Channel_IRQHandler            /* 62 */
    .word   DMA0_Channel7_IRQHandler             /* 63 */
    .word   0                                    /* 64 */
    .word   SDIO_IRQHandler                      /* 65 */
    .word   TIMER4_IRQHandler                    /* 66 */
    .word   SPI2_IRQHandler                      /* 67 */
    .word   UART3_IRQHandler                     /* 68 */
    .word   UART4_IRQHandler                     /* 69 */
    .word   TIMER5_DAC_IRQHandler                /* 70 */
    .word   TIMER6_IRQHandler                    /* 71 */
    .word   DMA1_Channel0_IRQHandler             /* 72 */
    .word   DMA1_Channel1_IRQHandler             /* 73 */
    .word   DMA1_Channel2_IRQHandler             /* 74 */
    .word   DMA1_Channel3_IRQHandler             /* 75 */
    .word   DMA1_Channel4_IRQHandler             /* 76 */
    .word   0                                    /* 77 */
    .word   0                                    /* 78 */
    .word   CAN1_TX_IRQHandler                   /* 79 */
    .word   CAN1_RX0_IRQHandler                  /* 80 */
    .word   CAN1_RX1_IRQHandler                  /* 81 */
    .word   CAN1_EWMC_IRQHandler                 /* 82 */
    .word   USBFS_IRQHandler                     /* 83 */
    .word   DMA1_Channel5_IRQHandler             /* 84 */
    .word   DMA1_Channel6_IRQHandler             /* 85 */
    .word   DMA1_Channel7_IRQHandler             /* 86 */
    .word   USART5_IRQHandler                    /* 87 */
    .word   I2C2_EV_IRQHandler                   /* 88 */
    .word   I2C2_ER_IRQHandler                   /* 89 */
    .word   USBHS_EP1_Out_IRQHandler             /* 90 */
    .word   USBHS_EP1_In_IRQHandler              /* 91 */
    .word   USBHS_WKUP_IRQHandler                /* 92 */
    .word   USBHS_IRQHandler                     /* 93 */
    .word   DCI_IRQHandler                       /* 94 */
    .word   0                                    /* 95 */
    .word   TRNG_IRQHandler                      /* 96 */
    .word   FPU_IRQHandler                       /* 97 */

/* ---- Reset ----------------------------------------------------------------------- */
    .section .text.Reset_Handler, "ax", %progbits
    .global Reset_Handler
    .type   Reset_Handler, %function
    .thumb_func
Reset_Handler:
    cpsid   i                           /* main() enables IRQs after SysTick setup */
    ldr     r0, =0xE000ED08             /* SCB->VTOR */
    ldr     r1, =__Vectors
    str     r1, [r0]
    ldr     r0, =_estack                /* own stack, not the one left by the bootloader */
    msr     msp, r0
    movs    r0, #0
    msr     control, r0                 /* thread mode privileged, MSP, no FPCA */
    isb
    ldr     r0, =0xE000E010             /* SysTick->CTRL: stop the bootloader tick */
    movs    r1, #0
    str     r1, [r0]
    mvns    r1, r1                      /* 0xFFFFFFFF */
    ldr     r0, =0xE000E180             /* NVIC->ICER[0..2]: disable all IRQs */
    str     r1, [r0, #0]
    str     r1, [r0, #4]
    str     r1, [r0, #8]
    ldr     r0, =0xE000E280             /* NVIC->ICPR[0..2]: clear pending IRQs */
    str     r1, [r0, #0]
    str     r1, [r0, #4]
    str     r1, [r0, #8]
    ldr     r0, =0xE000ED88             /* SCB->CPACR: CP10/CP11 full access */
    ldr     r1, [r0]
    orr     r1, r1, #0x00F00000
    str     r1, [r0]
    dsb
    isb
    bl      SystemInit                  /* clocks: 120 MHz PLL from 8 MHz HXTAL */

    ldr     r0, =_sidata                /* .data: copy the initial values from flash */
    ldr     r1, =_sdata
    ldr     r2, =_edata
1:  cmp     r1, r2
    ittt    lo
    ldrlo   r3, [r0], #4
    strlo   r3, [r1], #4
    blo     1b
    ldr     r1, =_sbss                  /* .bss: zero */
    ldr     r2, =_ebss
    movs    r3, #0
2:  cmp     r1, r2
    itt     lo
    strlo   r3, [r1], #4
    blo     2b

    bl      __libc_init_array
    bl      main
3:  b       3b
    .size   Reset_Handler, . - Reset_Handler

/* ---- Default handlers (weak, override in C) ---------------------------------------- */
    .section .text.Default_Handler, "ax", %progbits
    .type   Default_Handler, %function
    .thumb_func
Default_Handler:
    b       .
    .size   Default_Handler, . - Default_Handler

    .weak   NMI_Handler
    .thumb_set NMI_Handler, Default_Handler
    .weak   HardFault_Handler
    .thumb_set HardFault_Handler, Default_Handler
    .weak   MemManage_Handler
    .thumb_set MemManage_Handler, Default_Handler
    .weak   BusFault_Handler
    .thumb_set BusFault_Handler, Default_Handler
    .weak   UsageFault_Handler
    .thumb_set UsageFault_Handler, Default_Handler
    .weak   SVC_Handler
    .thumb_set SVC_Handler, Default_Handler
    .weak   DebugMon_Handler
    .thumb_set DebugMon_Handler, Default_Handler
    .weak   PendSV_Handler
    .thumb_set PendSV_Handler, Default_Handler
    .weak   SysTick_Handler
    .thumb_set SysTick_Handler, Default_Handler
    .weak   WWDGT_IRQHandler
    .thumb_set WWDGT_IRQHandler, Default_Handler
    .weak   LVD_IRQHandler
    .thumb_set LVD_IRQHandler, Default_Handler
    .weak   TAMPER_STAMP_IRQHandler
    .thumb_set TAMPER_STAMP_IRQHandler, Default_Handler
    .weak   RTC_WKUP_IRQHandler
    .thumb_set RTC_WKUP_IRQHandler, Default_Handler
    .weak   FMC_IRQHandler
    .thumb_set FMC_IRQHandler, Default_Handler
    .weak   RCU_CTC_IRQHandler
    .thumb_set RCU_CTC_IRQHandler, Default_Handler
    .weak   EXTI0_IRQHandler
    .thumb_set EXTI0_IRQHandler, Default_Handler
    .weak   EXTI1_IRQHandler
    .thumb_set EXTI1_IRQHandler, Default_Handler
    .weak   EXTI2_IRQHandler
    .thumb_set EXTI2_IRQHandler, Default_Handler
    .weak   EXTI3_IRQHandler
    .thumb_set EXTI3_IRQHandler, Default_Handler
    .weak   EXTI4_IRQHandler
    .thumb_set EXTI4_IRQHandler, Default_Handler
    .weak   DMA0_Channel0_IRQHandler
    .thumb_set DMA0_Channel0_IRQHandler, Default_Handler
    .weak   DMA0_Channel1_IRQHandler
    .thumb_set DMA0_Channel1_IRQHandler, Default_Handler
    .weak   DMA0_Channel2_IRQHandler
    .thumb_set DMA0_Channel2_IRQHandler, Default_Handler
    .weak   DMA0_Channel3_IRQHandler
    .thumb_set DMA0_Channel3_IRQHandler, Default_Handler
    .weak   DMA0_Channel4_IRQHandler
    .thumb_set DMA0_Channel4_IRQHandler, Default_Handler
    .weak   DMA0_Channel5_IRQHandler
    .thumb_set DMA0_Channel5_IRQHandler, Default_Handler
    .weak   DMA0_Channel6_IRQHandler
    .thumb_set DMA0_Channel6_IRQHandler, Default_Handler
    .weak   ADC_IRQHandler
    .thumb_set ADC_IRQHandler, Default_Handler
    .weak   CAN0_TX_IRQHandler
    .thumb_set CAN0_TX_IRQHandler, Default_Handler
    .weak   CAN0_RX0_IRQHandler
    .thumb_set CAN0_RX0_IRQHandler, Default_Handler
    .weak   CAN0_RX1_IRQHandler
    .thumb_set CAN0_RX1_IRQHandler, Default_Handler
    .weak   CAN0_EWMC_IRQHandler
    .thumb_set CAN0_EWMC_IRQHandler, Default_Handler
    .weak   EXTI5_9_IRQHandler
    .thumb_set EXTI5_9_IRQHandler, Default_Handler
    .weak   TIMER0_BRK_TIMER8_IRQHandler
    .thumb_set TIMER0_BRK_TIMER8_IRQHandler, Default_Handler
    .weak   TIMER0_UP_TIMER9_IRQHandler
    .thumb_set TIMER0_UP_TIMER9_IRQHandler, Default_Handler
    .weak   TIMER0_TRG_CMT_TIMER10_IRQHandler
    .thumb_set TIMER0_TRG_CMT_TIMER10_IRQHandler, Default_Handler
    .weak   TIMER0_Channel_IRQHandler
    .thumb_set TIMER0_Channel_IRQHandler, Default_Handler
    .weak   TIMER1_IRQHandler
    .thumb_set TIMER1_IRQHandler, Default_Handler
    .weak   TIMER2_IRQHandler
    .thumb_set TIMER2_IRQHandler, Default_Handler
    .weak   TIMER3_IRQHandler
    .thumb_set TIMER3_IRQHandler, Default_Handler
    .weak   I2C0_EV_IRQHandler
    .thumb_set I2C0_EV_IRQHandler, Default_Handler
    .weak   I2C0_ER_IRQHandler
    .thumb_set I2C0_ER_IRQHandler, Default_Handler
    .weak   I2C1_EV_IRQHandler
    .thumb_set I2C1_EV_IRQHandler, Default_Handler
    .weak   I2C1_ER_IRQHandler
    .thumb_set I2C1_ER_IRQHandler, Default_Handler
    .weak   SPI0_IRQHandler
    .thumb_set SPI0_IRQHandler, Default_Handler
    .weak   SPI1_IRQHandler
    .thumb_set SPI1_IRQHandler, Default_Handler
    .weak   USART0_IRQHandler
    .thumb_set USART0_IRQHandler, Default_Handler
    .weak   USART1_IRQHandler
    .thumb_set USART1_IRQHandler, Default_Handler
    .weak   USART2_IRQHandler
    .thumb_set USART2_IRQHandler, Default_Handler
    .weak   EXTI10_15_IRQHandler
    .thumb_set EXTI10_15_IRQHandler, Default_Handler
    .weak   RTC_Alarm_IRQHandler
    .thumb_set RTC_Alarm_IRQHandler, Default_Handler
    .weak   USBFS_WKUP_IRQHandler
    .thumb_set USBFS_WKUP_IRQHandler, Default_Handler
    .weak   TIMER7_BRK_TIMER11_IRQHandler
    .thumb_set TIMER7_BRK_TIMER11_IRQHandler, Default_Handler
    .weak   TIMER7_UP_TIMER12_IRQHandler
    .thumb_set TIMER7_UP_TIMER12_IRQHandler, Default_Handler
    .weak   TIMER7_TRG_CMT_TIMER13_IRQHandler
    .thumb_set TIMER7_TRG_CMT_TIMER13_IRQHandler, Default_Handler
    .weak   TIMER7_Channel_IRQHandler
    .thumb_set TIMER7_Channel_IRQHandler, Default_Handler
    .weak   DMA0_Channel7_IRQHandler
    .thumb_set DMA0_Channel7_IRQHandler, Default_Handler
    .weak   SDIO_IRQHandler
    .thumb_set SDIO_IRQHandler, Default_Handler
    .weak   TIMER4_IRQHandler
    .thumb_set TIMER4_IRQHandler, Default_Handler
    .weak   SPI2_IRQHandler
    .thumb_set SPI2_IRQHandler, Default_Handler
    .weak   UART3_IRQHandler
    .thumb_set UART3_IRQHandler, Default_Handler
    .weak   UART4_IRQHandler
    .thumb_set UART4_IRQHandler, Default_Handler
    .weak   TIMER5_DAC_IRQHandler
    .thumb_set TIMER5_DAC_IRQHandler, Default_Handler
    .weak   TIMER6_IRQHandler
    .thumb_set TIMER6_IRQHandler, Default_Handler
    .weak   DMA1_Channel0_IRQHandler
    .thumb_set DMA1_Channel0_IRQHandler, Default_Handler
    .weak   DMA1_Channel1_IRQHandler
    .thumb_set DMA1_Channel1_IRQHandler, Default_Handler
    .weak   DMA1_Channel2_IRQHandler
    .thumb_set DMA1_Channel2_IRQHandler, Default_Handler
    .weak   DMA1_Channel3_IRQHandler
    .thumb_set DMA1_Channel3_IRQHandler, Default_Handler
    .weak   DMA1_Channel4_IRQHandler
    .thumb_set DMA1_Channel4_IRQHandler, Default_Handler
    .weak   CAN1_TX_IRQHandler
    .thumb_set CAN1_TX_IRQHandler, Default_Handler
    .weak   CAN1_RX0_IRQHandler
    .thumb_set CAN1_RX0_IRQHandler, Default_Handler
    .weak   CAN1_RX1_IRQHandler
    .thumb_set CAN1_RX1_IRQHandler, Default_Handler
    .weak   CAN1_EWMC_IRQHandler
    .thumb_set CAN1_EWMC_IRQHandler, Default_Handler
    .weak   USBFS_IRQHandler
    .thumb_set USBFS_IRQHandler, Default_Handler
    .weak   DMA1_Channel5_IRQHandler
    .thumb_set DMA1_Channel5_IRQHandler, Default_Handler
    .weak   DMA1_Channel6_IRQHandler
    .thumb_set DMA1_Channel6_IRQHandler, Default_Handler
    .weak   DMA1_Channel7_IRQHandler
    .thumb_set DMA1_Channel7_IRQHandler, Default_Handler
    .weak   USART5_IRQHandler
    .thumb_set USART5_IRQHandler, Default_Handler
    .weak   I2C2_EV_IRQHandler
    .thumb_set I2C2_EV_IRQHandler, Default_Handler
    .weak   I2C2_ER_IRQHandler
    .thumb_set I2C2_ER_IRQHandler, Default_Handler
    .weak   USBHS_EP1_Out_IRQHandler
    .thumb_set USBHS_EP1_Out_IRQHandler, Default_Handler
    .weak   USBHS_EP1_In_IRQHandler
    .thumb_set USBHS_EP1_In_IRQHandler, Default_Handler
    .weak   USBHS_WKUP_IRQHandler
    .thumb_set USBHS_WKUP_IRQHandler, Default_Handler
    .weak   USBHS_IRQHandler
    .thumb_set USBHS_IRQHandler, Default_Handler
    .weak   DCI_IRQHandler
    .thumb_set DCI_IRQHandler, Default_Handler
    .weak   TRNG_IRQHandler
    .thumb_set TRNG_IRQHandler, Default_Handler
    .weak   FPU_IRQHandler
    .thumb_set FPU_IRQHandler, Default_Handler

    .end
