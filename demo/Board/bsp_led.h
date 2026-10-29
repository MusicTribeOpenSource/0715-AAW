/*
 * bsp_led.h - LED drivers: 5x HT16K33 on I2C0 (100 kHz class bus, 200 kHz SCL)
 * Hardware part only: init, brightness, raw writes, all on / off.
 * Not thread safe: one caller thread at a time.
 */
#ifndef BSP_LED_H
#define BSP_LED_H

#include <stdint.h>

/* HT16K33 8 bit write addresses */
#define BSP_LED_CHIP_BTN        0xE0U
#define BSP_LED_CHIP_ENC_ROW1   0xE4U
#define BSP_LED_CHIP_ENC_ROW2   0xE6U
#define BSP_LED_CHIP_ENC_ROW3   0xE8U
#define BSP_LED_CHIP_ENC_ROW4   0xEAU
#define BSP_LED_NUM_CHIPS       5U
#define BSP_LED_RAM_BYTES       16U     /* display RAM 0x00..0x0F */

extern const uint8_t g_bsp_led_chips[BSP_LED_NUM_CHIPS];

/* I2C0 master */
void BspLedI2c_Init(void);

/* Raw transfer; returns 0 on success, -1 on timeout / no ACK (bus released with STOP) */
int BspLedI2c_Write(uint8_t chip, const uint8_t *data, uint32_t len);

/* Two byte frame [first, second] */
int BspLed_Write2(uint8_t chip, uint8_t first, uint8_t second);

/* HT16K33 setup of every chip (oscillator, INT, display on, brightness 0..15) and all
 * LEDs off. Returns the number of chips that failed. */
uint32_t BspLed_Init(uint8_t brightness);

/* Every LED of every chip on or off; returns the number of failed writes */
uint32_t BspLed_All(uint8_t on);

#endif /* BSP_LED_H */
