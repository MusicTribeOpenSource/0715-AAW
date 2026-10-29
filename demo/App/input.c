/*
 * input.c - button and encoder event source, see input.h
 */
#include "input.h"
#include "bsp_buttons.h"
#include "bsp_gpio.h"

#define SCAN_PERIOD_MS      1U      /* chain read every 1 ms */
#define EVENT_PERIOD_MS     20U     /* button check at 50 Hz (debounce by sampling) */
#define STEPS_PER_DETENT    4U      /* quadrature steps per detent */
#define NUM_ENCODERS        (INPUT_NUM_KNOBS + 1U)  /* index 32 = select encoder */
#define ENC_IDX_SELECT      INPUT_NUM_KNOBS
#define QUEUE_SIZE          16U     /* power of two */

/* 74HC165 chain positions of the button and the encoder registers */
static const uint8_t s_button_regs[8]  = {0U, 3U, 6U, 9U, 12U, 13U, 14U, 15U};
static const uint8_t s_encoder_regs[8] = {1U, 2U, 4U, 5U, 7U, 8U, 10U, 11U};

typedef struct
{
    uint8_t prev;           /* last gray state 0..3 */
    uint8_t valid;          /* prev holds a real reading */
    uint8_t cw_steps;
    uint8_t ccw_steps;
    int8_t  detents;        /* since the last event */
} Encoder_t;

static Encoder_t s_enc[NUM_ENCODERS];
static uint8_t   s_btn_pressed[INPUT_NUM_BUTTONS];    /* last 1 ms reading */
static uint8_t   s_btn_sampled[INPUT_NUM_BUTTONS];    /* 20 ms sample */

static InputEvent_t s_queue[QUEUE_SIZE];
static uint32_t s_q_head, s_q_tail, s_dropped;
static uint32_t s_last_scan_ms, s_last_event_ms;

static void Post(InputEventType_t type, uint8_t id, int8_t delta)
{
    if ((s_q_head - s_q_tail) >= QUEUE_SIZE)
    {
        s_dropped++;
        return;
    }
    s_queue[s_q_head & (QUEUE_SIZE - 1U)].type  = type;
    s_queue[s_q_head & (QUEUE_SIZE - 1U)].id    = id;
    s_queue[s_q_head & (QUEUE_SIZE - 1U)].delta = delta;
    s_q_head++;
}

/* +1 clockwise, -1 counter clockwise, 0 invalid jump */
static int RotateDir(uint8_t curr, uint8_t prev)
{
    static const uint8_t cw_prev[4]  = {1U, 3U, 0U, 2U};    /* indexed by curr */
    static const uint8_t ccw_prev[4] = {2U, 0U, 3U, 1U};

    if (prev == cw_prev[curr])
    {
        return 1;
    }
    if (prev == ccw_prev[curr])
    {
        return -1;
    }
    return 0;
}

/* one detent after 4 steps in the same direction */
static void EncoderUpdate(Encoder_t *e, uint8_t curr)
{
    int dir;

    if (e->valid == 0U)
    {
        e->prev = curr;
        e->valid = 1U;
        return;
    }
    if (curr == e->prev)
    {
        return;
    }
    dir = RotateDir(curr, e->prev);
    e->prev = curr;
    if (dir > 0)
    {
        e->ccw_steps = 0U;
        if (++e->cw_steps >= STEPS_PER_DETENT)
        {
            e->cw_steps = 0U;
            if (e->detents < 127)
            {
                e->detents++;
            }
        }
    }
    else if (dir < 0)
    {
        e->cw_steps = 0U;
        if (++e->ccw_steps >= STEPS_PER_DETENT)
        {
            e->ccw_steps = 0U;
            if (e->detents > -127)
            {
                e->detents--;
            }
        }
    }
}

/* SELECT encoder A/B lines -> gray state 0..3 */
static uint8_t SelectState(void)
{
    const uint8_t a = (gpio_input_bit_get(BSP_PIN_ENCODER_A) != RESET) ? 1U : 0U;
    const uint8_t b = (gpio_input_bit_get(BSP_PIN_ENCODER_B) != RESET) ? 1U : 0U;
    static const uint8_t state[4] = {2U, 0U, 3U, 1U};    /* index (a << 1) | b */

    return state[(a << 1) | b];
}

static void Scan(void)
{
    uint8_t regs[BSP_BUTTONS_NUM_SHIFT_REGS];
    uint32_t i, k;

    BspButtons_Read(regs);

    /* buttons, active low; bit -> button id as wired */
    for (i = 0U; i < INPUT_NUM_BUTTONS; i++)
    {
        const uint32_t d = i / 8U, r = i % 8U;
        uint32_t id = ((7U - d) * 8U) + r;

        if ((id >= 4U) && (id < 8U))
        {
            id += 4U;
        }
        else if ((id >= 8U) && (id < 12U))
        {
            id -= 4U;
        }
        s_btn_pressed[id] = ((regs[s_button_regs[d]] & (1U << r)) == 0U) ? 1U : 0U;
    }

    /* knobs: 4 per register, 2 bits each */
    for (i = 0U; i < 8U; i++)
    {
        for (k = 0U; k < 4U; k++)
        {
            const uint8_t value = (uint8_t)((regs[s_encoder_regs[i]] >> (k * 2U)) & 0x03U);
            EncoderUpdate(&s_enc[(INPUT_NUM_KNOBS - 1U) - ((i * 4U) + k)], value);
        }
    }

    EncoderUpdate(&s_enc[ENC_IDX_SELECT], SelectState());
}

static void MakeEvents(void)
{
    uint32_t i;

    for (i = 0U; i < INPUT_NUM_BUTTONS; i++)
    {
        if ((s_btn_pressed[i] != 0U) && (s_btn_sampled[i] == 0U))
        {
            Post(INPUT_EV_BUTTON, (uint8_t)(i + 1U), 0);
        }
        else if ((s_btn_pressed[i] == 0U) && (s_btn_sampled[i] != 0U))
        {
            Post(INPUT_EV_BUTTON_UP, (uint8_t)(i + 1U), 0);
        }
        s_btn_sampled[i] = s_btn_pressed[i];
    }
    for (i = 0U; i < NUM_ENCODERS; i++)
    {
        if (s_enc[i].detents != 0)
        {
            Post(INPUT_EV_ENCODER, (uint8_t)((i == ENC_IDX_SELECT) ? INPUT_ENC_SELECT : (i + 1U)), s_enc[i].detents);
            s_enc[i].detents = 0;
        }
    }
}

void Input_Poll(uint32_t now_ms)
{
    if ((now_ms - s_last_scan_ms) < SCAN_PERIOD_MS)
    {
        return;
    }
    s_last_scan_ms = now_ms;
    Scan();

    if ((now_ms - s_last_event_ms) >= EVENT_PERIOD_MS)
    {
        s_last_event_ms = now_ms;
        MakeEvents();
    }
}

int Input_GetEvent(InputEvent_t *ev)
{
    if (s_q_tail == s_q_head)
    {
        return 0;
    }
    *ev = s_queue[s_q_tail & (QUEUE_SIZE - 1U)];
    s_q_tail++;
    return 1;
}

uint8_t Input_IsPressed(uint8_t id)
{
    if ((id < 1U) || (id > INPUT_NUM_BUTTONS))
    {
        return 0U;
    }
    return s_btn_sampled[id - 1U];
}

uint32_t Input_GetDropped(void)
{
    return s_dropped;
}
