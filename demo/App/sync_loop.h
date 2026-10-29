/*
 * sync_loop.h - SYNC OUT -> SYNC IN loopback test (cable tip-tip, ring-ring)
 *
 * SYNC OUT tip (PA0) toggles every 5 ms, ring (PA1) every 10 ms, from the 1 ms tick.
 * SYNC IN tip (PA9, EXTI9) and ring (PA10, EXTI10) count both edges in interrupts.
 * Once per second the edge counts are compared with the toggles; the different
 * rates show crossed or shorted lines. Before each toggle the input level is
 * compared with the output level to find the polarity of the input path.
 */
#ifndef SYNC_LOOP_H
#define SYNC_LOOP_H

#include <stdint.h>

typedef struct
{
    uint32_t out_edges;     /* toggles of the output in the window */
    uint32_t in_edges;      /* edges counted on the input in the window */
    uint32_t same;          /* level samples input == output */
    uint32_t inverted;      /* level samples input != output */
    uint8_t  ok;            /* in_edges == out_edges != 0, one polarity only */
} SyncLoopChannel_t;

typedef struct
{
    SyncLoopChannel_t tip;
    SyncLoopChannel_t ring;
} SyncLoopResult_t;

/* EXTI9/EXTI10 on PA9/PA10, both edges; outputs low. After BspGpio_Init. */
void SyncLoop_Init(void);

/* Call from the main loop; returns 1 when a new 1 s result is in *res */
int SyncLoop_Poll(uint32_t now_ms, SyncLoopResult_t *res);

#endif /* SYNC_LOOP_H */
