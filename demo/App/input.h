/*
 * input.h - button and encoder event source (bare metal, polled from the main loop)
 *
 * Scans the 74HC165 chain (64 buttons, 32 knob encoders) and the SELECT encoder
 * (PA15/PA4) every 1 ms; every 20 ms it queues events:
 *   INPUT_EV_BUTTON   button pressed, id 1..64
 *   INPUT_EV_BUTTON_UP button released, id 1..64
 *   INPUT_EV_ENCODER  encoder turned, id 1..32 = knob, INPUT_ENC_SELECT = select encoder,
 *                     delta = detents in that 20 ms window (+ clockwise, - counter clockwise)
 * Bit mapping and decoding follow the BCR32 wiring.
 */
#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#define INPUT_NUM_BUTTONS   64U
#define INPUT_NUM_KNOBS     32U
#define INPUT_ENC_SELECT    33U
#define INPUT_BTN_SELECT    24U     /* SELECT button */
#define INPUT_BTN_T1        18U     /* T1..T4 = BTN18..21 */
#define INPUT_BTN_T4        21U

typedef enum
{
    INPUT_EV_BUTTON = 0,
    INPUT_EV_BUTTON_UP,
    INPUT_EV_ENCODER
} InputEventType_t;

typedef struct
{
    InputEventType_t type;
    uint8_t id;
    int8_t delta;       /* encoder only */
} InputEvent_t;

/* Call from the main loop as often as possible; scans when a new ms started */
void Input_Poll(uint32_t now_ms);

/* Next queued event; returns 0 when the queue is empty */
int Input_GetEvent(InputEvent_t *ev);

/* Button id 1..64 held now (debounced 20 ms sample); 0 for other ids */
uint8_t Input_IsPressed(uint8_t id);

/* Events dropped because the queue was full */
uint32_t Input_GetDropped(void);

#endif /* INPUT_H */
