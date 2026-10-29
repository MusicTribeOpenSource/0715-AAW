/*
 * midi_thru.h - DIN MIDI test: everything received on MIDI IN goes out unchanged
 * on MIDI OUT A and MIDI OUT B (raw bytes, no parsing, no filtering)
 */
#ifndef MIDI_THRU_H
#define MIDI_THRU_H

#include <stdint.h>

/* USART1/USART5 at 31250 baud, TX DMA, RX interrupt. After BspClock_Init/BspGpio_Init. */
void MidiThru_Init(void);

/* Call from the main loop: when both outputs are idle, sends what came in meanwhile;
 * returns the number of bytes forwarded now */
uint32_t MidiThru_Poll(void);

/* Bytes forwarded since power up; bytes lost on MIDI IN (ring buffer full / overrun) */
uint32_t MidiThru_GetForwarded(void);
uint32_t MidiThru_GetDropped(void);

#endif /* MIDI_THRU_H */
