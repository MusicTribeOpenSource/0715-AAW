/*
 * boot_jump.c - start the bootloader, see boot_jump.h
 */
#include "boot_jump.h"
#include "gd32f4xx.h"
#include "gd32f4xx_dma.h"   /* not in gd32f4xx_libopt.h */
#include "bsp_delay.h"
#include "usbd_desc.h"

#define BOOT_SW_INFO_ROOT       0x0800FFFCU     /* last word of the 64 KB bootloader region */
#define BOOT_REGION_START       0x08000000U
#define BOOT_REGION_END         0x08010000U
#define FLASH_AREA_VALID        0x0B0C3204U
#define USB_OFF_WAIT_MS         200U            /* the host sees the disconnect */

typedef struct
{
    void (*entry)(void);
    uint32_t flash_state;
} BootSwInfo_t;

void BootJump_Start(void)
{
    const BootSwInfo_t *info = *(const BootSwInfo_t *const *)BOOT_SW_INFO_ROOT;
    uint32_t i;

    if (((uint32_t)info < BOOT_REGION_START) || ((uint32_t)info >= BOOT_REGION_END) ||
        (info->flash_state != FLASH_AREA_VALID))
    {
        return;
    }

    DrvUsbDeInit();
    for (i = 0U; i < USB_OFF_WAIT_MS; i++)
    {
        BspDelay_Us(1000U);
    }

    /* no DMA may go on writing into the bootloader's RAM: SPI1 (OLED / flash), MIDI TX */
    dma_channel_disable(DMA0, DMA_CH3);
    dma_channel_disable(DMA0, DMA_CH4);
    dma_channel_disable(DMA0, DMA_CH6);
    dma_channel_disable(DMA1, DMA_CH6);

    __disable_irq();
    for (i = 0U; i < 3U; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFFU;
        NVIC->ICPR[i] = 0xFFFFFFFFU;
    }
    SysTick->CTRL = 0U;
    SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;

    info->entry();
}
