/*
 * bsp_spi1.c - SPI1 master, see bsp_spi1.h
 */
#include "bsp_spi1.h"
#include "gd32f4xx.h"
#include "gd32f4xx_dma.h"   /* not in gd32f4xx_libopt.h */

#define SPI1_DMA                DMA0
#define SPI1_TX_DMA_CH          DMA_CH4     /* sub-peripheral 0 */
#define SPI1_RX_DMA_CH          DMA_CH3     /* sub-peripheral 0 */
#define BSP_IRQ_PRIO_SPI1_DMA   7U          /* below USBFS (6), group PRE4_SUB0 */
#define DMA_ALL_FLAGS           (DMA_FLAG_FTF | DMA_FLAG_HTF | DMA_FLAG_TAE | DMA_FLAG_SDE | DMA_FLAG_FEE)

static void (*volatile s_dma_done)(void);
static void (*volatile s_idle_hook)(void);
static volatile uint8_t s_dma_busy;
static volatile uint8_t s_dma_rx;           /* the running transfer receives (ends on RX) */
static const uint8_t s_dummy = 0xFFU;       /* clocked out while receiving */

static void dma_channel_init(dma_channel_enum ch, uint32_t direction)
{
    dma_single_data_parameter_struct dma_init_struct;

    dma_deinit(SPI1_DMA, ch);
    dma_single_data_para_struct_init(&dma_init_struct);
    dma_init_struct.periph_addr         = (uint32_t)&SPI_DATA(SPI1);
    dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
    dma_init_struct.direction           = direction;
    dma_init_struct.priority            = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(SPI1_DMA, ch, &dma_init_struct);
    dma_channel_subperipheral_select(SPI1_DMA, ch, DMA_SUBPERI0);
    dma_interrupt_enable(SPI1_DMA, ch, DMA_CHXCTL_FTFIE);
}

static void dma_init(void)
{
    rcu_periph_clock_enable(RCU_DMA0);
    dma_channel_init(SPI1_TX_DMA_CH, DMA_MEMORY_TO_PERIPH);
    dma_channel_init(SPI1_RX_DMA_CH, DMA_PERIPH_TO_MEMORY);
    nvic_irq_enable((uint8_t)DMA0_Channel4_IRQn, BSP_IRQ_PRIO_SPI1_DMA, 0U);
    nvic_irq_enable((uint8_t)DMA0_Channel3_IRQn, BSP_IRQ_PRIO_SPI1_DMA, 0U);
}

void BspSpi1_Init(void)
{
    spi_parameter_struct spi_init_struct;

    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_4;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI1, &spi_init_struct);

    spi_enable(SPI1);
    dma_init();
}

void BspSpi1_Write(const uint8_t *data, uint32_t len)
{
    while (len > 0U)
    {
        while (RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE))
        {
        }
        spi_i2s_data_transmit(SPI1, *data);
        data++;
        len--;
    }
}

/* Drop what polled writes left in the receiver (RBNE, overrun): read DATA, then STAT */
static void rx_flush(void)
{
    BspSpi1_WaitIdle();
    (void)SPI_DATA(SPI1);
    (void)SPI_STAT(SPI1);
}

uint8_t BspSpi1_Transfer(uint8_t byte)
{
    rx_flush();
    spi_i2s_data_transmit(SPI1, byte);
    while (RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_RBNE))
    {
    }
    return (uint8_t)spi_i2s_data_receive(SPI1);
}

static void tx_dma_start(const uint8_t *data, uint32_t len, uint32_t mem_inc)
{
    dma_channel_disable(SPI1_DMA, SPI1_TX_DMA_CH);
    dma_flag_clear(SPI1_DMA, SPI1_TX_DMA_CH, DMA_ALL_FLAGS);
    dma_memory_address_generation_config(SPI1_DMA, SPI1_TX_DMA_CH, (uint8_t)mem_inc);
    dma_memory_address_config(SPI1_DMA, SPI1_TX_DMA_CH, DMA_MEMORY_0, (uint32_t)data);
    dma_transfer_number_config(SPI1_DMA, SPI1_TX_DMA_CH, len);
    dma_channel_enable(SPI1_DMA, SPI1_TX_DMA_CH);
}

void BspSpi1_WriteDma(const uint8_t *data, uint32_t len, void (*done)(void))
{
    s_dma_done = done;
    s_dma_rx = 0U;
    s_dma_busy = 1U;

    tx_dma_start(data, len, DMA_MEMORY_INCREASE_ENABLE);
    spi_dma_enable(SPI1, SPI_DMA_TRANSMIT);
}

void BspSpi1_ReadDma(uint8_t *data, uint32_t len, void (*done)(void))
{
    s_dma_done = done;
    s_dma_rx = 1U;
    s_dma_busy = 1U;

    rx_flush();
    dma_channel_disable(SPI1_DMA, SPI1_RX_DMA_CH);
    dma_flag_clear(SPI1_DMA, SPI1_RX_DMA_CH, DMA_ALL_FLAGS);
    dma_memory_address_config(SPI1_DMA, SPI1_RX_DMA_CH, DMA_MEMORY_0, (uint32_t)data);
    dma_transfer_number_config(SPI1_DMA, SPI1_RX_DMA_CH, len);
    dma_channel_enable(SPI1_DMA, SPI1_RX_DMA_CH);

    tx_dma_start(&s_dummy, len, DMA_MEMORY_INCREASE_DISABLE);
    spi_dma_enable(SPI1, SPI_DMA_RECEIVE);
    spi_dma_enable(SPI1, SPI_DMA_TRANSMIT);
}

uint8_t BspSpi1_DmaBusy(void)
{
    return s_dma_busy;
}

void BspSpi1_SetIdleHook(void (*hook)(void))
{
    s_idle_hook = hook;
}

void BspSpi1_WaitIdle(void)
{
    while (RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE))
    {
    }
    while (RESET != spi_i2s_flag_get(SPI1, SPI_FLAG_TRANS))
    {
    }
}

/* End of a transfer: owner's done(), then the bus is offered to the idle hook */
static void transfer_end(void)
{
    spi_dma_disable(SPI1, SPI_DMA_TRANSMIT);
    spi_dma_disable(SPI1, SPI_DMA_RECEIVE);
    BspSpi1_WaitIdle();
    s_dma_busy = 0U;
    if (s_dma_done != 0)
    {
        s_dma_done();
    }
    if ((s_dma_busy == 0U) && (s_idle_hook != 0))
    {
        s_idle_hook();
    }
}

/* TX finished = last byte handed to SPI. A receiving transfer ends on RX instead. */
void DMA0_Channel4_IRQHandler(void)
{
    if (dma_interrupt_flag_get(SPI1_DMA, SPI1_TX_DMA_CH, DMA_INT_FLAG_FTF) != RESET)
    {
        dma_interrupt_flag_clear(SPI1_DMA, SPI1_TX_DMA_CH, DMA_INT_FLAG_FTF);
        if (s_dma_rx == 0U)
        {
            transfer_end();
        }
    }
}

/* RX finished = last byte clocked in */
void DMA0_Channel3_IRQHandler(void)
{
    if (dma_interrupt_flag_get(SPI1_DMA, SPI1_RX_DMA_CH, DMA_INT_FLAG_FTF) != RESET)
    {
        dma_interrupt_flag_clear(SPI1_DMA, SPI1_RX_DMA_CH, DMA_INT_FLAG_FTF);
        transfer_end();
    }
}
