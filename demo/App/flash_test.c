/*
 * flash_test.c - external flash self test, see flash_test.h
 */
#include "flash_test.h"
#include "bsp_flash.h"

#define TEST_SECTOR     127U
#define TEST_ADDR       (TEST_SECTOR * BSP_FLASH_SECTOR_SIZE)
#define TEST_PAGES      (BSP_FLASH_SECTOR_SIZE / BSP_FLASH_PAGE_SIZE)
#define FLASH_ID_W25X40CL   0xEF12U

typedef enum
{
    T_IDLE = 0,
    T_ERASE,        /* erase requested, waiting */
    T_BLANK,        /* read after erase */
    T_PROGRAM,      /* page s_page programming */
    T_VERIFY,       /* read back */
    T_END
} TestState_t;

typedef struct
{
    uint16_t id;
    uint8_t  ok;            /* 1 = passed */
    uint8_t  step_failed;   /* TestState_t of the failure, 0 = none */
    uint32_t bad_offset;    /* first mismatch */
    uint32_t erase_ms, blank_ms, program_ms, verify_ms;
} FlashTestResult_t;

static volatile TestState_t s_t;
static volatile uint8_t s_step_done;
static volatile int s_step_result;
static uint32_t s_page, s_t0, s_prog_t0;
static uint8_t s_buf[BSP_FLASH_SECTOR_SIZE];
FlashTestResult_t g_flash_test;     /* test result */

static uint8_t Pattern(uint32_t i)
{
    return (uint8_t)((i * 7U) ^ (i >> 8) ^ 0x5AU);
}

static void StepDone(int result)
{
    s_step_result = result;
    s_step_done = 1U;
}

static void Fail(TestState_t step, uint32_t offset)
{
    g_flash_test.ok = 0U;
    g_flash_test.step_failed = (uint8_t)step;
    g_flash_test.bad_offset = offset;
    s_t = T_END;
}

static int Request(TestState_t next, int rc, uint32_t now_ms)
{
    if (rc != 0)
    {
        Fail(next, 0U);
        return -1;
    }
    s_step_done = 0U;
    s_t = next;
    s_t0 = now_ms;
    return 0;
}

void FlashTest_Start(uint32_t now_ms)
{
    g_flash_test.id = BspFlash_ReadId();
    if (g_flash_test.id != FLASH_ID_W25X40CL)
    {
        Fail(T_IDLE, 0U);
        return;
    }
    (void)Request(T_ERASE, BspFlash_EraseSector(TEST_ADDR, StepDone), now_ms);
}

void FlashTest_Poll(uint32_t now_ms)
{
    uint32_t i;

    BspFlash_Poll();
    if ((s_t == T_IDLE) || (s_t == T_END) || (s_step_done == 0U))
    {
        return;
    }
    if (s_step_result != 0)
    {
        Fail(s_t, 0U);
        return;
    }

    switch (s_t)
    {
    case T_ERASE:
        g_flash_test.erase_ms = now_ms - s_t0;
        (void)Request(T_BLANK, BspFlash_Read(TEST_ADDR, s_buf, sizeof(s_buf), StepDone), now_ms);
        break;

    case T_BLANK:
        g_flash_test.blank_ms = now_ms - s_t0;
        for (i = 0U; i < sizeof(s_buf); i++)
        {
            if (s_buf[i] != 0xFFU)
            {
                Fail(T_BLANK, i);
                return;
            }
        }
        for (i = 0U; i < sizeof(s_buf); i++)
        {
            s_buf[i] = Pattern(i);
        }
        s_page = 0U;
        s_prog_t0 = now_ms;
        (void)Request(T_PROGRAM, BspFlash_Program(TEST_ADDR, s_buf, BSP_FLASH_PAGE_SIZE, StepDone), now_ms);
        break;

    case T_PROGRAM:
        if (++s_page < TEST_PAGES)
        {
            const uint32_t off = s_page * BSP_FLASH_PAGE_SIZE;

            (void)Request(T_PROGRAM, BspFlash_Program(TEST_ADDR + off, &s_buf[off], BSP_FLASH_PAGE_SIZE, StepDone), now_ms);
            break;
        }
        g_flash_test.program_ms = now_ms - s_prog_t0;
        for (i = 0U; i < sizeof(s_buf); i++)
        {
            s_buf[i] = 0U;      /* the read back must really fill it */
        }
        (void)Request(T_VERIFY, BspFlash_Read(TEST_ADDR, s_buf, sizeof(s_buf), StepDone), now_ms);
        break;

    case T_VERIFY:
        g_flash_test.verify_ms = now_ms - s_t0;
        for (i = 0U; i < sizeof(s_buf); i++)
        {
            if (s_buf[i] != Pattern(i))
            {
                Fail(T_VERIFY, i);
                return;
            }
        }
        g_flash_test.ok = 1U;
        s_t = T_END;
        break;

    default:
        break;
    }
}
