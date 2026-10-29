/*
 * bsp_flash.c - W25X40CL SPI NOR flash, see bsp_flash.h
 */
#include "bsp_flash.h"
#include "bsp_gpio.h"
#include "bsp_spi1.h"

/* W25X40CL commands (datasheet) */
#define CMD_WREN    0x06U   /* write enable */
#define CMD_RDSR    0x05U   /* read status register */
#define CMD_READ    0x03U   /* read data */
#define CMD_PP      0x02U   /* page program */
#define CMD_SE      0x20U   /* sector erase, 4 KB */
#define CMD_RDID    0x90U   /* manufacturer / device ID */
#define SR_BUSY     0x01U

typedef enum
{
    OP_READ = 0,
    OP_PROGRAM,
    OP_ERASE
} Op_t;

typedef enum
{
    ST_IDLE = 0,
    ST_QUEUED,      /* waits for the bus */
    ST_XFER,        /* DMA data phase running */
    ST_WIP,         /* program / erase running in the chip */
    ST_DONE         /* done() pending */
} State_t;

static volatile State_t s_state;
static Op_t s_op;
static uint32_t s_addr, s_len;
static uint8_t *s_buf;
static BspFlashDone_t s_done;

static void cs_low(void)  { gpio_bit_reset(BSP_PIN_FLASH_CS); }
static void cs_high(void) { gpio_bit_set(BSP_PIN_FLASH_CS); }

/* Command with a 24 bit address, CS stays low for the data phase */
static void cmd_addr(uint8_t cmd, uint32_t addr)
{
    const uint8_t b[4] = { cmd, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)addr };

    cs_low();
    BspSpi1_Write(b, sizeof(b));
    BspSpi1_WaitIdle();
}

static void write_enable(void)
{
    const uint8_t c = CMD_WREN;

    cs_low();
    BspSpi1_Write(&c, 1U);
    BspSpi1_WaitIdle();
    cs_high();
}

static uint8_t read_status(void)
{
    uint8_t sr;

    cs_low();
    (void)BspSpi1_Transfer(CMD_RDSR);
    sr = BspSpi1_Transfer(0xFFU);
    cs_high();
    return sr;
}

/* DMA data phase finished (interrupt): CS up; program waits for the chip, read is done */
static void xfer_done(void)
{
    cs_high();
    s_state = (s_op == OP_READ) ? ST_DONE : ST_WIP;
}

uint16_t BspFlash_ReadId(void)
{
    uint16_t id;

    while (BspSpi1_DmaBusy() != 0U)
    {
    }
    cmd_addr(CMD_RDID, 0U);
    id = (uint16_t)((uint16_t)BspSpi1_Transfer(0xFFU) << 8);
    id |= BspSpi1_Transfer(0xFFU);
    cs_high();
    return id;
}

static int queue(Op_t op, uint32_t addr, uint8_t *buf, uint32_t len, BspFlashDone_t done)
{
    if (s_state != ST_IDLE)
    {
        return -1;
    }
    s_op = op;
    s_addr = addr;
    s_buf = buf;
    s_len = len;
    s_done = done;
    s_state = ST_QUEUED;
    return 0;
}

static int writable(uint32_t addr)
{
    return ((addr / BSP_FLASH_SECTOR_SIZE) >= BSP_FLASH_FIRST_FREE_SECTOR) && (addr < BSP_FLASH_SIZE);
}

int BspFlash_Read(uint32_t addr, uint8_t *dst, uint32_t len, BspFlashDone_t done)
{
    if ((len == 0U) || (len > 0xFFFFU) || ((addr + len) > BSP_FLASH_SIZE))
    {
        return -1;
    }
    return queue(OP_READ, addr, dst, len, done);
}

int BspFlash_Program(uint32_t addr, const uint8_t *src, uint32_t len, BspFlashDone_t done)
{
    if ((len == 0U) || (writable(addr) == 0) ||
        ((addr % BSP_FLASH_PAGE_SIZE) + len > BSP_FLASH_PAGE_SIZE))
    {
        return -1;
    }
    return queue(OP_PROGRAM, addr, (uint8_t *)src, len, done);
}

int BspFlash_EraseSector(uint32_t addr, BspFlashDone_t done)
{
    if (writable(addr) == 0)
    {
        return -1;
    }
    return queue(OP_ERASE, addr - (addr % BSP_FLASH_SECTOR_SIZE), 0, 0U, done);
}

uint8_t BspFlash_Busy(void)
{
    return (s_state != ST_IDLE) ? 1U : 0U;
}

void BspFlash_Poll(void)
{
    switch (s_state)
    {
    case ST_QUEUED:
        if (BspSpi1_DmaBusy() != 0U)
        {
            break;      /* OLED frame on the bus */
        }
        if (s_op == OP_READ)
        {
            s_state = ST_XFER;
            cmd_addr(CMD_READ, s_addr);
            BspSpi1_ReadDma(s_buf, s_len, xfer_done);
        }
        else if (s_op == OP_PROGRAM)
        {
            s_state = ST_XFER;
            write_enable();
            cmd_addr(CMD_PP, s_addr);
            BspSpi1_WriteDma(s_buf, s_len, xfer_done);
        }
        else
        {
            write_enable();
            cmd_addr(CMD_SE, s_addr);
            cs_high();
            s_state = ST_WIP;
        }
        break;

    case ST_WIP:
        if ((BspSpi1_DmaBusy() == 0U) && ((read_status() & SR_BUSY) == 0U))
        {
            s_state = ST_DONE;
        }
        break;

    case ST_DONE:
        s_state = ST_IDLE;
        if (s_done != 0)
        {
            s_done(0);
        }
        break;

    case ST_IDLE:
    case ST_XFER:
    default:
        break;
    }
}
