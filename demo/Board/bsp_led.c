/*
 * bsp_led.c - HT16K33 LED drivers on I2C0, see bsp_led.h
 */
#include "bsp_led.h"
#include "bsp_delay.h"
#include "gd32f4xx.h"

#define LED_I2C             I2C0
#define LED_I2C_OWN_ADDR    0xFFU       /* own address, unused in master mode */
#define LED_I2C_TIMEOUT     20000U      /* polling loops per flag */
#define LED_WRITE_GAP_US    50U         /* 50 us between two writes */

/* HT16K33 commands */
#define HT16K33_SYS_SETUP_ADDRESS       0x20U
#define HT16K33_INT_SETUP_ADDRESS       0xA0U
#define HT16K33_DISPLAY_SET_ADDRESS     0x80U
#define HT16K33_DISPLAY_DUTY_ADDRESS    0xE0U
#define HT16K33_SYS_SETUP_OSC_ON        0x21U
#define HT16K33_INT_SET_ACTIVELOW       0xA1U
#define HT16K33_DISPLAY_ON_BLINK_OFF    0x81U

const uint8_t g_bsp_led_chips[BSP_LED_NUM_CHIPS] =
{
    BSP_LED_CHIP_BTN, BSP_LED_CHIP_ENC_ROW1, BSP_LED_CHIP_ENC_ROW2, BSP_LED_CHIP_ENC_ROW3, BSP_LED_CHIP_ENC_ROW4
};

void BspLedI2c_Init(void)
{
    i2c_clock_config(LED_I2C, 200000U, I2C_DTCY_2);
    i2c_mode_addr_config(LED_I2C, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, LED_I2C_OWN_ADDR);
    i2c_enable(LED_I2C);
    i2c_ack_config(LED_I2C, I2C_ACK_ENABLE);
}

static int wait_flag(i2c_flag_enum flag, FlagStatus state)
{
    uint32_t n = LED_I2C_TIMEOUT;
    while (i2c_flag_get(LED_I2C, flag) != state)
    {
        if (--n == 0U)
        {
            return -1;
        }
    }
    return 0;
}

/* On a timeout release the bus, so that one missing chip does not block the transfers
 * to the others */
static int abort_transfer(void)
{
    i2c_stop_on_bus(LED_I2C);
    i2c_flag_clear(LED_I2C, I2C_FLAG_AERR);
    (void)wait_flag(I2C_FLAG_I2CBSY, RESET);
    return -1;
}

int BspLedI2c_Write(uint8_t chip, const uint8_t *data, uint32_t len)
{
    uint32_t i, n;

    if (wait_flag(I2C_FLAG_I2CBSY, RESET) != 0)
    {
        return abort_transfer();
    }
    i2c_start_on_bus(LED_I2C);
    if (wait_flag(I2C_FLAG_SBSEND, SET) != 0)
    {
        return abort_transfer();
    }
    i2c_master_addressing(LED_I2C, chip, I2C_TRANSMITTER);
    if (wait_flag(I2C_FLAG_ADDSEND, SET) != 0)
    {
        return abort_transfer();
    }
    i2c_flag_clear(LED_I2C, I2C_FLAG_ADDSEND);

    for (i = 0; i < len; i++)
    {
        i2c_data_transmit(LED_I2C, data[i]);
        if (wait_flag(I2C_FLAG_TBE, SET) != 0)
        {
            return abort_transfer();
        }
    }
    i2c_stop_on_bus(LED_I2C);

    /* wait until the STOP is generated */
    n = LED_I2C_TIMEOUT;
    while ((I2C_CTL0(LED_I2C) & I2C_CTL0_STOP) != 0U)
    {
        if (--n == 0U)
        {
            return -1;
        }
    }
    return 0;
}

int BspLed_Write2(uint8_t chip, uint8_t first, uint8_t second)
{
    const uint8_t frame[2] = {first, second};
    return BspLedI2c_Write(chip, frame, sizeof(frame));
}

/* Every command goes out as the frame [value, address]; the HT16K33 takes the first byte
 * (frames as proven on the board) */
uint32_t BspLed_Init(uint8_t brightness)
{
    uint32_t c, failed = 0;

    for (c = 0; c < BSP_LED_NUM_CHIPS; c++)
    {
        const uint8_t chip = g_bsp_led_chips[c];
        int err = 0;

        err |= BspLed_Write2(chip, HT16K33_SYS_SETUP_OSC_ON, HT16K33_SYS_SETUP_ADDRESS);
        err |= BspLed_Write2(chip, HT16K33_INT_SET_ACTIVELOW, HT16K33_INT_SETUP_ADDRESS);
        err |= BspLed_Write2(chip, HT16K33_DISPLAY_ON_BLINK_OFF, HT16K33_DISPLAY_SET_ADDRESS);
        err |= BspLed_Write2(chip, (uint8_t)(0xE0U | (brightness & 0x0FU)), HT16K33_DISPLAY_DUTY_ADDRESS);
        if (err != 0)
        {
            failed++;
        }
    }
    (void)BspLed_All(0U);
    return failed;
}

/* all off / all on: frame [ram address, data] per display RAM byte */
uint32_t BspLed_All(uint8_t on)
{
    uint32_t c, a, failed = 0;
    const uint8_t value = (on != 0U) ? 0xFFU : 0x00U;

    for (c = 0; c < BSP_LED_NUM_CHIPS; c++)
    {
        for (a = 0; a < BSP_LED_RAM_BYTES; a++)
        {
            if (BspLed_Write2(g_bsp_led_chips[c], (uint8_t)a, value) != 0)
            {
                failed++;
            }
            BspDelay_Us(LED_WRITE_GAP_US);
        }
        BspDelay_Us(LED_WRITE_GAP_US);
    }
    return failed;
}
