/*
 * bsp_cv.c - CV / gate outputs, see bsp_cv.h
 */
#include "bsp_cv.h"
#include "bsp_gpio.h"
#include "bsp_delay.h"

#define CV_DAC_GAIN_REG     0x04U   /* DAC register addresses */
#define CV_DAC_DATA_REG     0x08U
#define CV_STEP_US          50U     /* 50 us between the steps of init / reset */

/* SPI2 master, mode 0, PCLK1 / 64 = 469 kHz */
static void cv_spi_init(void)
{
    spi_parameter_struct spi_init_struct;

    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_64;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI2, &spi_init_struct);
    spi_enable(SPI2);
}

static void cv_spi_write(uint8_t byte)
{
    while (RESET == spi_i2s_flag_get(SPI2, SPI_FLAG_TBE))
    {
    }
    spi_i2s_data_transmit(SPI2, byte);
}

/* register address, then the data bytes */
static void cv_write_reg(uint8_t reg, uint8_t hi, uint8_t lo)
{
    cv_spi_write(reg);
    cv_spi_write(hi);
    cv_spi_write(lo);
}

/* mux channel: S0 = bit 0, S1 = bit 1 */
static void cv_select(uint32_t channel)
{
    gpio_bit_write(BSP_PIN_CV_SW_S0, ((channel & 1U) != 0U) ? SET : RESET);
    gpio_bit_write(BSP_PIN_CV_SW_S1, ((channel & 2U) != 0U) ? SET : RESET);
}

void BspCv_Init(void)
{
    uint32_t ch;

    cv_spi_init();

    /* 1. enable the mux (active low), 2. settle */
    gpio_bit_reset(BSP_PIN_CV_SW_EN);
    BspDelay_Us(CV_STEP_US);

    /* 3. gain (0x01 0x01) */
    gpio_bit_reset(BSP_PIN_CV_DAC_CS);
    BspDelay_Us(CV_STEP_US);
    cv_write_reg(CV_DAC_GAIN_REG, 0x01U, 0x01U);
    BspDelay_Us(CV_STEP_US);
    gpio_bit_set(BSP_PIN_CV_DAC_CS);

    /* 4. all channels 0, mux disabled at the end */
    for (ch = 0; ch < BSP_CV_NUM_CHANNELS; ch++)
    {
        cv_select(ch);
        BspDelay_Us(CV_STEP_US);
        gpio_bit_reset(BSP_PIN_CV_DAC_CS);
        BspDelay_Us(CV_STEP_US);
        cv_write_reg(CV_DAC_DATA_REG, 0x00U, 0x00U);
        BspDelay_Us(CV_STEP_US);
        gpio_bit_set(BSP_PIN_CV_DAC_CS);
        BspDelay_Us(CV_STEP_US);
    }
    gpio_bit_set(BSP_PIN_CV_SW_EN);

    /* 5. channel 0 selected */
    cv_select(0U);

    /* gates: 0 V */
    for (ch = 0; ch < BSP_CV_NUM_CHANNELS; ch++)
    {
        BspGate_Set(ch, BSP_GATE_0V);
    }
}

void BspCv_Write(uint32_t channel, uint16_t value)
{
    if (channel >= BSP_CV_NUM_CHANNELS)
    {
        return;
    }
    gpio_bit_set(BSP_PIN_CV_SW_EN);                 /* mux off */
    cv_select(channel);
    gpio_bit_reset(BSP_PIN_CV_DAC_CS);
    BspDelay_Us(CV_STEP_US);
    cv_write_reg(CV_DAC_DATA_REG, (uint8_t)(value >> 8), (uint8_t)value);
    BspDelay_Us(CV_STEP_US);
    gpio_bit_set(BSP_PIN_CV_DAC_CS);
    BspDelay_Us(CV_STEP_US);
    gpio_bit_reset(BSP_PIN_CV_SW_EN);               /* mux on: the channel takes the value */
}

/* one 0 V, 5 V, 10 V pin per gate; 10 V = 5V + 10V pins */
void BspGate_Set(uint32_t gate, BspGateLevel_t level)
{
    static const struct
    {
        uint32_t port0, pin0, port5, pin5, port10, pin10;
    } g[BSP_CV_NUM_CHANNELS] =
    {
        { BSP_PIN_GATE1_0V, BSP_PIN_GATE1_5V, BSP_PIN_GATE1_10V },
        { BSP_PIN_GATE2_0V, BSP_PIN_GATE2_5V, BSP_PIN_GATE2_10V },
        { BSP_PIN_GATE3_0V, BSP_PIN_GATE3_5V, BSP_PIN_GATE3_10V },
        { BSP_PIN_GATE4_0V, BSP_PIN_GATE4_5V, BSP_PIN_GATE4_10V },
    };

    if (gate >= BSP_CV_NUM_CHANNELS)
    {
        return;
    }
    switch (level)
    {
    case BSP_GATE_5V:
        gpio_bit_reset(g[gate].port0, g[gate].pin0);
        gpio_bit_reset(g[gate].port10, g[gate].pin10);
        gpio_bit_set(g[gate].port5, g[gate].pin5);
        break;
    case BSP_GATE_10V:
        gpio_bit_reset(g[gate].port0, g[gate].pin0);
        gpio_bit_set(g[gate].port10, g[gate].pin10);
        gpio_bit_set(g[gate].port5, g[gate].pin5);
        break;
    case BSP_GATE_0V:
    default:
        gpio_bit_reset(g[gate].port10, g[gate].pin10);
        gpio_bit_reset(g[gate].port5, g[gate].pin5);
        gpio_bit_set(g[gate].port0, g[gate].pin0);
        break;
    }
}
