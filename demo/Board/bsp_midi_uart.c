/*
 * bsp_midi_uart.c - DIN MIDI ports, see bsp_midi_uart.h
 */
#include "bsp_midi_uart.h"
#include "bsp_irq.h"
#include "gd32f4xx.h"
#include "gd32f4xx_dma.h"     /* not in gd32f4xx_libopt.h */

#define MIDI_BAUD   31250U

/* all interrupt flags of a DMA channel, cleared before it is started again */
#define DMA_ALL_FLAGS   (DMA_FLAG_FTF | DMA_FLAG_HTF | DMA_FLAG_TAE | DMA_FLAG_SDE | DMA_FLAG_FEE)

typedef struct
{
    uint32_t         usart;
    uint32_t         dma;
    dma_channel_enum ch;
    uint8_t         *buf;
} MidiTxPort_t;

/* DMA buffers in main SRAM (RW_IRAM1), the DMA cannot reach the TCMSRAM */
static uint8_t s_tx_buf_a[BSP_MIDI_TX_BUFFER_SIZE];
static uint8_t s_tx_buf_b[BSP_MIDI_TX_BUFFER_SIZE];

static const MidiTxPort_t s_tx[BSP_MIDI_NUM_PORTS] =
{
    { USART1, DMA0, DMA_CH6, s_tx_buf_a },
    { USART5, DMA1, DMA_CH6, s_tx_buf_b },
};

static uint8_t           s_rx_buf[BSP_MIDI_RX_BUFFER_SIZE];
static volatile uint32_t s_rx_head;     /* written by the ISR */
static volatile uint32_t s_rx_tail;     /* written by the consumer */
static volatile uint32_t s_rx_dropped;

/* memory -> USART data register, 8 bit, the channel stops at the end */
static void tx_dma_init(uint32_t dma, dma_channel_enum ch, dma_subperipheral_enum sub, uint32_t usart)
{
    dma_single_data_parameter_struct dma_init_struct;

    dma_deinit(dma, ch);
    dma_single_data_para_struct_init(&dma_init_struct);
    dma_init_struct.periph_addr         = (uint32_t)&USART_DATA(usart);
    dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
    dma_init_struct.direction           = DMA_MEMORY_TO_PERIPH;
    dma_init_struct.priority            = DMA_PRIORITY_MEDIUM;
    dma_single_data_mode_init(dma, ch, &dma_init_struct);
    dma_channel_subperipheral_select(dma, ch, sub);
    usart_dma_transmit_config(usart, USART_TRANSMIT_DMA_ENABLE);
}

void BspMidiUart_Init(void)
{
    /* port B: USART5 TX only */
    usart_deinit(USART5);
    usart_baudrate_set(USART5, MIDI_BAUD);
    usart_transmit_config(USART5, USART_TRANSMIT_ENABLE);
    usart_enable(USART5);
    tx_dma_init(DMA1, DMA_CH6, DMA_SUBPERI5, USART5);

    /* port A: USART1 RX + TX */
    usart_deinit(USART1);
    usart_baudrate_set(USART1, MIDI_BAUD);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    usart_enable(USART1);
    nvic_irq_enable(USART1_IRQn, BSP_IRQ_PRIO_USART1_MIDI_RX, 0U);
    usart_interrupt_enable(USART1, USART_INT_RBNE);
    rcu_periph_clock_enable(RCU_DMA0);
    tx_dma_init(DMA0, DMA_CH6, DMA_SUBPERI4, USART1);
}

int BspMidiUart_IsTxBusy(BspMidiPort_t port)
{
    const MidiTxPort_t *p = &s_tx[port];
    return ((DMA_CHCTL(p->dma, p->ch) & DMA_CHXCTL_CHEN) != 0U) ? 1 : 0;
}

/* copy, then start the TX DMA */
int BspMidiUart_Send(BspMidiPort_t port, const uint8_t *data, uint32_t len)
{
    const MidiTxPort_t *p;
    uint32_t i;

    if ((port >= BSP_MIDI_NUM_PORTS) || (len == 0U) || (len > BSP_MIDI_TX_BUFFER_SIZE) ||
        (BspMidiUart_IsTxBusy(port) != 0))
    {
        return -1;
    }
    p = &s_tx[port];
    for (i = 0; i < len; i++)
    {
        p->buf[i] = data[i];
    }
    dma_flag_clear(p->dma, p->ch, DMA_ALL_FLAGS);
    dma_memory_address_config(p->dma, p->ch, DMA_MEMORY_0, (uint32_t)p->buf);
    dma_transfer_number_config(p->dma, p->ch, len);
    dma_channel_enable(p->dma, p->ch);
    return 0;
}

uint32_t BspMidiUart_Read(uint8_t *buf, uint32_t max)
{
    uint32_t n = 0;
    uint32_t tail = s_rx_tail;
    const uint32_t head = s_rx_head;

    while ((tail != head) && (n < max))
    {
        buf[n++] = s_rx_buf[tail & (BSP_MIDI_RX_BUFFER_SIZE - 1U)];
        tail++;
    }
    s_rx_tail = tail;
    return n;
}

uint32_t BspMidiUart_GetRxDropped(void)
{
    return s_rx_dropped;
}

/* MIDI IN: one byte per interrupt; reading DATA after STAT0 clears RBNE and ORERR */
void USART1_IRQHandler(void)
{
    const uint32_t stat = USART_STAT0(USART1);

    if ((stat & (USART_STAT0_RBNE | USART_STAT0_ORERR)) != 0U)
    {
        const uint8_t byte = (uint8_t)usart_data_receive(USART1);
        const uint32_t head = s_rx_head;

        if ((stat & USART_STAT0_ORERR) != 0U)
        {
            s_rx_dropped++;
        }
        if ((head - s_rx_tail) < BSP_MIDI_RX_BUFFER_SIZE)
        {
            s_rx_buf[head & (BSP_MIDI_RX_BUFFER_SIZE - 1U)] = byte;
            s_rx_head = head + 1U;
        }
        else
        {
            s_rx_dropped++;
        }
    }
}
