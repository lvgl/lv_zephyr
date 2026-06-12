/*
 * Copyright (c) 2026 LVGL
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SCREENSHOT_H
#define APP_SCREENSHOT_H

/**
 * Call periodically from the LVGL handler loop (with the LVGL lock held).
 * Once the UI has settled (no running animations), takes a screenshot of
 * the active screen, writes it to screenshot.bmp and exits the process.
 */
void app_screenshot_poll(void);

#endif /* APP_SCREENSHOT_H */
