/*
 * activity_led.h - MIDI activity LEDs: MIDI IN, MIDI OUT A, MIDI OUT B, USB
 *
 * Each trigger lights the LED for ACTIVITY_LED_FLASH_MS; ongoing traffic keeps it lit.
 * Main loop only (the LED I2C bus is not used from interrupts).
 * Positions on the button LED chip (HT16K33 0xE0):
 *   USB = RAM 7 bit 0, OUT A = RAM 9 bit 0, OUT B = RAM 9 bit 1, IN = RAM 11 bit 0
 */
#ifndef ACTIVITY_LED_H
#define ACTIVITY_LED_H

#include <stdint.h>

#define ACTIVITY_LED_FLASH_MS   50U

typedef enum
{
    ACTIVITY_LED_MIDI_IN = 0,
    ACTIVITY_LED_MIDI_A,
    ACTIVITY_LED_MIDI_B,
    ACTIVITY_LED_USB,
    ACTIVITY_LED_NUM
} ActivityLed_t;

/* Light the LED now (written on the next ActivityLed_Poll) */
void ActivityLed_Trigger(ActivityLed_t led, uint32_t now_ms);

/* Call from the main loop: switches LEDs off after the flash time, writes changed bytes */
void ActivityLed_Poll(uint32_t now_ms);

#endif /* ACTIVITY_LED_H */
