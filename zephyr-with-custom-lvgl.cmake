# Build customizations for running a custom LVGL revision with this project.
#
# Zephyr compiles LVGL from a hand-maintained source list that ships with
# Zephyr, so a custom LVGL whose file layout differs (a source it adds or
# removes) can break the build. This file lets you reconcile those differences
# -- and override LVGL config values -- without editing Zephyr or LVGL, using
# CMake's built-in CMAKE_PROJECT_INCLUDE:
#
#     west build -p -b <board> -- -DCMAKE_PROJECT_INCLUDE=$PWD/zephyr-with-custom-lvgl.cmake
#
# CMAKE_PROJECT_INCLUDE is run right after every project() call. A Zephyr build
# has two -- project(Zephyr-Kernel), during find_package(Zephyr) before module
# targets exist, and the application's own project() afterwards, when they do.
# The guard below is load-bearing: it makes this file act only on the pass where
# the LVGL library target is present.
#
# Targets you can touch (LVGL source is split across two):
#   modules__lvgl : static library that compiles LVGL's own sources
#   app           : where the demos and your UI code are compiled

if(NOT TARGET modules__lvgl)
  return()
endif()

message(STATUS "[custom-lvgl] applying build customizations")

# --- Reconcile a custom LVGL's source layout --------------------------------
# Add a source your revision introduced:
# target_sources(modules__lvgl PRIVATE
#     ${ZEPHYR_LVGL_MODULE_DIR}/src/misc/lv_my_new_file.c)
#
# Remove a source your revision deleted (or that you don't want compiled):
# get_target_property(_srcs modules__lvgl SOURCES)
# list(FILTER _srcs EXCLUDE REGEX "widgets/win/lv_win\\.c$")
# set_target_properties(modules__lvgl PROPERTIES SOURCES "${_srcs}")

# --- Override an LVGL config value (clean -- no source edits) ----------------
# LVGL resolves options with `#ifndef LV_X`, so a -D define supplied here is
# seen first and wins over the Kconfig value. Apply it to whichever target
# compiles the code that reads the macro. Example: force the default theme to
# dark mode (read by lv_display.c in the library AND the demo in `app`):
# foreach(_tgt modules__lvgl app)
#   if(TARGET ${_tgt})
#     target_compile_definitions(${_tgt} PRIVATE LV_THEME_DEFAULT_DARK=1)
#   endif()
# endforeach()

# Note: mutating an existing target relies on the internal target name
# (modules__lvgl is derived from the glue path modules/lvgl). Re-run a clean
# build (west build -p) after changing which files LVGL compiles.
