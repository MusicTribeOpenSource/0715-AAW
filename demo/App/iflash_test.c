/*
 * iflash_test.c - internal flash self test, see iflash_test.h
 */
#include "iflash_test.h"
#include "bsp_iflash.h"
#include "gd32f4xx.h"

#define TEST_WORDS      1024U   /* 4 KB */
#define CYCLES_PER_MS   (SystemCoreClock / 1000U)

typedef enum
{
    T_IDLE = 0,
    T_ERASE,
    T_PROGRAM,
    T_END
} TestState_t;

typedef struct
{
    uint8_t  ok;
    uint8_t  step_failed;       /* TestState_t, 0 = none */
    int16_t  fmc_result;
    uint32_t bad_offset;
    uint32_t erase_ms;          /* SysTick: stops counting if the CPU stalls */
    uint32_t erase_cycles_ms;   /* DWT cycles: always counts */
    uint32_t program_ms;
} IFlashTestResult_t;

static TestState_t s_t;
static uint32_t s_t0, s_cyc0;
static uint32_t s_words[TEST_WORDS];
IFlashTestResult_t g_iflash_test;   /* test result */

static uint32_t Pattern(uint32_t i)
{
    return (i * 0x9E3779B9U) ^ 0xA5A5A5A5U;
}

static void Fail(TestState_t step, uint32_t offset)
{
    g_iflash_test.ok = 0U;
    g_iflash_test.step_failed = (uint8_t)step;
    g_iflash_test.fmc_result = (int16_t)BspIFlash_Result();
    g_iflash_test.bad_offset = offset;
    s_t = T_END;
}

void IFlashTest_Start(uint32_t now_ms)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;     /* DWT cycle counter on */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    s_t0 = now_ms;
    s_cyc0 = DWT->CYCCNT;
    if (BspIFlash_EraseStart() != 0)
    {
        Fail(T_ERASE, 0U);
        return;
    }
    s_t = T_ERASE;
}

void IFlashTest_Poll(uint32_t now_ms)
{
    const volatile uint32_t *flash = (const volatile uint32_t *)BSP_IFLASH_FREE_ADDR;
    uint32_t i;

    BspIFlash_Poll();
    if ((s_t == T_IDLE) || (s_t == T_END) || (BspIFlash_Busy() != 0U))
    {
        return;
    }
    if (BspIFlash_Result() != 0)
    {
        Fail(s_t, 0U);
        return;
    }

    switch (s_t)
    {
    case T_ERASE:
        g_iflash_test.erase_ms = now_ms - s_t0;
        g_iflash_test.erase_cycles_ms = (DWT->CYCCNT - s_cyc0) / CYCLES_PER_MS;
        for (i = 0U; i < (BSP_IFLASH_FREE_SIZE / 4U); i++)
        {
            if (flash[i] != 0xFFFFFFFFU)
            {
                Fail(T_ERASE, i * 4U);
                return;
            }
        }
        for (i = 0U; i < TEST_WORDS; i++)
        {
            s_words[i] = Pattern(i);
        }
        s_t0 = now_ms;
        if (BspIFlash_ProgramStart(BSP_IFLASH_FREE_ADDR, s_words, TEST_WORDS) != 0)
        {
            Fail(T_PROGRAM, 0U);
            return;
        }
        s_t = T_PROGRAM;
        break;

    case T_PROGRAM:
        g_iflash_test.program_ms = now_ms - s_t0;
        for (i = 0U; i < TEST_WORDS; i++)
        {
            if (flash[i] != Pattern(i))
            {
                Fail(T_PROGRAM, i * 4U);
                return;
            }
        }
        g_iflash_test.ok = 1U;
        s_t = T_END;
        break;

    default:
        break;
    }
}
