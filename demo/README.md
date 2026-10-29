# BCR32 bare-metal demo

A bare-metal (no RTOS) firmware for the Behringer BCR32 hardware. It exercises every on-board
peripheral with a small, self-contained example and serves as a starting point for new
firmware on the same device.

## Hardware

| Item | Value |
| --- | --- |
| MCU | GigaDevice GD32F405RE (Cortex-M4F), 512 KB flash, 128 KB SRAM + 64 KB TCM SRAM (TCM unused) |
| Clock | 8 MHz crystal, PLL to 120 MHz (AHB 120 MHz, APB2 60 MHz, APB1 30 MHz) |
| Programming | USB firmware update (see below) |

## Memory map

The image is linked to `0x08010000` and is started by the BCR32 bootloader. The bootloader is
never written.

| Region | Address | Size | Use |
| --- | --- | --- | --- |
| Internal flash, sectors 0–3 | `0x08000000` | 64 KB | Bootloader (untouched) |
| Internal flash, sector 4 | `0x08010000` | 64 KB | This demo (about 34 KB) |
| Internal flash, sector 5 | `0x08020000` | 128 KB | Free; used by the internal flash self test |
| Internal flash, sectors 6–7 | `0x08040000` | 256 KB | Reserved: device presets / settings (untouched) |
| External SPI flash, sectors 0–79 | `0x000000` | 320 KB | Reserved: device data incl. factory calibration (write protected by the driver) |
| External SPI flash, sectors 80–127 | `0x050000` | 192 KB | Free; sector 127 used by the external flash self test |

The first 20 bytes of the image hold the software information block the bootloader checks
(`Board/startup_bcr32.s`, scatter file `Board/bcr32_demo.sct`).

## What the demo does

After power-up:

1. The OLED shows **BCR32** centered.
2. One LED at a time walks once over every HT16K33 output, then all LEDs stay off.
3. Self tests of the external SPI flash and of the internal flash run in the background;
   results are kept in RAM (`g_flash_test`, `g_iflash_test`).
4. Everything below runs from then on, driven by a 1 ms SysTick main loop.

| Feature | Behaviour | Files |
| --- | --- | --- |
| Buttons | Event driven; pressing a button shows `BTNn` (1–64) on the OLED | `App/input.c`, `Board/bsp_buttons.c` |
| Encoders | 32 knobs + SELECT encoder; shows `ENCODER +x` / `-x` (detents per 20 ms) and `KNOB n` / `SELECT` | `App/input.c` |
| CV outputs | Knobs 1–4 set CV1–4, 0.0–10.0 V in 0.1 V steps; `CVx:x.xV` bottom right | `App/cv_out.c`, `Board/bsp_cv.c` |
| Gate outputs | BTN1–3 / 5–7 / 9–11 / 13–15 set GATE1 / 2 / 3 / 4 to 0 / 5 / 10 V; `GATEx:xV` bottom right | `App/main.c`, `Board/bsp_cv.c` |
| SYNC | Loopback test with a cable SYNC OUT → SYNC IN (tip 100 Hz, ring 50 Hz); result shown while SELECT is held | `App/sync_loop.c` |
| DIN MIDI | Every byte from MIDI IN is forwarded unchanged to MIDI OUT A and B | `App/midi_thru.c`, `Board/bsp_midi_uart.c` |
| USB MIDI | Class-compliant device with two ports ("BCR-32 CTRL", "BCR-32 ZAQ"); received packets are echoed back | `UsbDriver/`, `Board/bsp_usb.c` |
| Activity LEDs | MIDI IN, OUT A, OUT B and USB LEDs flash on traffic | `App/activity_led.c` |
| OLED | SSD1306 128×32 on SPI1, whole frame by DMA, non-blocking double buffer | `ThirdParty/ssd1306/`, `Board/bsp_spi1.c` |
| External flash | W25X40CL on SPI1 (shared with the OLED), asynchronous DMA read / program / erase | `Board/bsp_flash.c`, `App/flash_test.c` |
| Internal flash | Asynchronous erase / program of sector 5 | `Board/bsp_iflash.c`, `App/iflash_test.c` |
| USB SysEx | `F0 00 20 32 7F 64 74 F7` (mode query) is answered with `F0 00 20 32 00 64 74 00 F7` (application running) on the port it came from; `F0 00 20 32 7F 64 77 F7` starts the bootloader | `App/sysex.c` |
| Bootloader entry | SysEx `0x77`, or hold **T1 + T2 + T3 + T4**, starts the bootloader for a firmware update | `App/boot_jump.c` |

### Controls

| Control | Action |
| --- | --- |
| Any button | OLED shows `BTNn` |
| BTN1–3, 5–7, 9–11, 13–15 | GATE1–4 to 0 V / 5 V / 10 V (first, second, third button of each group of four) |
| SELECT (BTN24), held | OLED shows `SYNC OK` / `SYNC FAIL` and the edge counts; blank on release |
| Knobs 1–4 | CV1–4 ± 0.1 V per detent |
| Other knobs, SELECT encoder | OLED shows the detents of the turn |
| T1 + T2 + T3 + T4 (BTN18–21), held | Start the bootloader |

## Project layout

```
demo/
├── App/          demo application (main loop, input events, CV, SYNC, MIDI thru, tests, ...)
├── Board/        startup, scatter file, board support (GPIO, LEDs, buttons, SPI1, flash, CV, MIDI UART, USB)
├── Drivers/      CMSIS and GigaDevice GD32F4xx standard peripheral library
├── ThirdParty/   SSD1306 OLED driver
├── UsbDriver/    GigaDevice USBFS device stack and the USB MIDI class
├── Project/      keil/: Keil MDK project (bcr32_demo.uvprojx); gcc/: GCC build (CMake)
└── tools/
    └── update/       After Build script (generate_BCR.bat) and the update binaries
        └── windows/  update.exe, firmware update over USB MIDI
```

## Building

Requirements: Keil MDK 5 with ARM Compiler 5.06 (ARMCC) and the GigaDevice GD32F4xx DFP 3.0.3.

1. Open `Project/keil/bcr32_demo.uvprojx`, target `BCR32_Demo`.
2. Build. Outputs in `Project/keil/Objects/`: `bcr32_demo.axf` and `bcr32_demo.hex`.
3. The After Build steps then create the update binary
   `tools/update/BCR32_FW_V<x.y.z>_<date>_<time>.bin`:
   `fromelf` writes `Objects/bcr32_demo.bin`, `tools/update/generate_BCR.bat` copies it.
   The version comes from `App/version.h`. Images of earlier builds are
   kept (each name has its own date and time); delete them by hand when no longer needed.

Command line build: `UV4 -b Project\keil\bcr32_demo.uvprojx -t BCR32_Demo`.

### GCC (CMake)

`Project/gcc/` holds the GCC build: `CMakeLists.txt`, the toolchain file, the GNU startup file
`startup_bcr32_gcc.S` and the linker script `bcr32_demo.ld` (same memory layout as the Keil
startup file and scatter file; keep both sets in step). Requirements: CMake 3.20+, Ninja (or
make) and the GNU Arm Embedded toolchain on the `PATH`.

```
cmake -S Project/gcc -B build -G Ninja
cmake --build build
```

Outputs in `build/`: `bcr32_demo.elf`, `.hex`, `.bin`, `.map`. On Windows the build also
copies the update binary to `tools/update/` like the Keil build
(`-DBCR32_UPDATE_IMAGE=OFF` to skip it).

## Firmware update over USB (no debugger)

1. Connect the BCR32 over USB and close any program that holds its MIDI ports.
2. Run `tools\update\windows\update.exe tools\update\BCR32_FW_V<x.y.z>_<date>_<time>.bin`.

The tool makes the running demo start the bootloader over USB, transfers the image and restarts
the device; no key needs to be pressed. If an update is interrupted the device stays in the
bootloader; run the tool again. Windows only, one BCR32 connected at a time.

## Interrupt priorities

Group `NVIC_PRIGROUP_PRE4_SUB0` (16 levels, lower = more urgent).

| Level | Source |
| --- | --- |
| 2 | EXTI9 / EXTI10 (SYNC IN tip / ring) |
| 3 | USART1 RX (MIDI IN) |
| 6 | USBFS |
| 7 | DMA0 channel 3 / 4 (SPI1 RX / TX: OLED, external flash) |
| 15 | SysTick (1 ms tick) |

## Notes and limitations

- **CV calibration:** the nominal scale (0 V = DAC 0, 10 V = DAC 65535) is used; the per-unit
  factory calibration in external flash sector 50 is not read, so outputs may differ from the
  display by a few tens of millivolts.
- **Self tests write flash at every power-up:** external flash sector 127 and internal flash
  sector 5 are erased and programmed once per boot. Fine for a demo; trigger them on demand for
  long-term use.
- **Shared SPI1 bus:** the OLED and the external flash share SPI1 (7.5 MHz, within the SSD1306
  limit). Only one transfer runs at a time; a large flash read delays an OLED update.
- **Erase timing:** the internal flash erase may briefly stall the CPU; the self test reports
  SysTick and cycle-counter times for the erase.
- **MIDI Active Sensing** (`FE`) from a connected device is forwarded like any other byte and
  makes the activity LEDs blink.

## Licenses

Demo code: AGPL-3.0-or-later. Third-party components keep their own licenses: GigaDevice  
GD32F4xx standard peripheral and USB libraries (BSD-3-Clause), Arm CMSIS (Apache-2.0).
