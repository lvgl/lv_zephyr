# LVGL on Zephyr RTOS — starter project

[![CI](https://github.com/lvgl/lv_zephyr/actions/workflows/ci.yml/badge.svg)](https://github.com/lvgl/lv_zephyr/actions/workflows/ci.yml)

Get an [LVGL](https://lvgl.io/) application running on [Zephyr RTOS](https://zephyrproject.org/)
in minutes — on your PC (no hardware needed) or on a supported development board.

This repository is a self-contained Zephyr
[workspace application](https://docs.zephyrproject.org/latest/develop/application/index.html#zephyr-workspace-application):
cloning it and running `west update` fetches Zephyr and every module needed to build,
so you do **not** need a pre-existing Zephyr installation or any extra tooling.

## Prerequisites

You'll need a working Zephyr development environment. Follow the official
[Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)
and complete these sections:

1. **Select and Update OS** and **Install dependencies** — the host tools
   (CMake, Python, and the devicetree compiler).
2. **Get Zephyr and install Python dependencies** — a virtual environment,
   `west`, the Zephyr CMake package, and Zephyr's Python dependencies.
3. **Install the Zephyr SDK** — the
   [compiler toolchains](https://docs.zephyrproject.org/latest/develop/toolchains/zephyr_sdk.html).

In section 2 you can skip sub-step 4 ("Get the Zephyr source code"): this
repository is already its own west workspace, so `west init`/`west update` are
covered by the [Quick start](#quick-start) below. Running it anyway is harmless.

To run the simulator on your PC, you also need SDL2
(`sudo apt install libsdl2-dev` on Ubuntu/Debian).

## Quick start

```sh
git clone https://github.com/lvgl/lv_zephyr.git
cd lv_zephyr

# Initialize the west workspace inside the repository
# and download Zephyr + modules into deps/
west init -l manifest
west update
west patch apply   # re-apply local fixes to deps/ (see manifest/zephyr/patches.yml)
```

> [!NOTE]
> Run `west patch apply` after every `west update`. Because `deps/` is
> git-ignored and re-downloaded by west, fixes we carry on top of upstream
> Zephyr/modules live as patches in
> [manifest/zephyr/patches.yml](manifest/zephyr/patches.yml) and must be
> re-applied. Currently this fixes a Renesas DRW (Dave2D) driver compile
> error under GCC 14 that otherwise breaks all three EK-RA board builds.
> `west patch apply` is not idempotent — if a patch is already applied it
> errors out; run `west patch clean` first (or re-run `west update`) to reset.

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

## Customizing the LVGL build

Most of the time you configure LVGL through Kconfig (`prj.conf` or
`west build -t menuconfig`) — that is the normal, first-choice way to turn
features, fonts and options on or off.

Occasionally you need to reach past Kconfig into how the LVGL module is
*compiled* — for example when running a custom LVGL revision whose file layout
differs from the one Zephyr bundles (a source it adds or removes), or to force
a single config value regardless of Kconfig. You can do this **without editing
Zephyr or LVGL** using CMake's built-in `CMAKE_PROJECT_INCLUDE`: point it at a
small CMake file and Zephyr runs it once the build's targets exist.

```sh
west build -p -b native_sim/native/64 -- -DCMAKE_PROJECT_INCLUDE=$PWD/zephyr-with-custom-lvgl.cmake
```

This repository ships a ready-to-edit
[`zephyr-with-custom-lvgl.cmake`](zephyr-with-custom-lvgl.cmake) with the
patterns below. The file runs with LVGL's build targets available, so it can add
or remove sources, or override a configuration value. The two relevant targets
are `modules__lvgl` (the static library that compiles LVGL's own sources) and
`app` (where the demos and your UI code are compiled).

**Override a config value (clean — no source edits).** LVGL resolves every
option with an `#ifndef LV_X` guard, so a `-D` define supplied here is seen
first and *wins* over the Kconfig value. Apply it to whichever target compiles
the code that reads the macro. For example, to force the default theme to dark
mode (read both by the LVGL library and by the widgets demo in `app`):

```cmake
# zephyr-with-custom-lvgl.cmake
if(NOT TARGET modules__lvgl)
  return()   # also fires for Zephyr's internal project(); act only when the target exists
endif()

foreach(tgt modules__lvgl app)
  if(TARGET ${tgt})
    target_compile_definitions(${tgt} PRIVATE LV_THEME_DEFAULT_DARK=1)
  endif()
endforeach()
```

**Add or remove a source** (e.g. to match a custom LVGL revision):

```cmake
# add a file your revision introduced
target_sources(modules__lvgl PRIVATE ${ZEPHYR_LVGL_MODULE_DIR}/src/misc/lv_my_new_file.c)

# remove a file your revision deleted
get_target_property(srcs modules__lvgl SOURCES)
list(FILTER srcs EXCLUDE REGEX "widgets/win/lv_win\\.c$")
set_target_properties(modules__lvgl PROPERTIES SOURCES "${srcs}")
```

> `CMAKE_PROJECT_INCLUDE` is a CMake cache/`-D` argument, not an environment
> variable — use `-- -DCMAKE_PROJECT_INCLUDE=$MY_PATH` and let the shell expand
> your variable. Because it globs/edits the build at configure time, run a clean
> build (`west build -p`) after changing which files LVGL compiles.

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
