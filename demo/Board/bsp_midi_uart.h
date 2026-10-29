/*
 * bsp_midi_uart.h - DIN MIDI ports, 31250 baud 8N1
 *   port A: USART1 TX (PA2, MIDI OUT A) + RX (PA3, MIDI IN), TX by DMA0 CH6
 *   port B: USART5 TX (PC6, MIDI OUT B), TX by DMA1 CH6
 * Received bytes go to a ring buffer (USART1 RX interrupt).
 */
#ifndef BSP_MIDI_UART_H
#define BSP_MIDI_UART_H

#include <stdint.h>

typedef enum
{
    BSP_MIDI_PORT_A = 0,    /* USART1 */
    BSP_MIDI_PORT_B = 1,    /* USART5, output only */
    BSP_MIDI_NUM_PORTS
} BspMidiPort_t;

#define BSP_MIDI_TX_BUFFER_SIZE     512U    /* bytes per send */
#define BSP_MIDI_RX_BUFFER_SIZE     256U    /* power of two */

void BspMidiUart_Init(void);

/* Copies the bytes and starts the DMA; returns 0, or -1 if the port is still sending
 * (or len is 0 / too long). Not thread safe per port. */
int BspMidiUart_Send(BspMidiPort_t port, const uint8_t *data, uint32_t len);
int BspMidiUart_IsTxBusy(BspMidiPort_t port);

/* Bytes received on MIDI IN (port A); single consumer thread. Returns the count copied. */
uint32_t BspMidiUart_Read(uint8_t *buf, uint32_t max);

/* Bytes lost because the ring buffer was full, or by a USART overrun */
uint32_t BspMidiUart_GetRxDropped(void);

#endif /* BSP_MIDI_UART_H */
