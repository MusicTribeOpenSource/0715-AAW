/*
 * main.c - BCR32 bare-metal demo (no RTOS)
 *
 * Image at 0x08010000, started by the BCR32 bootloader; SystemInit: 120 MHz PLL from
 * the 8 MHz HXTAL.
 * Demo: SysTick 1 ms tick, one LED at a time walks once over every HT16K33 output
 * (chip, RAM byte, bit), then all LEDs stay off.
 * Outputs without a fitted LED show as dark steps.
 * OLED (SSD1306 128x32 on SPI1) shows "BCR32" centered at power up, then the last
 * button / encoder event (input.c event queue, see OnInputEvent).
 * USB MIDI (GD32 USBFS device stack, ports "BCR-32 CTRL"/"BCR-32 ZAQ"):
 * every packet received from the host is sent back unchanged (loopback).
 * SYNC OUT -> SYNC IN loopback (cable plugged between them, sync_loop.c): result every
 * second, OLED "SYNC OK" / "SYNC FAIL" while the SELECT button is held.
 * Knobs 1..4 set CV1..4 (cv_out.c, 0..10 V in 0.1 V steps), value bottom right of the OLED.
 * BTN1..16 set GATE1..4 to 0 / 5 / 10 V (GateFromButton), "GATEx:yV" bottom right of the OLED.
 * DIN MIDI: every byte from MIDI IN goes out unchanged on MIDI OUT A and B (midi_thru.c).
 * Activity LEDs flash on traffic: MIDI IN / OUT A / OUT B with the DIN thru, USB with
 * the USB MIDI loopback (activity_led.c).
 * External SPI flash (W25X40CL, DMA, bsp_flash.c): self test on sector 127 at power up
 * (flash_test.c).
 * Internal flash (bsp_iflash.c, sector 5 only): self test at power up (iflash_test.c).
 * USB SysEx (sysex.c): 0x74 mode query answered, 0x77 starts the bootloader.
 * T1 + T2 + T3 + T4 (BTN18..21) held together: start the bootloader (boot_jump.c) for a
 * firmware update with tools/update/windows/update.exe.
 */
#include <stdio.h>
#include <string.h>

#include "gd32f4xx.h"

#include "input.h"
#include "sync_loop.h"
#include "cv_out.h"
#include "midi_thru.h"
#include "activity_led.h"
#include "flash_test.h"
#include "iflash_test.h"
#include "boot_jump.h"
#include "sysex.h"

#include "bsp_cv.h"
#include "bsp_gpio.h"
#include "bsp_led.h"
#include "bsp_delay.h"
#include "bsp_spi1.h"
#include "bsp_usb.h"
#include "ssd1306.h"
#include "usbd_midi.h"

#define LED_BRIGHTNESS      0x0FU   /* HT16K33 duty 0..15 */
#define STEP_PERIOD         100U    /* ms per LED */
#define LED_WRITE_GAP_US    50U     /* as bsp_led.c: gap between two HT16K33 writes */

static volatile uint32_t s_tick_ms;

void SysTick_Handler(void)
{
    s_tick_ms++;
}

static void OledShowSplash(void)
{
    static char text[] = "BCR32";
    const uint8_t w = (uint8_t)((sizeof(text) - 1U) * Font_16x26.FontWidth);

    ssd1306_Fill(Black);
    ssd1306_SetCursor((uint8_t)((SSD1306_WIDTH - w) / 2U),
                      (uint8_t)((SSD1306_HEIGHT - Font_16x26.FontHeight) / 2U));
    (void)ssd1306_WriteString(text, Font_16x26, White);
    ssd1306_UpdateScreen();
}

static SyncLoopResult_t s_sync_last;        /* last 1 s SYNC loopback result */
static uint8_t s_sync_valid;                /* s_sync_last holds a result */
static uint8_t s_sync_view;                 /* SELECT held: OLED shows the SYNC state */

static uint8_t SyncOk(const SyncLoopResult_t *r)
{
    return ((r->tip.ok != 0U) && (r->ring.ok != 0U)) ? 1U : 0U;
}

/* "SYNC OK" / "SYNC FAIL", below it the input/output edge counts of tip and ring */
static void OledShowSync(void)
{
    char line[24];

    ssd1306_Fill(Black);
    ssd1306_SetCursor(0U, 0U);
    if (s_sync_valid == 0U)
    {
        (void)ssd1306_WriteString("SYNC ...", Font_11x18, White);   /* first second */
    }
    else
    {
        (void)ssd1306_WriteString((SyncOk(&s_sync_last) != 0U) ? "SYNC OK" : "SYNC FAIL", Font_11x18, White);
        (void)snprintf(line, sizeof(line), "T%u/%u R%u/%u",
                       (unsigned int)s_sync_last.tip.in_edges, (unsigned int)s_sync_last.tip.out_edges,
                       (unsigned int)s_sync_last.ring.in_edges, (unsigned int)s_sync_last.ring.out_edges);
        ssd1306_SetCursor(0U, (uint8_t)(SSD1306_HEIGHT - Font_7x10.FontHeight));
        (void)ssd1306_WriteString(line, Font_7x10, White);
    }
    ssd1306_UpdateScreen();
}

/* SYNC loopback result (1 s window): kept; on the OLED only while SELECT is held */
static void OnSyncResult(const SyncLoopResult_t *r)
{
    s_sync_last = *r;
    s_sync_valid = 1U;
    if (s_sync_view != 0U)
    {
        OledShowSync();
    }
}

/* BTN1..16 in groups of 4, one group per gate: 1st = 0 V, 2nd = 5 V, 3rd = 10 V,
 * 4th unused (BTN1-3 GATE1, BTN5-7 GATE2, BTN9-11 GATE3, BTN13-15 GATE4); level latched.
 * Writes "GATEx:yV" to label and returns 1 for a gate button, else returns 0. */
static int GateFromButton(uint32_t id, char *label, uint32_t size)
{
    static const BspGateLevel_t level[3] = {BSP_GATE_0V, BSP_GATE_5V, BSP_GATE_10V};
    static const char *const name[3] = {"0V", "5V", "10V"};
    uint32_t gate, pos;

    if ((id < 1U) || (id > (BSP_CV_NUM_CHANNELS * 4U)))
    {
        return 0;
    }
    gate = (id - 1U) / 4U;
    pos  = (id - 1U) % 4U;
    if (pos >= 3U)
    {
        return 0;
    }
    BspGate_Set(gate, level[pos]);
    (void)snprintf(label, size, "GATE%u:%s", (unsigned int)(gate + 1U), name[pos]);
    return 1;
}

/* Input event handler: the OLED shows the last event
 *   SELECT button: SYNC state while held (OledShowSync), blank screen on release
 *   button:  "BTNn" centered, large font (other releases change nothing)
 *   encoder: "ENCODER +x" / "ENCODER -x" (detents in one 20 ms detection), below it
 *            which encoder ("KNOB n" or "SELECT") */
static void OnInputEvent(const InputEvent_t *ev)
{
    char line[16];

    if (ev->id == INPUT_BTN_SELECT)
    {
        if (ev->type == INPUT_EV_BUTTON)
        {
            s_sync_view = 1U;
            OledShowSync();
            return;
        }
        if (ev->type == INPUT_EV_BUTTON_UP)
        {
            s_sync_view = 0U;
            ssd1306_Fill(Black);
            ssd1306_UpdateScreen();
            return;
        }
    }
    if (ev->type == INPUT_EV_BUTTON_UP)
    {
        return;
    }

    ssd1306_Fill(Black);
    if (ev->type == INPUT_EV_BUTTON)
    {
        char gate[12];
        const uint32_t n = (uint32_t)snprintf(line, sizeof(line), "BTN%u", ev->id);

        if (GateFromButton(ev->id, gate, sizeof(gate)) != 0)
        {
            /* gate button: "BTNn" top left, "GATEx:yV" bottom right (layout as the knobs) */
            const uint8_t w = (uint8_t)(strlen(gate) * Font_7x10.FontWidth);

            ssd1306_SetCursor(0U, 0U);
            (void)ssd1306_WriteString(line, Font_11x18, White);
            ssd1306_SetCursor((uint8_t)(SSD1306_WIDTH - w), (uint8_t)(SSD1306_HEIGHT - Font_7x10.FontHeight));
            (void)ssd1306_WriteString(gate, Font_7x10, White);
        }
        else
        {
            const uint8_t w = (uint8_t)(n * Font_16x26.FontWidth);

            ssd1306_SetCursor((uint8_t)((SSD1306_WIDTH - w) / 2U),
                              (uint8_t)((SSD1306_HEIGHT - Font_16x26.FontHeight) / 2U));
            (void)ssd1306_WriteString(line, Font_16x26, White);
        }
    }
    else
    {
        (void)snprintf(line, sizeof(line), "ENCODER %+d", ev->delta);
        ssd1306_SetCursor(0U, 0U);
        (void)ssd1306_WriteString(line, Font_11x18, White);

        if (ev->id == INPUT_ENC_SELECT)
        {
            (void)snprintf(line, sizeof(line), "SELECT");
        }
        else
        {
            (void)snprintf(line, sizeof(line), "KNOB %u", ev->id);
        }
        ssd1306_SetCursor(0U, (uint8_t)(SSD1306_HEIGHT - Font_7x10.FontHeight));
        (void)ssd1306_WriteString(line, Font_7x10, White);

        /* knobs 1..4 set CV1..4, the value bottom right */
        if ((ev->id >= 1U) && (ev->id <= CV_OUT_NUM))
        {
            const uint32_t dv = CvOut_Add(ev->id - 1U, ev->delta);
            const uint8_t w = (uint8_t)((uint32_t)snprintf(line, sizeof(line), "CV%u:%u.%uV",
                                                           ev->id, (unsigned int)(dv / 10U),
                                                           (unsigned int)(dv % 10U)) * Font_7x10.FontWidth);

            ssd1306_SetCursor((uint8_t)(SSD1306_WIDTH - w), (uint8_t)(SSD1306_HEIGHT - Font_7x10.FontHeight));
            (void)ssd1306_WriteString(line, Font_7x10, White);
        }
    }
    ssd1306_UpdateScreen();
}

/* Loopback: host -> device packets go straight back, followed by the SysEx replies
 * (sysex.c); data stays in the receive buffer (host NAKed when full) while the
 * previous IN transfer is pending */
static void UsbMidiLoopback(void)
{
    static uint8_t buf[64 + 16];
    uint32_t n;

    if ((IsUsbConnected() == 0U) || (UsbMidiTxBusy() != 0U))
    {
        return;
    }
    n = UsbMidiRxRead(buf, 64U);
    if (n == 0U)
    {
        return;
    }
    Sysex_UsbRx(buf, n);
    n += Sysex_UsbTx(&buf[n], sizeof(buf) - n);
    USBTxNBytes(buf, n);
    ActivityLed_Trigger(ACTIVITY_LED_USB, s_tick_ms);
}

int main(void)
{
    uint32_t last = 0U;
    uint32_t chip = 0U, addr = 0U, bit = 0U;    /* LED currently lit */
    uint8_t led_done = 0U;                      /* walk finished, all LEDs off */

    /* SystemInit already ran from Reset_Handler; IRQs are still masked (CPSID i) */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    (void)SysTick_Config(SystemCoreClock / 1000U);    /* also needed by BspDelay_Us */
    __enable_irq();

    BspClock_Init();
    BspGpio_Init();
    SyncLoop_Init();
    CvOut_Init();       /* ~1 ms busy wait */
    MidiThru_Init();
    BspSpi1_Init();
    Oled_Init();        /* reset, SSD1306 setup, blank screen */
    OledShowSplash();
    FlashTest_Start(s_tick_ms);     /* shares SPI1 with the OLED */
    IFlashTest_Start(s_tick_ms);
    BspUsb_Init();
    BspLedI2c_Init();
    (void)BspLed_Init(LED_BRIGHTNESS);
    (void)BspLed_Write2(g_bsp_led_chips[chip], (uint8_t)addr, (uint8_t)(1U << bit));

    for (;;)
    {
        InputEvent_t ev;
        SyncLoopResult_t sync;

        UsbMidiLoopback();

        Input_Poll(s_tick_ms);
        while (Input_GetEvent(&ev) != 0)
        {
            OnInputEvent(&ev);
        }

        /* SysEx 0x77, or T1 + T2 + T3 + T4 held: bootloader for a firmware update */
        if ((Sysex_BootRequested() != 0U) ||
            ((Input_IsPressed(INPUT_BTN_T1) != 0U) && (Input_IsPressed(INPUT_BTN_T1 + 1U) != 0U) &&
             (Input_IsPressed(INPUT_BTN_T1 + 2U) != 0U) && (Input_IsPressed(INPUT_BTN_T4) != 0U)))
        {
            BootJump_Start();
        }

        CvOut_Poll(s_tick_ms);
        if (MidiThru_Poll() != 0U)
        {
            ActivityLed_Trigger(ACTIVITY_LED_MIDI_IN, s_tick_ms);
            ActivityLed_Trigger(ACTIVITY_LED_MIDI_A, s_tick_ms);
            ActivityLed_Trigger(ACTIVITY_LED_MIDI_B, s_tick_ms);
        }
        ActivityLed_Poll(s_tick_ms);
        FlashTest_Poll(s_tick_ms);
        IFlashTest_Poll(s_tick_ms);

        if (SyncLoop_Poll(s_tick_ms, &sync) != 0)
        {
            OnSyncResult(&sync);
        }

        if ((led_done == 0U) && ((s_tick_ms - last) >= STEP_PERIOD))
        {
            last += STEP_PERIOD;

            /* previous LED off: clear its whole RAM byte */
            (void)BspLed_Write2(g_bsp_led_chips[chip], (uint8_t)addr, 0x00U);
            BspDelay_Us(LED_WRITE_GAP_US);

            /* next position: bit -> RAM byte -> chip; one pass only */
            if (++bit == 8U)
            {
                bit = 0U;
                if (++addr == BSP_LED_RAM_BYTES)
                {
                    addr = 0U;
                    if (++chip == BSP_LED_NUM_CHIPS)
                    {
                        led_done = 1U;
                    }
                }
            }

            if (led_done == 0U)
            {
                (void)BspLed_Write2(g_bsp_led_chips[chip], (uint8_t)addr, (uint8_t)(1U << bit));
            }
        }
    }
}
