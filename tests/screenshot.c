/*
 * Copyright (c) 2026 LVGL
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Self-contained screenshot thread: compiled only when CONFIG_APP_SCREENSHOT=y
 * (set in tests/screenshot.conf). No changes to main.c are required — the
 * thread is started automatically by the Zephyr kernel at boot.
 *
 * Workflow:
 *   1. Sleep until the demo screen is visible (after the intro).
 *   2. Wait up to MAX_UPTIME_MS for all LVGL animations to finish.
 *   3. Compare the framebuffer against tests/screenshot-reference.png.
 *   4. Exit 0 on pass (or when the reference is created for the first time),
 *      exit 1 on mismatch.
 *
 * Must be run from the repository root so that the reference path resolves:
 *   SDL_VIDEODRIVER=dummy ./build/zephyr/zephyr.exe
 *
 * Set CONFIG_APP_INTRO_DURATION_MS in screenshot.conf to speed up the test.
 */

#include <zephyr/kernel.h>
#include <lvgl.h>
#include <lvgl_zephyr.h>
#include <stdlib.h>

#define MIN_UPTIME_MS     (CONFIG_APP_INTRO_DURATION_MS + 500)
#define MAX_UPTIME_MS     (CONFIG_APP_INTRO_DURATION_MS + 8000)
#define SETTLE_ITERATIONS 50

static void screenshot_thread(void *a, void *b, void *c)
{
	/* Wait for the UI to settle past the intro screen. */
	k_msleep(MIN_UPTIME_MS);

	/* Wait for animations to stop, hard-capped at MAX_UPTIME_MS. */
	for (int i = 0; i < SETTLE_ITERATIONS; i++) {
		lvgl_lock();
		int running = lv_anim_count_running();
		lvgl_unlock();
		if (running == 0) {
			break;
		}
		if (k_uptime_get() >= MAX_UPTIME_MS) {
			break;
		}
		k_msleep(16);
	}

	lvgl_lock();
	lv_test_screenshot_result_t res =
		lv_test_screenshot_compare("tests/screenshot-reference.png");
	lvgl_unlock();

	exit(res == LV_TEST_SCREENSHOT_RESULT_PASSED ? 0 : 1);
}

K_THREAD_DEFINE(screenshot_tid, 8192, screenshot_thread, NULL, NULL, NULL, 5, 0, 0);
