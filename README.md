# LVGL on Zephyr RTOS — starter project

[![CI](https://github.com/lvgl/lv_zephyr/actions/workflows/ci.yml/badge.svg)](https://github.com/lvgl/lv_zephyr/actions/workflows/ci.yml)

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
git clone https://github.com/lvgl/lv_zephyr.git
cd lv_zephyr

# Initialize the west workspace inside the repository
# and download Zephyr + modules into deps/
west init -l manifest
west update
```

Everything stays inside your clone: the code you edit is tracked by git, and
everything west downloads goes to the git-ignored `deps/` folder:

```
lv_zephyr/            # your clone — the whole workspace lives in here
├── CMakeLists.txt    # application build
├── prj.conf          # Zephyr + LVGL configuration
├── boards/           # per-board defaults (.cmake), Kconfig (.conf) and DTS overlays (.overlay)
├── src/main.c        # application entry point
├── manifest/west.yml # west manifest: pins Zephyr and the modules to fetch
└── deps/             # auto-downloaded by west — git-ignored, don't edit
    ├── zephyr/       # Zephyr RTOS
    ├── modules/      # LVGL and vendor HALs
    └── bootloader/   # MCUboot (used by ESP32 targets)
```

> [!NOTE]
> `west update` downloads Zephyr and the vendor HALs for all supported boards
> (a few GB). If you only target one board, you can trim the
> `name-allowlist` in [manifest/west.yml](manifest/west.yml) before running
> it.

### Run on your PC (simulator)

```sh
west build -b native_sim/native/64 -t run
```

A window opens showing the LVGL widgets demo:
clicks act as touch input, and the Zephyr shell is available in the terminal.

### Run on a development board

Run these from inside the `lv_zephyr` directory. Where a shield is required it
is auto-selected by `boards/<board>.cmake`; no `--shield` flag needed.

**Tested:**

| Board | Build command |
|-------|---------------|
| [EK-RA8D1](https://www.renesas.com/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ek-ra8d1-evaluation-kit-ra8d1-mcu-group) | `west build -p -b ek_ra8d1` |
| [EK-RA6M3](https://www.renesas.com/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ek-ra6m3-evaluation-kit-ra6m3-mcu-group) | `west build -p -b ek_ra6m3` |

**Untested** (configuration present; not hardware-verified):

| Board | Build command |
|-------|---------------|
| [STM32U5G9J-DK2](https://www.st.com/en/evaluation-tools/stm32u5g9j-dk2.html) | `west build -p -b stm32u5g9j_dk2` |
| [EK-RA8D2](https://www.renesas.com/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ek-ra8d2-evaluation-kit-ra8d2-mcu-group) | `west build -p -b ek_ra8d2/r7ka8d2kflcac/cm85` |
| [FRDM-MCXN947](https://www.nxp.com/design/design-center/development-boards-and-designs/FRDM-MCXN947) | `west build -p -b frdm_mcxn947/mcxn947/cpu0` |
| [MIMXRT1170-EVK](https://www.nxp.com/design/design-center/development-boards-and-designs/MIMXRT1170-EVK) | `west build -p -b mimxrt1170_evk@B/mimxrt1176/cm7` |
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
   [manifest/west.yml](manifest/west.yml) (module names are in
   `deps/zephyr/west.yml`), then run `west update`.
2. Optionally add `boards/<board>.cmake` / `.conf` / `.overlay` for
   board-specific settings (see below).
3. Build with `west build -p -b <board>`.

#### Board overlay files

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

### Choosing the LVGL version

LVGL is checked out as a standalone dependency straight from
[lvgl/lvgl](https://github.com/lvgl/lvgl), not through Zephyr's manifest.
By default it pins the exact commit the bundled Zephyr release ships with
(v9.5.0 for Zephyr v4.4.0), so out of the box you get the Zephyr-validated
version — but the choice is yours: set the `revision` of the `lvgl` project
in [manifest/west.yml](manifest/west.yml) to any tag, branch or commit SHA:

```yaml
- name: lvgl
  url: https://github.com/lvgl/lvgl
  revision: 85aa60d18b3d5e5588d7b247abf90198f07c8a63  # == v9.5.0, bundled with Zephyr v4.4.0
  # revision: v9.4.0     # …or a release tag of your choice
  # revision: master     # …or the development branch
  path: deps/modules/lib/gui/lvgl
```

then run `west update`. Keep in mind that Zephyr's LVGL glue code
(`deps/zephyr/modules/lvgl`) is written against the LVGL version that Zephyr
release was validated with, so versions far away from the default may need
glue adjustments. When bumping the Zephyr revision, update the default LVGL
pin to match the `lvgl` entry in `deps/zephyr/west.yml`.

### Updating Zephyr

The Zephyr release is pinned by the `revision` of the `zephyr` project in
[manifest/west.yml](manifest/west.yml). To upgrade, bump the revision and run
`west update`.

## Testing

CI ([.github/workflows/ci.yml](.github/workflows/ci.yml)) builds the
application for the simulator and every supported board on each pull
request, and runs a screenshot test: the app is built for native_sim with
[tests/screenshot.conf](tests/screenshot.conf), run headless, and the
rendered UI is captured with LVGL's snapshot feature and compared against
[tests/screenshot-reference.bmp](tests/screenshot-reference.bmp) (small
tolerance for anti-aliasing differences). To run it locally:

```sh
west build -p -b native_sim/native/64 -- -DEXTRA_CONF_FILE=tests/screenshot.conf
SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software ./build/zephyr/zephyr.exe
python3 tests/compare_screenshots.py tests/screenshot-reference.bmp screenshot.bmp diff.bmp
```

After an intentional UI change, regenerate the reference by replacing
`tests/screenshot-reference.bmp` with the new `screenshot.bmp` (CI also
uploads the captured screenshot and a diff visualization as artifacts on
every run).

## Using the LVGL Project Creator

This repository also serves as the template used by the
[LVGL project creator](https://lvgl.io/tools/project-creator). Projects
generated by the creator follow the same workflow as above: inside the
generated project run `west init -l manifest` and `west update`, then build
with `west build -p -b <your-board>`.
