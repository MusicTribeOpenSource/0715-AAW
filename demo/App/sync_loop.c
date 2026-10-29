/*
 * sync_loop.c - SYNC OUT -> SYNC IN loopback test, see sync_loop.h
 */
#include "sync_loop.h"
#include "bsp_gpio.h"

#define TIP_HALF_PERIOD_MS      5U      /* 100 Hz square wave */
#define RING_HALF_PERIOD_MS     10U     /* 50 Hz square wave */
#define WINDOW_MS               1000U
#define BSP_IRQ_PRIO_SYNC_IN    2U      /* bsp_irq.h: SYNC IN gate */

typedef struct
{
    uint32_t out_port, out_pin;
    uint32_t in_port, in_pin;
    uint32_t half_period_ms;
    uint8_t  level;                     /* output level now */
    SyncLoopChannel_t acc;              /* current window (in_edges unused) */
} Channel_t;

static Channel_t s_tip  = {BSP_PIN_SYNC_OUT_TIP,  BSP_PIN_SYNC_IN_TIP,  TIP_HALF_PERIOD_MS,  0U, {0}};
static Channel_t s_ring = {BSP_PIN_SYNC_OUT_RING, BSP_PIN_SYNC_IN_RING, RING_HALF_PERIOD_MS, 0U, {0}};

static volatile uint32_t s_tip_in_edges, s_ring_in_edges;   /* ISR counters */
static uint32_t s_last_ms, s_window_start_ms;

void EXTI5_9_IRQHandler(void)
{
    if (exti_interrupt_flag_get(EXTI_9) != RESET)
    {
        exti_interrupt_flag_clear(EXTI_9);
        s_tip_in_edges++;
    }
}

void EXTI10_15_IRQHandler(void)
{
    if (exti_interrupt_flag_get(EXTI_10) != RESET)
    {
        exti_interrupt_flag_clear(EXTI_10);
        s_ring_in_edges++;
    }
}

void SyncLoop_Init(void)
{
    gpio_bit_reset(BSP_PIN_SYNC_OUT_TIP);
    gpio_bit_reset(BSP_PIN_SYNC_OUT_RING);

    rcu_periph_clock_enable(RCU_SYSCFG);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN9);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN10);
    exti_init(EXTI_9, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_init(EXTI_10, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_interrupt_flag_clear(EXTI_9);
    exti_interrupt_flag_clear(EXTI_10);
    nvic_irq_enable((uint8_t)EXTI5_9_IRQn, BSP_IRQ_PRIO_SYNC_IN, 0U);
    nvic_irq_enable((uint8_t)EXTI10_15_IRQn, BSP_IRQ_PRIO_SYNC_IN, 0U);
}

/* Level check, then toggle on the channel's period */
static void ChannelTick(Channel_t *c, uint32_t now_ms)
{
    if ((now_ms % c->half_period_ms) != 0U)
    {
        return;
    }
    if (((gpio_input_bit_get(c->in_port, c->in_pin) != RESET) ? 1U : 0U) == c->level)
    {
        c->acc.same++;
    }
    else
    {
        c->acc.inverted++;
    }
    c->level ^= 1U;
    if (c->level != 0U)
    {
        gpio_bit_set(c->out_port, c->out_pin);
    }
    else
    {
        gpio_bit_reset(c->out_port, c->out_pin);
    }
    c->acc.out_edges++;
}

static void ChannelResult(Channel_t *c, volatile uint32_t *in_edges, SyncLoopChannel_t *r)
{
    *r = c->acc;
    __disable_irq();
    r->in_edges = *in_edges;
    *in_edges = 0U;
    __enable_irq();
    r->ok = ((r->out_edges != 0U) && (r->in_edges == r->out_edges) &&
             ((r->same == 0U) || (r->inverted == 0U))) ? 1U : 0U;
    c->acc.out_edges = 0U;
    c->acc.same = 0U;
    c->acc.inverted = 0U;
}

int SyncLoop_Poll(uint32_t now_ms, SyncLoopResult_t *res)
{
    int ready = 0;

    if (now_ms == s_last_ms)
    {
        return 0;
    }
    s_last_ms = now_ms;

    /* result before this ms' toggles: every edge of the window has arrived (>= 1 ms ago) */
    if ((now_ms - s_window_start_ms) >= WINDOW_MS)
    {
        s_window_start_ms = now_ms;
        ChannelResult(&s_tip, &s_tip_in_edges, &res->tip);
        ChannelResult(&s_ring, &s_ring_in_edges, &res->ring);
        ready = 1;
    }

    ChannelTick(&s_tip, now_ms);
    ChannelTick(&s_ring, now_ms);
    return ready;
}
