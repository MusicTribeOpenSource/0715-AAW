/*
 * bsp_usb.c - USBFS hardware hooks of the GD32 USB stack, see bsp_usb.h
 * Delays by SysTick busy wait (no timer). DM/DP pins are set up by BspGpio_Init
 * (no usb_gpio_config).
 */
#include "bsp_usb.h"
#include "bsp_delay.h"
#include "drv_usb_hw.h"
#include "drv_usbd_int.h"
#include "usbd_desc.h"

#define BSP_IRQ_PRIO_USBFS  6U      /* bsp_irq.h plan, group PRE4_SUB0 */

extern usb_core_driver hUsbDeviceFS;   /* usbd_desc.c */

/* CK48M = PLLQ: 8 MHz / 8 * 240 / 5 = 48 MHz (SystemInit, 120 MHz setting) */
void usb_rcu_config(void)
{
    rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLQ);
    rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);
    rcu_periph_clock_enable(RCU_USBFS);
}

void usb_intr_config(void)
{
    nvic_irq_enable((uint8_t)USBFS_IRQn, BSP_IRQ_PRIO_USBFS, 0U);
}

void usb_udelay(const uint32_t usec)
{
    BspDelay_Us(usec);
}

void usb_mdelay(const uint32_t msec)
{
    uint32_t i;

    for (i = 0U; i < msec; i++)
    {
        BspDelay_Us(1000U);
    }
}

void USBFS_IRQHandler(void)
{
    usbd_isr(&hUsbDeviceFS);
}

void BspUsb_Init(void)
{
    usb_rcu_config();
    DrvUsbInit();       /* usbd_init with the MIDI class, remote wakeup pulse */
    usb_intr_config();
}
