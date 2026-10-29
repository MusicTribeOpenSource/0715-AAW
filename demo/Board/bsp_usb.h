/*
 * bsp_usb.h - USBFS device bring-up for the GD32 USB stack (UsbDriver/)
 * Provides the drv_usb_hw.h hooks the stack needs (48 MHz clock, IRQ, delays)
 * without TIMER3: delays busy wait on SysTick (bsp_delay), so they also work
 * inside the USB ISR.
 */
#ifndef BSP_USB_H
#define BSP_USB_H

/* USB 48 MHz clock, stack init (USB MIDI class, connect), USBFS IRQ on.
 * SysTick must already run (BspDelay). */
void BspUsb_Init(void);

#endif /* BSP_USB_H */
