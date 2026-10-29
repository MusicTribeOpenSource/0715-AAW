/*
 * bsp_gpio.h - BCR32 pin map, peripheral clocks and GPIO init
 * Pins are (port, pin) pairs for the GD32 SPL, e.g.
 *   gpio_bit_set(BSP_PIN_SCAN_CLK);  gpio_input_bit_get(BSP_PIN_SCAN_IN);
 */
#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include "gd32f4xx.h"

/* port A */
#define BSP_PIN_SYNC_OUT_TIP        GPIOA, GPIO_PIN_0
#define BSP_PIN_SYNC_OUT_RING       GPIOA, GPIO_PIN_1
#define BSP_PIN_USART1_MIDI_OUT     GPIOA, GPIO_PIN_2
#define BSP_PIN_USART1_MIDI_IN      GPIOA, GPIO_PIN_3
#define BSP_PIN_ENCODER_B           GPIOA, GPIO_PIN_4     /* SELECT encoder B */
#define BSP_PIN_SCAN_CLK            GPIOA, GPIO_PIN_5     /* 74HC165 chain clock */
#define BSP_PIN_SCAN_IN             GPIOA, GPIO_PIN_6     /* 74HC165 chain serial out */
#define BSP_PIN_SCAN_PL             GPIOA, GPIO_PIN_7     /* 74HC165 chain parallel load */
#define BSP_PIN_OLED_RST            GPIOA, GPIO_PIN_8
#define BSP_PIN_SYNC_IN_TIP         GPIOA, GPIO_PIN_9     /* TIMER0_CH1 */
#define BSP_PIN_SYNC_IN_RING        GPIOA, GPIO_PIN_10    /* EXTI10 */
#define BSP_PIN_USB_DM              GPIOA, GPIO_PIN_11
#define BSP_PIN_USB_DP              GPIOA, GPIO_PIN_12
#define BSP_PIN_ENCODER_A           GPIOA, GPIO_PIN_15    /* SELECT encoder A */

/* port B */
#define BSP_PIN_GATE3_10V           GPIOB, GPIO_PIN_0
#define BSP_PIN_GATE3_5V            GPIOB, GPIO_PIN_1
#define BSP_PIN_FOOTSW1             GPIOB, GPIO_PIN_4
#define BSP_PIN_CV_DAC_DIN          GPIOB, GPIO_PIN_5     /* SPI2_MOSI */
#define BSP_PIN_GATE3_0V            GPIOB, GPIO_PIN_6
#define BSP_PIN_FOOTSW2             GPIOB, GPIO_PIN_7
#define BSP_PIN_LED_I2C0_SCL        GPIOB, GPIO_PIN_8
#define BSP_PIN_LED_I2C0_SDA        GPIOB, GPIO_PIN_9
#define BSP_PIN_CV_SW_EN            GPIOB, GPIO_PIN_10
#define BSP_PIN_OLED_DI             GPIOB, GPIO_PIN_12
#define BSP_PIN_SPI1_SCK            GPIOB, GPIO_PIN_13    /* OLED + SPI flash */
#define BSP_PIN_SPI1_MISO           GPIOB, GPIO_PIN_14
#define BSP_PIN_SPI1_MOSI           GPIOB, GPIO_PIN_15

/* port C */
#define BSP_PIN_CV_SW_S0            GPIOC, GPIO_PIN_0
#define BSP_PIN_CV_SW_S1            GPIOC, GPIO_PIN_1
#define BSP_PIN_GATE1_10V           GPIOC, GPIO_PIN_2
#define BSP_PIN_GATE1_5V            GPIOC, GPIO_PIN_3
#define BSP_PIN_GATE1_0V            GPIOC, GPIO_PIN_4
#define BSP_PIN_GATE2_10V           GPIOC, GPIO_PIN_5
#define BSP_PIN_MIDI_OUT_B          GPIOC, GPIO_PIN_6     /* USART5_TX */
#define BSP_PIN_FLASH_CS            GPIOC, GPIO_PIN_7
#define BSP_PIN_GATE2_5V            GPIOC, GPIO_PIN_8
#define BSP_PIN_GATE2_0V            GPIOC, GPIO_PIN_9
#define BSP_PIN_CV_DAC_SCLK         GPIOC, GPIO_PIN_10    /* SPI2_SCK */
#define BSP_PIN_CV_DAC_CS           GPIOC, GPIO_PIN_11
#define BSP_PIN_OLED_CS             GPIOC, GPIO_PIN_12
#define BSP_PIN_GATE4_10V           GPIOC, GPIO_PIN_13
#define BSP_PIN_GATE4_5V            GPIOC, GPIO_PIN_14
#define BSP_PIN_GATE4_0V            GPIOC, GPIO_PIN_15

/* port D */
#define BSP_PIN_PSU_STATUS          GPIOD, GPIO_PIN_2     /* not configured */

/* Peripheral clocks of the whole board (without SystemInit / SysTick) */
void BspClock_Init(void);

/* All pin modes and the default levels (chip selects high) */
void BspGpio_Init(void);

#endif /* BSP_GPIO_H */
