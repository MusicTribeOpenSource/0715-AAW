# BCR32 MCU Interface Definition

| Item | Value |
| --- | --- |
| MCU | GigaDevice GD32F405RE (Arm Cortex-M4F) |
| Package | LQFP64 |
| Memory | 512 KB flash, 128 KB SRAM + 64 KB TCM SRAM |
| Crystal | 8 MHz (HXTAL) |

Pin numbers follow the GD32F405Rx LQFP64 package (pin-compatible with STM32F405Rx).

---

## 1. Pin Assignment

Direction: **I** input, **O** output, **I/O** bidirectional. AF = alternate function number.

### 1.1 Port A

| Pin | Port | Signal | Dir | MCU function (AF) | Connected to |
| --- | --- | --- | --- | --- | --- |
| 14 | PA0 | SYNC_OUT_TIP | O | GPIO | SYNC OUT jack, tip (clock) |
| 15 | PA1 | SYNC_OUT_RING | O | GPIO | SYNC OUT jack, ring (start/stop) |
| 16 | PA2 | MIDI_OUT_A | O | USART1_TX (AF7) | MIDI OUT A (DIN) |
| 17 | PA3 | MIDI_IN | I | USART1_RX (AF7) | MIDI IN (DIN) |
| 20 | PA4 | SEL_ENC_B | I | GPIO | SELECT encoder, phase B |
| 21 | PA5 | SCAN_CLK | O | GPIO | 74HC165 chain, CP (clock) |
| 22 | PA6 | SCAN_DATA | I | GPIO | 74HC165 chain, Q7 (serial out) |
| 23 | PA7 | SCAN_PL | O | GPIO | 74HC165 chain, PL (parallel load) |
| 41 | PA8 | OLED_RST | O | GPIO | OLED reset |
| 42 | PA9 | SYNC_IN_TIP | I | TIMER0_CH1 (AF1) | SYNC IN jack, tip (clock) |
| 43 | PA10 | SYNC_IN_RING | I | GPIO / EXTI10 | SYNC IN jack, ring (start/stop) |
| 44 | PA11 | USB_DM | I/O | USBFS_DM (AF10) | USB-C (POWER / DATA), D− |
| 45 | PA12 | USB_DP | I/O | USBFS_DP (AF10) | USB-C (POWER / DATA), D+ |
| 46 | PA13 | SWDIO | I/O | SWDIO (AF0) | Debug connector |
| 49 | PA14 | SWCLK | I | SWCLK (AF0) | Debug connector |
| 50 | PA15 | SEL_ENC_A | I | GPIO | SELECT encoder, phase A |

### 1.2 Port B

| Pin | Port | Signal | Dir | MCU function (AF) | Connected to |
| --- | --- | --- | --- | --- | --- |
| 26 | PB0 | GATE3_10V | O | GPIO | GATE 3 output stage, 10 V select |
| 27 | PB1 | GATE3_5V | O | GPIO | GATE 3 output stage, 5 V select |
| 28 | PB2 | MCU_PB1 | — | BOOT1 | No function assigned |
| 55 | PB3 | MCU_SWO | — | — | Connected to PB11 (PCB routing error); not usable as SWO |
| 56 | PB4 | FOOTSW1 | I | GPIO | FOOT 1 jack |
| 57 | PB5 | CV_DAC_DIN | O | SPI2_MOSI (AF6) | CV DAC, data in |
| 58 | PB6 | GATE3_0V | O | GPIO | GATE 3 output stage, 0 V select |
| 59 | PB7 | FOOTSW2 | I | GPIO | FOOT 2 jack |
| 61 | PB8 | LED_SCL | O (open-drain) | I2C0_SCL (AF4) | HT16K33 LED drivers, SCL |
| 62 | PB9 | LED_SDA | I/O (open-drain) | I2C0_SDA (AF4) | HT16K33 LED drivers, SDA |
| 29 | PB10 | CV_MUX_EN | O | GPIO | CV demultiplexer enable (active low) |
| 30 | PB11 | MCU_PB2 | — | — | Connected to PB3 |
| 33 | PB12 | OLED_DC | O | GPIO | OLED data/command select |
| 34 | PB13 | SPI1_SCK | O | SPI1_SCK (AF5) | OLED and SPI flash, SCK |
| 35 | PB14 | SPI1_MISO | I | SPI1_MISO (AF5) | SPI flash, SO |
| 36 | PB15 | SPI1_MOSI | O | SPI1_MOSI (AF5) | OLED and SPI flash, SI |

### 1.3 Port C

| Pin | Port | Signal | Dir | MCU function (AF) | Connected to |
| --- | --- | --- | --- | --- | --- |
| 8 | PC0 | CV_MUX_S0 | O | GPIO | CV demultiplexer, select S0 |
| 9 | PC1 | CV_MUX_S1 | O | GPIO | CV demultiplexer, select S1 |
| 10 | PC2 | GATE1_10V | O | GPIO | GATE 1 output stage, 10 V select |
| 11 | PC3 | GATE1_5V | O | GPIO | GATE 1 output stage, 5 V select |
| 24 | PC4 | GATE1_0V | O | GPIO | GATE 1 output stage, 0 V select |
| 25 | PC5 | GATE2_10V | O | GPIO | GATE 2 output stage, 10 V select |
| 37 | PC6 | MIDI_OUT_B | O | USART5_TX (AF8) | MIDI OUT B (DIN) |
| 38 | PC7 | FLASH_CS | O | GPIO | SPI flash, CS (active low) |
| 39 | PC8 | GATE2_5V | O | GPIO | GATE 2 output stage, 5 V select |
| 40 | PC9 | GATE2_0V | O | GPIO | GATE 2 output stage, 0 V select |
| 51 | PC10 | CV_DAC_SCLK | O | SPI2_SCK (AF6) | CV DAC, SCLK |
| 52 | PC11 | CV_DAC_CS | O | GPIO | CV DAC, CS (active low) |
| 53 | PC12 | OLED_CS | O | GPIO | OLED, CS (active low) |
| 2 | PC13 | GATE4_10V | O | GPIO | GATE 4 output stage, 10 V select |
| 3 | PC14 | GATE4_5V | O | GPIO | GATE 4 output stage, 5 V select |
| 4 | PC15 | GATE4_0V | O | GPIO | GATE 4 output stage, 0 V select |

PC13–PC15 are backup-domain pins with limited drive capability (max. 2 MHz, 3 mA sink, one at a time).

### 1.4 Port D and system pins

| Pin | Port | Signal | Dir | Connected to |
| --- | --- | --- | --- | --- |
| 54 | PD2 | PSU_STATUS | I | Power supply status |
| 5 | PH0 | OSC_IN | I | 8 MHz crystal |
| 6 | PH1 | OSC_OUT | O | 8 MHz crystal |
| 7 | NRST | NRST | I | Reset |
| 60 | BOOT0 | BOOT0 | I | Boot mode select (low = boot from main flash) |

---

## 2. Peripheral Allocation

| MCU peripheral | Pins | Interface | Connected device |
| --- | --- | --- | --- |
| USBFS | PA11, PA12 | USB 2.0 full speed, device | Host computer (USB MIDI) |
| USART1 | PA2, PA3 | UART, 31 250 baud, 8N1 | MIDI OUT A, MIDI IN |
| USART5 | PC6 | UART, 31 250 baud, 8N1, TX only | MIDI OUT B |
| I2C0 | PB8, PB9 | I2C master | 5 × HT16K33 LED driver |
| SPI1 | PB13, PB14, PB15 | SPI master, mode 0, MSB first | SSD1306 OLED, SPI NOR flash (shared bus) |
| SPI2 | PC10, PB5 | SPI master, mode 0, MSB first, TX only | CV DAC |
| TIMER0 | PA9 | Input capture, CH1 | SYNC IN clock |
| EXTI | PA10 | External interrupt, line 10 | SYNC IN start/stop |
| SWD | PA13, PA14 | Serial Wire Debug | Debug connector |

---

## 3. External Devices

### 3.1 CV outputs

One SPI DAC (SPI2) drives a 4-channel analogue demultiplexer with sample-and-hold, giving CV 1–4 (0–10 V).

| CV_MUX_S1 (PC1) | CV_MUX_S0 (PC0) | Output |
| --- | --- | --- |
| 0 | 0 | CV 1 |
| 0 | 1 | CV 2 |
| 1 | 0 | CV 3 |
| 1 | 1 | CV 4 |

CV_MUX_EN (PB10): low = demultiplexer enabled, high = all outputs held.

DAC registers: `0x04` gain, `0x08` data (16-bit).

### 3.2 GATE outputs

Each of the four gate output stages has three select lines. Output level:

| Output level | xx_0V | xx_5V | xx_10V |
| --- | --- | --- | --- |
| 0 V | 1 | 0 | 0 |
| 5 V | 0 | 1 | 0 |
| 10 V | 0 | 1 | 1 |

| Gate | 0V line | 5V line | 10V line |
| --- | --- | --- | --- |
| GATE 1 | PC4 | PC3 | PC2 |
| GATE 2 | PC9 | PC8 | PC5 |
| GATE 3 | PB6 | PB1 | PB0 |
| GATE 4 | PC15 | PC14 | PC13 |

### 3.3 Buttons and encoders

- 16 × 74HC165 parallel-in/serial-out shift registers in a daisy chain (128 inputs).
- Inputs: all panel keys, the push switches and A/B phases of the 32 encoders.
- Control: PL (PA7), CP (PA5); serial data from Q7 of the last stage to PA6.
- The SELECT encoder phases connect directly to PA15 (A) and PA4 (B).

### 3.4 LED drivers

Five HT16K33 on I2C0:

| 7-bit address | 8-bit write address | LEDs |
| --- | --- | --- |
| `0x70` | `0xE0` | Keys (RGB and single-colour) |
| `0x72` | `0xE4` | Encoder LED rings, row 1 |
| `0x73` | `0xE6` | Encoder LED rings, row 2 |
| `0x74` | `0xE8` | Encoder LED rings, row 3 |
| `0x75` | `0xEA` | Encoder LED rings, row 4 |

### 3.5 OLED display

| Item | Value |
| --- | --- |
| Controller | SSD1306 |
| Resolution | 128 × 32, monochrome |
| Interface | 4-wire SPI (SPI1), max. 10 MHz |
| Pins | SCK PB13, MOSI PB15, CS PC12, D/C PB12, RST PA8 |

### 3.6 SPI NOR flash

| Item | Value |
| --- | --- |
| Capacity | 512 KB |
| Erase sector | 4 KB |
| Interface | SPI (SPI1, shared with the OLED) |
| Pins | SCK PB13, SO→MISO PB14, SI←MOSI PB15, CS PC7 |

### 3.7 MIDI

| Jack | MCU pin | Peripheral |
| --- | --- | --- |
| MIDI IN | PA3 | USART1_RX |
| MIDI OUT A | PA2 | USART1_TX |
| MIDI OUT B | PC6 | USART5_TX |

### 3.8 SYNC and footswitches

| Jack | Contact | MCU pin | Dir |
| --- | --- | --- | --- |
| SYNC IN (3.5 mm TRS) | Tip – clock | PA9 | I |
| SYNC IN | Ring – start/stop | PA10 | I |
| SYNC OUT (3.5 mm TRS) | Tip – clock | PA0 | O |
| SYNC OUT | Ring – start/stop | PA1 | O |
| FOOT 1 (6.35 mm TS) | Tip | PB4 | I |
| FOOT 2 (6.35 mm TS) | Tip | PB7 | I |

---

## 4. Unused and Special Pins

| Pin | Port | Note |
| --- | --- | --- |
| 28 | PB2 (BOOT1) | No function assigned |
| 55 | PB3 (SWO) | Connected to PB11 on the PCB; SWO trace output not available |
| 30 | PB11 | Connected to PB3 |
| 2–4 | PC13–PC15 | Backup-domain pins, low drive (see §1.3) |
