# Supported boards

## Build commands

Run these from inside the `lv_zephyr` directory. Where a shield is required it
is auto-selected by `boards/<board>.cmake`; no `--shield` flag needed.

**Tested:**

| Board | Build command |
|-------|---------------|
| [EK-RA8D1](https://www.renesas.com/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ek-ra8d1-evaluation-kit-ra8d1-mcu-group) | `west build -p -b ek_ra8d1` |
| [EK-RA6M3](https://www.renesas.com/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ek-ra6m3-evaluation-kit-ra6m3-mcu-group) | `west blobs fetch hal_renesas`, then `west build -p -b ek_ra6m3` |
| [EK-RA8D2](https://www.renesas.com/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ek-ra8d2-evaluation-kit-ra8d2-mcu-group) | `west build -p -b ek_ra8d2/r7ka8d2kflcac/cm85` |

**Untested** (configuration present; not hardware-verified):

| Board | Build command |
|-------|---------------|
| [STM32U5G9J-DK2](https://www.st.com/en/evaluation-tools/stm32u5g9j-dk2.html) | `west build -p -b stm32u5g9j_dk2` |
| [FRDM-MCXN947](https://www.nxp.com/design/design-center/development-boards-and-designs/FRDM-MCXN947) | `west build -p -b frdm_mcxn947/mcxn947/cpu0` |
| [MIMXRT1170-EVK](https://www.nxp.com/design/design-center/development-boards-and-designs/MIMXRT1170-EVK) | `west build -p -b mimxrt1170_evk@B/mimxrt1176/cm7` |
| [M5Stack Core2](https://shop.m5stack.com/products/m5stack-core2-esp32-iot-development-kit-v1-1) | `west blobs fetch hal_espressif`, then `west build -p -b m5stack_core2/esp32/procpu` |

> [!TIP]
> ESP32 builds (M5Stack Core2) additionally need the `esptool` Python package
> on your PATH; install it together with the rest of Zephyr's Python
> dependencies with `west packages pip --install`.

## Display hardware details

| Board | Interface / Panel | Resolution | Color depth | Memory regions |
|-------|-------------------|------------|-------------|----------------|
| EK-RA8D1 | MIPI DSI — ILI9806E<br>RTK-MIPI-LCD-B-00000BE | 480 × 854 | RGB565 | 0x00000000, 64 KB — ITCM<br>0x02000000, 2016 KB — Code flash<br>0x20000000, 64 KB — DTCM<br>0x22000000, 896 KB — SRAM<br>0x27000000, 12 KB — Data flash<br>0x68000000, 64 MB — SDRAM<br>0x90000000, 64 MB — OctoSPI NOR |
| EK-RA6M3 | GLCDC parallel RGB<br>RTK7EKA6M3B00001BU | 480 × 272 | RGB565‡ | 0x00000000, 2 MB — Code flash<br>0x1FFE0000, 640 KB — SRAM<br>0x20040400, 255 KB — GLCDC\_FB (SRAM sub-region)<br>0x40100000, 64 KB — Data flash<br>0x60000000, 32 MB — QSPI NOR |
| STM32U5G9J-DK2 | LTDC parallel RGB<br>(onboard, GT911 touch) | 800 × 480 | RGB565 | 0x08000000, 4 MB — Flash<br>0x20000000, 3008 KB — SRAM<br>0x28000000, 16 KB — SRAM4<br>0xA0000000, 128 MB — PSRAM |
| EK-RA8D2 | GLCDC parallel RGB<br>RTK-LCD-PAR1S-00001BE | 1024 × 600 | RGB565 | 0x02000000, 768 KB — MRAM (CM85)<br>0x020C0000, 256 KB — MRAM (CM33)<br>0x22000000, 1 MB — SRAM0<br>0x22100000, 640 KB — SRAM1<br>0x68000000, 64 MB — SDRAM |
| FRDM-MCXN947 | 8080 MIPI DBI — ST7796S<br>LCD-PAR-S035 | 480 × 320 | RGB565 | 0x04000000, 96 KB — SRAMX<br>0x10000000, 2 MB — Flash<br>0x20000000, 320 KB — SRAM0<br>0x20050000, 64 KB — SRAMG<br>0x20060000, 32 KB — SRAMH<br>0x90000000, 8 MB — QSPI NOR |
| MIMXRT1170-EVK | MIPI DSI — HX8394<br>RK055HDMIPI4MA0 | 720 × 1280 | RGB565 | 0x00000000, 256 KB — ITCM<br>0x20000000, 256 KB — DTCM<br>0x20200000, 256 KB — OCRAM<br>0x20240000, 512 KB — OCRAM1<br>0x202C0000, 512 KB — OCRAM2<br>0x30000000, 64 MB — Flash<br>0x80000000, 64 MB — SDRAM |
| M5Stack Core2 | SPI — ILI9342C<br>(onboard) | 320 × 240 | RGB565 | 0x00000000, 16 MB — Flash<br>0x3F400000, 4 MB — Flash window<br>0x3F800000, 4 MB — PSRAM window (8 MB physical)<br>0x3FF80000, 8 KB — RTC fast RAM<br>0x3FFAE000, 200 KB — SRAM2<br>0x3FFE0000, 128 KB — SRAM1<br>0x40070000, 192 KB — SRAM0<br>0x50000000, 8 KB — RTC slow RAM |

‡ **Known issue — green channel 1-pixel stripe:** A faint 1-pixel vertical stripe appears at color-carry transitions where the three green LSBs (DATA5–7, pins P608–P610 on PORT6) all switch high-to-low simultaneously. Affected 8-bit green values: 60→64 (most visible), 92→96, 124→128, 156→160, 188→192 (least visible). Root cause: the P608–P610 traces on the LCD expansion connector have higher capacitive load than the red/blue data lines, causing insufficient settling time even with maximum drive strength (`DSCR = high`). The pixel clock is already at the minimum supported divisor (32 → 7.5 MHz). A full fix would require hardware changes (series resistors on those three traces, or a board redesign with shorter routing). For most application UIs the artifact is imperceptible.

Memory regions are sourced from the SoC DTSI files and board DTS. All boards render in RGB565 (`LV_COLOR_DEPTH_16`); the wire format may differ — LTDC (STM32) and GLCDC/DSI (RA8D1, RA8D2) output RGB888 downstream.

## Adding another board

1. Make sure the board's HAL module is in the `name-allowlist` of
   [manifest/west.yml](manifest/west.yml) (module names are in
   `deps/zephyr/west.yml`), then run `west update`.
2. Optionally add `boards/<board>.cmake` / `.conf` / `.overlay` for
   board-specific settings (see below).
3. Build with `west build -p -b <board>`.

### Board overlay files

Four files in `boards/` let you customize a board without touching `deps/`:

| File | When applied | Purpose |
|------|-------------|---------|
| `boards/<board>.cmake` | CMake configure | Set CMake variables before Zephyr processes them (e.g. default `SHIELD`) |
| `boards/<board>.conf` | Always | Board-specific Kconfig (memory sizes, drivers, etc.) |
| `boards/<board>.overlay` | After shield overlays | Override shield DTS settings, add devices the shield didn't configure |
| `boards/pre-shield-overlays/<board>.overlay` | **Before** shield overlays | Define DTS nodes that the shield overlay references with `&label` |

The third file exists because Zephyr processes overlay files in a fixed order:

```
board DTS
  → pre-shield-overlays/<board>.overlay   (BOARD_EXTENSION_DIRS)
    → shield overlay                       (--shield flag)
      → boards/<board>.overlay             (auto-discovered, DTC_OVERLAY_FILE)
```

A shield overlay that contains `&zephyr_lcdif { ... }` requires the
`zephyr_lcdif` label to exist *before* the shield is processed. If that label
isn't defined in the upstream board DTS (as is the case for EK-RA6M3, whose
Zephyr board support predates GLCDC), you must create it in
`boards/pre-shield-overlays/` so it is available when the shield runs. The
regular `boards/<board>.overlay` arrives too late for this purpose.

Most boards won't need a pre-shield overlay — it is only required when adding
a peripheral that the shield references but the board's upstream DTS doesn't
define.
