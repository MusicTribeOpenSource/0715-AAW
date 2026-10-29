/*
 * midi_thru.c - DIN MIDI IN -> OUT A + OUT B, see midi_thru.h
 */
#include "midi_thru.h"
#include "bsp_midi_uart.h"

#define CHUNK_MAX   64U     /* bytes per forward; ~20 ms of MIDI at full rate */

static uint32_t s_forwarded;

void MidiThru_Init(void)
{
    BspMidiUart_Init();
}

/* Both ports get the same bytes: read only when both are idle, the data waits in the
 * RX ring buffer (256 bytes) meanwhile; IN and OUT run at the same baud rate */
uint32_t MidiThru_Poll(void)
{
    uint8_t buf[CHUNK_MAX];
    uint32_t n;

    if ((BspMidiUart_IsTxBusy(BSP_MIDI_PORT_A) != 0) || (BspMidiUart_IsTxBusy(BSP_MIDI_PORT_B) != 0))
    {
        return 0U;
    }
    n = BspMidiUart_Read(buf, sizeof(buf));
    if (n == 0U)
    {
        return 0U;
    }
    (void)BspMidiUart_Send(BSP_MIDI_PORT_A, buf, n);
    (void)BspMidiUart_Send(BSP_MIDI_PORT_B, buf, n);
    s_forwarded += n;
    return n;
}

uint32_t MidiThru_GetForwarded(void)
{
    return s_forwarded;
}

uint32_t MidiThru_GetDropped(void)
{
    return BspMidiUart_GetRxDropped();
}
