/*
 * bsp_iflash.c - internal flash erase / program, see bsp_iflash.h
 */
#include "bsp_iflash.h"
#include "gd32f4xx.h"

#define WORDS_PER_POLL  32U
#define FMC_ERR_FLAGS   (FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR)

/* end of this image in flash */
#if defined(__CC_ARM)
extern const uint32_t Load$$LR$$LR_APPL$$Limit;     /* armlink, Board/bcr32_demo.sct */
#define IMAGE_END   ((uint32_t)&Load$$LR$$LR_APPL$$Limit)
#else
extern const uint32_t __image_end__;                /* GCC, Project/gcc/bcr32_demo.ld */
#define IMAGE_END   ((uint32_t)&__image_end__)
#endif

typedef enum
{
    OP_NONE = 0,
    OP_ERASE,
    OP_PROGRAM
} Op_t;

static Op_t s_op;
static int s_result;
static uint32_t s_addr, s_left;
static const uint32_t *s_words;

static void finish(int result)
{
    fmc_lock();
    s_result = result;
    s_op = OP_NONE;
}

int BspIFlash_EraseStart(void)
{
    if ((s_op != OP_NONE) || (IMAGE_END > BSP_IFLASH_FREE_ADDR))
    {
        return -1;
    }
    fmc_unlock();
    fmc_flag_clear(FMC_FLAG_END | FMC_ERR_FLAGS);
    fmc_sector_erase_split_1(CTL_SECTOR_NUMBER_5);
    s_op = OP_ERASE;
    return 0;
}

int BspIFlash_ProgramStart(uint32_t addr, const uint32_t *words, uint32_t nwords)
{
    if ((s_op != OP_NONE) || (nwords == 0U) || ((addr & 3U) != 0U) || (addr < BSP_IFLASH_FREE_ADDR) ||
        ((addr + (nwords * 4U)) > (BSP_IFLASH_FREE_ADDR + BSP_IFLASH_FREE_SIZE)))
    {
        return -1;
    }
    fmc_unlock();
    fmc_flag_clear(FMC_FLAG_END | FMC_ERR_FLAGS);
    s_addr = addr;
    s_words = words;
    s_left = nwords;
    s_op = OP_PROGRAM;
    return 0;
}

uint8_t BspIFlash_Busy(void)
{
    return (s_op != OP_NONE) ? 1U : 0U;
}

int BspIFlash_Result(void)
{
    return s_result;
}

void BspIFlash_Poll(void)
{
    uint32_t n;

    if (s_op == OP_ERASE)
    {
        const fmc_state_enum st = fmc_state_get();

        if (st == FMC_BUSY)
        {
            return;
        }
        fmc_sector_erase_split_2();
        finish((st == FMC_READY) ? 0 : -(int)st);
    }
    else if (s_op == OP_PROGRAM)
    {
        for (n = 0U; (n < WORDS_PER_POLL) && (s_left > 0U); n++)
        {
            const fmc_state_enum st = fmc_word_program(s_addr, *s_words);

            if (st != FMC_READY)
            {
                finish(-(int)st);
                return;
            }
            s_addr += 4U;
            s_words++;
            s_left--;
        }
        if (s_left == 0U)
        {
            finish(0);
        }
    }
}
