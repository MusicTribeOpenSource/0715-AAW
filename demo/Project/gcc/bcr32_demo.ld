/*
 * bcr32_demo.ld - BCR32 bare-metal demo, GCC linker script
 * Same layout as Board/bcr32_demo.sct (Keil / ARMCC): keep both in step when one changes.
 *
 *   0x08000000  64 KB  Bootloader (kept, never written by this project)
 *   0x08010000 192 KB  Application  <- this image
 *   0x08040000 256 KB  Reserved: device presets / settings (not used here)
 * The bootloader reads the pointer at 0x08010000 (SW info root), checks the
 * SW info block it points to and jumps to its fnSoftwareEntry (Reset_Handler).
 */
ENTRY(Reset_Handler)

MEMORY
{
    APPL (rx)  : ORIGIN = 0x08010000, LENGTH = 192K
    RAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 128K     /* TCM SRAM 0x10000000 not used */
}

_estack     = ORIGIN(RAM) + LENGTH(RAM);                /* MSP: main() + all ISRs */
_stack_size = 0x800;

SECTIONS
{
    .swinfo_root ORIGIN(APPL) :                         /* 0x08010000: pointer to SwInfo */
    {
        KEEP(*(.swinfo_root))
    } > APPL

    .swinfo ORIGIN(APPL) + 4 :                          /* 0x08010004: SW info block */
    {
        KEEP(*(.swinfo))
    } > APPL

    .isr_vector :                                       /* 512 byte aligned for VTOR */
    {
        . = ALIGN(512);
        KEEP(*(.isr_vector))
    } > APPL

    .text :
    {
        *(.text*)
        *(.rodata*)
        KEEP(*(.init))
        KEEP(*(.fini))
        . = ALIGN(4);
    } > APPL

    .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } > APPL
    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > APPL

    .preinit_array : { PROVIDE_HIDDEN(__preinit_array_start = .); KEEP(*(.preinit_array*)); PROVIDE_HIDDEN(__preinit_array_end = .); } > APPL
    .init_array    : { PROVIDE_HIDDEN(__init_array_start = .); KEEP(*(SORT(.init_array.*))); KEEP(*(.init_array*)); PROVIDE_HIDDEN(__init_array_end = .); } > APPL
    .fini_array    : { PROVIDE_HIDDEN(__fini_array_start = .); KEEP(*(SORT(.fini_array.*))); KEEP(*(.fini_array*)); PROVIDE_HIDDEN(__fini_array_end = .); } > APPL

    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > APPL

    /* end of the image in flash, the Keil equivalent is Load$$LR$$LR_APPL$$Limit */
    __image_end__ = LOADADDR(.data) + SIZEOF(.data);

    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM

    /* heap for the C library (newlib-nano _sbrk), up to the stack */
    end = _ebss;
    PROVIDE(_end = _ebss);

    .stack_check (NOLOAD) :
    {
        . = ALIGN(8);
        . = . + _stack_size;                            /* link error if the stack does not fit */
    } > RAM
}
