/*
 * Copyright (c) 2026 LVGL
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_INTRO_H
#define APP_INTRO_H

#include <lvgl.h>

/**
 * Build the intro screen with a "LVGL <3 Zephyr" button. Does NOT load the
 * screen — the caller must call lv_screen_load() to make it visible. The
 * bounce animation starts on LV_EVENT_SCREEN_LOADED and stops automatically
 * on LV_EVENT_SCREEN_UNLOADED. Must be called with the LVGL lock held.
 */
lv_obj_t *intro_start(void);

#endif /* APP_INTRO_H */
