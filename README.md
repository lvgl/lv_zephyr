# LVGL on Zephyr RTOS — starter project

[![CI](https://github.com/lvgl/lv_zephyr/actions/workflows/ci.yml/badge.svg)](https://github.com/lvgl/lv_zephyr/actions/workflows/ci.yml)

Get an [LVGL](https://lvgl.io/) application running on [Zephyr RTOS](https://zephyrproject.org/)
in minutes — on your PC (no hardware needed) or on a supported development board.

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

## Run on your PC (simulator)

```sh
west build -b native_sim/native/64 -t run
```

A window opens showing the LVGL widgets demo:
clicks act as touch input, and the Zephyr shell is available in the terminal.

## Run on a development board

For supported boards, build commands, and hardware details see [BOARDS.md](BOARDS.md).

Then flash and (optionally) debug:

```sh
west flash
west debug
```

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

## Choosing the LVGL version

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

## Updating Zephyr

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
