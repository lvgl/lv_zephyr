# LVGL on Zephyr RTOS — starter project

Get an [LVGL](https://lvgl.io/) application running on [Zephyr RTOS](https://zephyrproject.org/)
in minutes — on your PC (no hardware needed) or on one of the supported development boards.

This repository is a self-contained Zephyr
[workspace application](https://docs.zephyrproject.org/latest/develop/application/index.html#zephyr-workspace-application):
cloning it and running `west update` fetches Zephyr and every module needed to build,
so you do **not** need a pre-existing Zephyr installation or any extra tooling.

## Prerequisites

Follow steps 1–4 of the official
[Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)
to install:

1. The host dependencies (CMake, Python, devicetree compiler)
2. `west`, Zephyr's meta-tool (`pip install west`)
3. The [Zephyr SDK](https://docs.zephyrproject.org/latest/develop/toolchains/zephyr_sdk.html) (toolchains)

To run the simulator on your PC you also need SDL2
(`sudo apt install libsdl2-dev` on Ubuntu/Debian).

## Quick start

```sh
# Create a workspace folder and clone this repository into it
mkdir lvgl-zephyr-workspace && cd lvgl-zephyr-workspace
git clone https://github.com/lvgl/lv_zephyr.git

# Initialize the west workspace and download Zephyr + modules
west init -l lv_zephyr
west update

# Build from inside the application repository
cd lv_zephyr
```

After `west update` the workspace keeps your code and the auto-downloaded
dependencies cleanly separated:

```
lvgl-zephyr-workspace/
├── lv_zephyr/        # this repository: your application + west manifest
└── deps/             # auto-downloaded by west — don't edit
    ├── zephyr/       # Zephyr RTOS
    ├── modules/      # LVGL and vendor HALs
    └── bootloader/   # MCUboot (used by ESP32 targets)
```

> [!NOTE]
> `west update` downloads Zephyr and the vendor HALs for all supported boards
> (a few GB). If you only target one board, you can trim the
> `name-allowlist` in [west.yml](west.yml) before running it.

### Run on your PC (simulator)

```sh
west build -b native_sim/native/64 -t run
```

A window opens showing the LVGL widgets demo:
clicks act as touch input, and the Zephyr shell is available in the terminal.

### Run on a development board

Run these from inside the `lv_zephyr` directory:

| Board | Build command |
|-------|---------------|
| [STM32U5G9J-DK2](https://www.st.com/en/evaluation-tools/stm32u5g9j-dk2.html) | `west build -p -b stm32u5g9j_dk2` |
| [EK-RA8D1](https://www.renesas.com/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ek-ra8d1-evaluation-kit-ra8d1-mcu-group) | `west build -p -b ek_ra8d1 --shield rtkmipilcdb00000be` |
| [FRDM-MCXN947](https://www.nxp.com/design/design-center/development-boards-and-designs/FRDM-MCXN947) | `west build -p -b frdm_mcxn947/mcxn947/cpu0 --shield lcd_par_s035_8080` |
| [MIMXRT1170-EVK](https://www.nxp.com/design/design-center/development-boards-and-designs/MIMXRT1170-EVK) | `west build -p -b mimxrt1170_evk@B/mimxrt1176/cm7 --shield rk055hdmipi4ma0` |
| [M5Stack Core2](https://shop.m5stack.com/products/m5stack-core2-esp32-iot-development-kit-v1-1) | `west blobs fetch hal_espressif`, then `west build -p -b m5stack_core2/esp32/procpu` |

> [!TIP]
> ESP32 builds (M5Stack Core2) additionally need the `esptool` Python package
> on your PATH; install it together with the rest of Zephyr's Python
> dependencies with `west packages pip --install`.

Then flash and (optionally) debug:

```sh
west flash
west debug
```

Any other Zephyr board with a display works too — see
[Adding another board](#adding-another-board).

## Make it your own

The repository is the application:

```
lv_zephyr/
├── west.yml         # West manifest: pins Zephyr and the modules to fetch
├── CMakeLists.txt   # Application build, pulls in the LVGL demo sources
├── prj.conf         # Zephyr + LVGL configuration (Kconfig)
├── boards/          # Per-board configuration overrides
└── src/main.c       # Application entry point
```

Start in [src/main.c](src/main.c): replace the `lv_demo_widgets()` call in
`create_ui()` with your own UI code. The display, input devices and LVGL
itself are initialized automatically by Zephyr from the devicetree before
`main()` runs.

LVGL is configured through Kconfig (`prj.conf` or `west build -t menuconfig`):
enable widgets, fonts and features there, and tune
`CONFIG_LV_Z_MEM_POOL_SIZE` / `CONFIG_LV_Z_VDB_SIZE` for performance vs. RAM.
See the [LVGL on Zephyr documentation](https://lvgl.io/docs/details/integration/rtos/zephyr)
for details.

You can design UIs visually with the [LVGL Editor](https://lvgl.io/editor) and
drop the exported code into `src/`.

### Adding another board

1. Make sure the board's HAL module is in the `name-allowlist` of
   [west.yml](west.yml) (module names are in `deps/zephyr/west.yml`), then run
   `west update`.
2. Optionally add `boards/<board>.conf` / `.overlay` for board-specific
   settings.
3. Build with `west build -p -b <board>`.

### Updating Zephyr or LVGL

The Zephyr release is pinned by the `revision` of the `zephyr` project in
[west.yml](west.yml); the LVGL version is the one bundled with that Zephyr
release as the `lvgl` module. To upgrade, bump the revision and run
`west update`.

## Using the LVGL Project Creator

This repository also serves as the template used by the
[LVGL project creator](https://lvgl.io/tools/project-creator). Projects
generated by the creator follow the same workflow as above: run
`west init -l <project folder>` next to the generated project, `west update`,
then build with the board-specific command shown in the table.
