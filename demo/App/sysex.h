/*
 * sysex.h - SysEx parser for the USB MIDI ports (docs/BCR32_Firmware_Update_Protocol.md)
 *
 * Reassembles SysEx messages from the USB-MIDI event packets of each cable and handles:
 *   0x74 mode query       -> reply F0 00 20 32 00 64 74 00 F7 ("application") on the same cable
 *   0x77 enter bootloader -> Sysex_BootRequested() becomes 1; the main loop starts the bootloader
 * Header F0 00 20 32, device 7F or 00, model 64 or 7F, exactly 8 bytes; anything else is ignored.
 * Main loop only.
 */
#ifndef SYSEX_H
#define SYSEX_H

#include <stdint.h>

/* Feed USB-MIDI event packets (4 bytes each) received from the host */
void Sysex_UsbRx(const uint8_t *pkt, uint32_t len);

/* Copy pending reply packets (USB-MIDI, 4 bytes each) to pkt, at most max bytes;
 * returns the number of bytes copied (0 = nothing to send) */
uint32_t Sysex_UsbTx(uint8_t *pkt, uint32_t max);

/* 0x77 was received: start the bootloader */
uint8_t Sysex_BootRequested(void);

#endif /* SYSEX_H */
