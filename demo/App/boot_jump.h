/*
 * boot_jump.h - start the BCR32 bootloader from the application (firmware update)
 * Sequence: USB off, 200 ms, all interrupts / DMA / SysTick off, then the entry of the bootloader's
 * software information record (pointer at 0x0800FFFC). The bootloader comes up in its
 * Booting state over USB (PID 12CE), ready for update.exe.
 */
#ifndef BOOT_JUMP_H
#define BOOT_JUMP_H

/* Does not return; returns only if the bootloader record is not valid */
void BootJump_Start(void);

#endif /* BOOT_JUMP_H */
