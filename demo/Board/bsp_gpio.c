/*
 * bsp_gpio.c - BCR32 peripheral clocks and GPIO init
 * Clock enables, pin modes and default levels of the whole board.
 */
#include "bsp_gpio.h"

#define OTYPE_NA    0xFFFFFFFFUL    /* output options not written */
#define AF_NA       0xFFFFFFFFUL    /* alternate function not written */

/* one pin: mode, pull, output options, alternate function */
static void pin_config(uint32_t port, uint32_t pin, uint32_t mode, uint32_t pud,
                       uint32_t otype, uint32_t speed, uint32_t af)
{
    gpio_mode_set(port, mode, pud, pin);
    if (otype != OTYPE_NA)
    {
        gpio_output_options_set(port, otype, speed, pin);
    }
    if (af != AF_NA)
    {
        gpio_af_set(port, af, pin);
    }
}

#define OUT_PP_MAX(p)   pin_config(p, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, AF_NA)

void BspClock_Init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_TIMER3);
    rcu_periph_clock_enable(RCU_I2C1);
    rcu_periph_clock_enable(RCU_I2C0);
    rcu_periph_clock_enable(RCU_SPI0);
    rcu_periph_clock_enable(RCU_SPI1);
    rcu_periph_clock_enable(RCU_SPI2);
    rcu_periph_clock_enable(RCU_USART1);
    rcu_periph_clock_enable(RCU_USART5);
    rcu_periph_clock_enable(RCU_ADC0);
    rcu_periph_clock_enable(RCU_DMA1);
    rcu_periph_clock_enable(RCU_USBFS);
    rcu_periph_clock_enable(RCU_PMU);
    rcu_periph_clock_enable(RCU_TIMER4);
}

void BspGpio_Init(void)
{
    /* USB */
    pin_config(BSP_PIN_USB_DM, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_AF_11);
    pin_config(BSP_PIN_USB_DP, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_AF_12);

    /* USART1 (MIDI A), USART5 TX (MIDI OUT B) */
    pin_config(BSP_PIN_USART1_MIDI_OUT, GPIO_MODE_AF, GPIO_PUPD_NONE, OTYPE_NA, GPIO_OSPEED_MAX, GPIO_AF_7);
    pin_config(BSP_PIN_USART1_MIDI_IN,  GPIO_MODE_AF, GPIO_PUPD_NONE, OTYPE_NA, GPIO_OSPEED_MAX, GPIO_AF_7);
    pin_config(BSP_PIN_MIDI_OUT_B,      GPIO_MODE_AF, GPIO_PUPD_NONE, OTYPE_NA, GPIO_OSPEED_MAX, GPIO_AF_8);

    /* LED I2C0 */
    pin_config(BSP_PIN_LED_I2C0_SCL, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_AF_4);
    pin_config(BSP_PIN_LED_I2C0_SDA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_AF_4);

    /* rotary encoder */
    pin_config(BSP_PIN_ENCODER_A, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, OTYPE_NA, GPIO_OSPEED_MAX, AF_NA);
    pin_config(BSP_PIN_ENCODER_B, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, OTYPE_NA, GPIO_OSPEED_MAX, AF_NA);

    /* buttons & knobs: 74HC165 chain */
    OUT_PP_MAX(BSP_PIN_SCAN_CLK);
    pin_config(BSP_PIN_SCAN_IN, GPIO_MODE_INPUT, GPIO_PUPD_NONE, OTYPE_NA, GPIO_OSPEED_MAX, AF_NA);
    OUT_PP_MAX(BSP_PIN_SCAN_PL);

    /* OLED, SPI1 shared with the SPI flash */
    OUT_PP_MAX(BSP_PIN_OLED_DI);
    OUT_PP_MAX(BSP_PIN_OLED_RST);
    OUT_PP_MAX(BSP_PIN_OLED_CS);
    pin_config(BSP_PIN_SPI1_SCK,  GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_AF_5);
    pin_config(BSP_PIN_SPI1_MISO, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_AF_5);
    pin_config(BSP_PIN_SPI1_MOSI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_AF_5);

    /* SPI flash */
    OUT_PP_MAX(BSP_PIN_FLASH_CS);

    /* CV: analog switch + DAC on SPI2 */
    OUT_PP_MAX(BSP_PIN_CV_SW_EN);
    OUT_PP_MAX(BSP_PIN_CV_SW_S0);
    OUT_PP_MAX(BSP_PIN_CV_SW_S1);
    OUT_PP_MAX(BSP_PIN_CV_DAC_CS);
    pin_config(BSP_PIN_CV_DAC_SCLK, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_AF_6);
    pin_config(BSP_PIN_CV_DAC_DIN,  GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_AF_6);

    /* gates */
    OUT_PP_MAX(BSP_PIN_GATE1_0V);
    OUT_PP_MAX(BSP_PIN_GATE1_5V);
    OUT_PP_MAX(BSP_PIN_GATE1_10V);
    OUT_PP_MAX(BSP_PIN_GATE2_0V);
    OUT_PP_MAX(BSP_PIN_GATE2_5V);
    OUT_PP_MAX(BSP_PIN_GATE2_10V);
    OUT_PP_MAX(BSP_PIN_GATE3_0V);
    OUT_PP_MAX(BSP_PIN_GATE3_5V);
    OUT_PP_MAX(BSP_PIN_GATE3_10V);
    OUT_PP_MAX(BSP_PIN_GATE4_0V);
    OUT_PP_MAX(BSP_PIN_GATE4_5V);
    OUT_PP_MAX(BSP_PIN_GATE4_10V);

    /* sync out (GPIO), sync in */
    pin_config(BSP_PIN_SYNC_OUT_TIP,  GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, AF_NA);
    pin_config(BSP_PIN_SYNC_OUT_RING, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, AF_NA);
    pin_config(BSP_PIN_SYNC_IN_TIP,   GPIO_MODE_INPUT,  GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_AF_1);
    pin_config(BSP_PIN_SYNC_IN_RING,  GPIO_MODE_INPUT,  GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, AF_NA);

    /* footswitches */
    pin_config(BSP_PIN_FOOTSW1, GPIO_MODE_INPUT, GPIO_PUPD_NONE, OTYPE_NA, GPIO_OSPEED_MAX, AF_NA);
    pin_config(BSP_PIN_FOOTSW2, GPIO_MODE_INPUT, GPIO_PUPD_NONE, OTYPE_NA, GPIO_OSPEED_MAX, AF_NA);

    /* default levels: all chip selects inactive */
    gpio_bit_set(BSP_PIN_CV_DAC_CS);
    gpio_bit_set(BSP_PIN_OLED_CS);
    gpio_bit_set(BSP_PIN_FLASH_CS);
    gpio_bit_set(BSP_PIN_USB_DM);
    gpio_bit_set(BSP_PIN_USB_DP);
}
