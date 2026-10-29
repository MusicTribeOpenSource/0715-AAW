/*
 * cv_out.c - CV1..4 outputs, see cv_out.h
 */
#include "cv_out.h"
#include "bsp_cv.h"

#define DAC_FULL_SCALE      65535U

static uint32_t s_dv[CV_OUT_NUM];       /* value in 0.1 V */
static uint32_t s_next;                 /* channel refreshed next */
static uint32_t s_last_ms;

static uint16_t DvToDac(uint32_t dv)
{
    return (uint16_t)((dv * DAC_FULL_SCALE) / CV_OUT_MAX_DV);
}

void CvOut_Init(void)
{
    BspCv_Init();
}

void CvOut_Poll(uint32_t now_ms)
{
    if (now_ms == s_last_ms)
    {
        return;
    }
    s_last_ms = now_ms;
    BspCv_Write(s_next, DvToDac(s_dv[s_next]));
    s_next = (s_next + 1U) % CV_OUT_NUM;
}

uint32_t CvOut_Add(uint32_t channel, int32_t delta)
{
    int32_t v;

    if (channel >= CV_OUT_NUM)
    {
        return 0U;
    }
    v = (int32_t)s_dv[channel] + delta;
    if (v < 0)
    {
        v = 0;
    }
    else if (v > (int32_t)CV_OUT_MAX_DV)
    {
        v = (int32_t)CV_OUT_MAX_DV;
    }
    s_dv[channel] = (uint32_t)v;
    return s_dv[channel];
}
