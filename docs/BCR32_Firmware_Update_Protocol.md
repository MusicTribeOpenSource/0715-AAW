# BCR32 Firmware Update Protocol: Entering the Bootloader

| Item | Value |
| --- | --- |
| Document | BCR32 Firmware Update Protocol, part 1: entering the bootloader |
| Transport | USB MIDI, System Exclusive |
| Implemented in | `demo/App/sysex.c` (application side) |

---

## 1. Scope

A firmware update replaces the application in internal flash. The image itself is written by
the BCR32 bootloader; before that the running application has to hand over to it.

This document specifies only that hand-over:

- how a host finds out whether the application or the bootloader is running (`0x74`),
- how the host makes the application start the bootloader (`0x77`).

The transfer of the image to the bootloader is not part of this document; `demo/tools/update/windows/update.exe`
performs it.

### 1.1 Conventions

- All values are hexadecimal unless stated otherwise.
- Message lengths include the leading `F0` and the trailing `F7`.
- `H` denotes the 6-byte message header `F0 00 20 32 7F 64`.

---

## 2. Device modes and USB identity

| Mode | USB VID / PID | USB product name | MIDI ports | `0x74` reply |
| --- | --- | --- | --- | --- |
| Application | `1397` / `12CF` | Behringer BCR-32 | 2 ("BCR-32 CTRL", "BCR-32 ZAQ") | `00` |
| Bootloader | `1397` / `12CE` | Behringer BCR-32 | 1 | `01` |

The bootloader also starts by itself after a reset when no valid application is in flash
(for example after an interrupted update).

---

## 3. SysEx frame

| Offset | Value | Description |
| --- | --- | --- |
| 0 | `F0` | Start of SysEx |
| 1–3 | `00 20 32` | Manufacturer ID (Behringer) |
| 4 | `7F` | Device byte. The application also accepts its device address `00`. |
| 5 | `64` | Model byte (BCR32). The application also accepts `7F`. |
| 6 | `cmd` | Command code |
| 7 | `F7` | End of SysEx (both commands carry no payload) |

Messages with another manufacturer, device or model byte, or with another length, are ignored
silently. The application accepts both commands on either of its USB MIDI ports and answers on
the port the request came from. DIN MIDI is not used for these commands.

---

## 4. Commands

### 4.1 `0x74` Mode query

Request: `F0 00 20 32 7F 64 74 F7` (exactly 8 bytes).

Reply: `F0 00 20 32 dd 64 74 mode F7` (9 bytes)

| `mode` | Meaning |
| --- | --- |
| `00` | Application running |
| `01` | Bootloader running |

`dd` is the device address (`00`) in the application and `7F` in the bootloader.

### 4.2 `0x77` Enter bootloader

Request: `F0 00 20 32 7F 64 77 F7` (exactly 8 bytes). No reply.

In the application:

1. The USB connection is stopped (the host sees the device disconnect).
2. After 200 ms all interrupts, DMA transfers and the SysTick are stopped.
3. The bootloader is started. It re-enumerates over USB with PID `12CE` and a single MIDI port.

The application data in flash is not changed; without an update a reset starts the
application again.

### 4.3 Entering the bootloader with the keys

Holding **T1 + T2 + T3 + T4** together on the BCR32 has the same effect as `0x77`. This is the way
in when no host software can send SysEx, or when the application does not respond.

---

## 5. Sequence

```
Host                                              Device (application)
 |  H 74 F7  ------------------------------------>  |
 |  <------------------------------------  F0 00 20 32 00 64 74 00 F7   (application)
 |  H 77 F7  ------------------------------------>  |   USB disconnect, 200 ms
 |  ... wait for the device with PID 12CE (up to 5 s), open its MIDI port ...
 |  H 74 F7  ------------------------------------>  |   Device (bootloader)
 |  <------------------------------------  F0 00 20 32 7F 64 74 01 F7   (bootloader)
 |  ... image transfer (not part of this document) ...
```

If the first `0x74` already returns `01`, the device is in the bootloader and `0x77` is not
needed.

### 5.1 Timing (recommended host values)

| Step | Recommended |
| --- | --- |
| Reply timeout, `0x74` | ≥ 500 ms |
| After `0x77`: wait for the bootloader device | up to 5 s |

### 5.2 USB-MIDI packets

The host sends the SysEx as standard USB-MIDI 1.0 event packets: CIN `0x4` (SysEx start/continue,
3 bytes), terminated by CIN `0x5`, `0x6` or `0x7` (1, 2 or 3 bytes). An 8-byte command is two
packets, e.g. on cable 0:

```
04 F0 00 20   04 32 7F 64   06 74 F7 00
```

---

## Appendix A. Message examples

| Purpose | Bytes |
| --- | --- |
| Mode query | `F0 00 20 32 7F 64 74 F7` |
| Reply: application | `F0 00 20 32 00 64 74 00 F7` |
| Reply: bootloader | `F0 00 20 32 7F 64 74 01 F7` |
| Enter bootloader | `F0 00 20 32 7F 64 77 F7` |
