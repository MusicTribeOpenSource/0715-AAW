/*
 * bsp_cv.h - CV outputs (one DAC on SPI2 + 4 channel sample/hold mux) and gate outputs
 * The mux holds each channel on a capacitor: the application must refresh all four
 * channels periodically (cv_out.c: one channel per ms). Not thread safe.
 */
#ifndef BSP_CV_H
#define BSP_CV_H

#include <stdint.h>

#define BSP_CV_NUM_CHANNELS     4U

typedef enum
{
    BSP_GATE_0V = 0,
    BSP_GATE_5V,
    BSP_GATE_10V
} BspGateLevel_t;

/* SPI2, DAC gain, all channels 0, channel 0 selected, mux disabled; all gates 0 V */
void BspCv_Init(void);

/* One complete channel update in 5 steps:
 * mux off, select channel, CS low, data register write, CS high, mux on */
void BspCv_Write(uint32_t channel, uint16_t value);

/* gate 0..3 */
void BspGate_Set(uint32_t gate, BspGateLevel_t level);

#endif /* BSP_CV_H */
