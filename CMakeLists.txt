# Copyright (c) 2026 LVGL
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(lvgl_application)

set(LVGL_DIR ${ZEPHYR_LVGL_MODULE_DIR})

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE
    ${app_sources}
    ${LVGL_DIR}/demos/lv_demos.c
)

target_sources_ifdef(CONFIG_LV_USE_TEST_SCREENSHOT_COMPARE app PRIVATE tests/screenshot.c)

# lv_fs_posix.c uses DT_DIR (a GNU/BSD extension). Zephyr sets
# -D_POSIX_C_SOURCE=200809L which hides it; _GNU_SOURCE restores it.
zephyr_compile_definitions_ifdef(CONFIG_LV_USE_FS_POSIX _GNU_SOURCE)

target_include_directories(app PRIVATE
    ${LVGL_DIR}/demos/
)

target_sources_ifdef(CONFIG_LV_USE_DEMO_WIDGETS app PRIVATE
    ${LVGL_DIR}/demos/widgets/assets/img_clothes.c
    ${LVGL_DIR}/demos/widgets/assets/img_demo_widgets_avatar.c
    ${LVGL_DIR}/demos/widgets/assets/img_demo_widgets_needle.c
    ${LVGL_DIR}/demos/widgets/assets/img_lvgl_logo.c
    ${LVGL_DIR}/demos/widgets/lv_demo_widgets.c
    ${LVGL_DIR}/demos/widgets/lv_demo_widgets_analytics.c
    ${LVGL_DIR}/demos/widgets/lv_demo_widgets_components.c
    ${LVGL_DIR}/demos/widgets/lv_demo_widgets_profile.c
    ${LVGL_DIR}/demos/widgets/lv_demo_widgets_shop.c
)
