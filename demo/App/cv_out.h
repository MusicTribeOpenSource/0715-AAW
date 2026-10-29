/*
 * cv_out.h - CV1..4 outputs set from the knob encoders 1..4
 *
 * 0.0 .. 10.0 V in 0.1 V steps, one step per detent. Nominal scale of the
 * BCR32 factory defaults: 0 V = DAC 0, 10 V = DAC 65535; the per unit factory
 * calibration (settings in flash) is not applied. The sample/hold mux needs a
 * refresh: one channel per ms (CvOut_Poll), each channel every 4 ms.
 */
#ifndef CV_OUT_H
#define CV_OUT_H

#include <stdint.h>

#define CV_OUT_NUM          4U
#define CV_OUT_MAX_DV       100U    /* 10.0 V in 0.1 V units */

/* DAC, mux, gates (0 V); all CV 0 V. After BspGpio_Init, SysTick running. */
void CvOut_Init(void);

/* Call from the main loop; refreshes one channel when a new ms started (~150 us) */
void CvOut_Poll(uint32_t now_ms);

/* channel 0..3: change by delta * 0.1 V, clamped; returns the new value in 0.1 V */
uint32_t CvOut_Add(uint32_t channel, int32_t delta);

#endif /* CV_OUT_H */
