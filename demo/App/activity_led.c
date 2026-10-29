/*
 * activity_led.c - MIDI activity LEDs, see activity_led.h
 */
#include "activity_led.h"
#include "bsp_led.h"

typedef struct
{
    uint8_t ram;        /* HT16K33 display RAM address */
    uint8_t bit;
} LedPos_t;

static const LedPos_t s_pos[ACTIVITY_LED_NUM] =
{
    { 11U, 0x01U },     /* MIDI IN */
    {  9U, 0x01U },     /* OUT A */
    {  9U, 0x02U },     /* OUT B */
    {  7U, 0x01U },     /* USB */
};

static uint32_t s_off_ms[ACTIVITY_LED_NUM];     /* switch off at */
static uint8_t  s_on[ACTIVITY_LED_NUM];
static uint8_t  s_pending;                      /* an LED changed state */

void ActivityLed_Trigger(ActivityLed_t led, uint32_t now_ms)
{
    if (led >= ACTIVITY_LED_NUM)
    {
        return;
    }
    s_off_ms[led] = now_ms + ACTIVITY_LED_FLASH_MS;
    if (s_on[led] == 0U)
    {
        s_on[led] = 1U;
        s_pending = 1U;
    }
}

void ActivityLed_Poll(uint32_t now_ms)
{
    static uint8_t written[BSP_LED_RAM_BYTES];      /* last value per RAM byte */
    uint8_t value[BSP_LED_RAM_BYTES] = {0};
    uint32_t i;

    for (i = 0U; i < ACTIVITY_LED_NUM; i++)
    {
        if ((s_on[i] != 0U) && ((int32_t)(now_ms - s_off_ms[i]) >= 0))
        {
            s_on[i] = 0U;
            s_pending = 1U;
        }
    }
    if (s_pending == 0U)
    {
        return;
    }
    s_pending = 0U;

    for (i = 0U; i < ACTIVITY_LED_NUM; i++)
    {
        if (s_on[i] != 0U)
        {
            value[s_pos[i].ram] |= s_pos[i].bit;
        }
    }
    /* only the bytes holding activity LEDs, only when changed */
    for (i = 0U; i < ACTIVITY_LED_NUM; i++)
    {
        const uint8_t ram = s_pos[i].ram;

        if (value[ram] != written[ram])
        {
            if (BspLed_Write2(BSP_LED_CHIP_BTN, ram, value[ram]) == 0)
            {
                written[ram] = value[ram];
            }
            else
            {
                s_pending = 1U;     /* retry on the next poll */
            }
        }
    }
}
