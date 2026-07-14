/*
 * Copyright (c) 2026 LVGL
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Self-contained screenshot thread: compiled when
 * CONFIG_LV_USE_TEST_SCREENSHOT_COMPARE=y. No changes to main.c are required
 * — the thread is started automatically by the Zephyr kernel at boot.
 *
 * Workflow:
 *   1. Sleep until the demo screen is visible (after the intro).
 *   2. Wait up to MAX_UPTIME_MS for all LVGL animations to finish.
 *   3. Compare the framebuffer against tests/screenshot-reference.png.
 *   4. Exit 0 on pass, exit 1 on mismatch (diff → _err.png).
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
#include <stdio.h>

/* Reference image path, without the .png extension. Defaults to the standard
 * reference; overridable at run time via the LV_Z_SCREENSHOT_REFERENCE env var
 * so a customized build (e.g. one exercising zephyr-with-custom-lvgl.cmake) can
 * be checked against its own reference. This is test-only behaviour, hence a
 * run-time env var rather than an application Kconfig option. */
#define SCREENSHOT_BASE_DEFAULT "tests/screenshot-reference"
#define SCREENSHOT_BASE_ENV     "LV_Z_SCREENSHOT_REFERENCE"

#define MIN_UPTIME_MS     (CONFIG_APP_INTRO_DURATION_MS + 500)
#define MAX_UPTIME_MS     (CONFIG_APP_INTRO_DURATION_MS + 8000)
#define SETTLE_ITERATIONS 50

/* Reference base path (without extension): the LV_Z_SCREENSHOT_REFERENCE env
 * var if set, otherwise the default. The returned pointer is valid for the
 * lifetime of the process. */
static const char *screenshot_reference_base(void)
{
	const char *base = getenv(SCREENSHOT_BASE_ENV);

	if (base == NULL || base[0] == '\0') {
		return SCREENSHOT_BASE_DEFAULT;
	}

	return base;
}

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

	const char *base = screenshot_reference_base();
	char ref_png[256];
	snprintf(ref_png, sizeof(ref_png), "%s.png", base);

	lvgl_lock();
	lv_test_screenshot_result_t res = lv_test_screenshot_compare(ref_png);
	lvgl_unlock();

	if (res == LV_TEST_SCREENSHOT_RESULT_NO_REFERENCE_IMAGE) {
		fprintf(stderr, "Reference image not found: %s\n", ref_png);
		exit(1);
	} else if (res != LV_TEST_SCREENSHOT_RESULT_PASSED) {
		fprintf(stderr,
			"Screenshot mismatch. Diff written to %s_err.png\n"
			"To update the reference:\n"
			"  mv %s_err.png %s.png\n", base, base, base);
		exit(1);
	}

	exit(0);
}

K_THREAD_DEFINE(screenshot_tid, 8192, screenshot_thread, NULL, NULL, NULL, 5, 0, 0);
