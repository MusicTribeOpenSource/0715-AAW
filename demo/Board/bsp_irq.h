/*
 * bsp_irq.h - interrupt priority plan of the board (single place)
 *
 * Grouping NVIC_PRIGROUP_PRE4_SUB0 (set once in main): 16 preemption levels, no sub-priority;
 * lower = more urgent. SysTick runs at the lowest level (15, SysTick_Config).
 */
#ifndef BSP_IRQ_H
#define BSP_IRQ_H

#define BSP_IRQ_PRIO_SYNC_IN_CAPTURE    1U   /* TIMER0 CH1: external clock period, jitter sensitive */
#define BSP_IRQ_PRIO_BPM_CLOCK          2U   /* TIMER1: internal sequencer clock */
#define BSP_IRQ_PRIO_SYNC_OUT           2U   /* TIMER4: SYNC OUT pulse timing */
#define BSP_IRQ_PRIO_SYNC_IN_GATE       2U   /* EXTI10: SYNC IN ring edges */
#define BSP_IRQ_PRIO_USART1_MIDI_RX     3U   /* 31250 baud, no FIFO: next byte after 320 us */
#define BSP_IRQ_PRIO_USBFS              6U   /* USBX device (ux_dcd_stm32 on the HAL PCD) */
#define BSP_IRQ_PRIO_GUI_BLINK          7U   /* TIMER2: display blink, ~5 Hz */

#endif /* BSP_IRQ_H */
