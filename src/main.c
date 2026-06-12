/*
 * Copyright (c) 2026 LVGL
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>
#include <lvgl_mem.h>
#include <lvgl_zephyr.h>
#include <lv_demos.h>
#include <stdio.h>

#ifdef CONFIG_APP_SCREENSHOT
#include "screenshot.h"
#endif

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

static void create_ui(void)
{
	/* Replace this demo with your own UI code */
	lv_demo_widgets();
}

int main(void)
{
	const struct device *display_dev;
	int ret;

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Display device not ready, aborting");
		return 0;
	}

	/* LVGL is initialized by the Zephyr LVGL module before main() runs.
	 * The lock protects LVGL state from concurrent access, e.g. by the
	 * shell or a rendering workqueue.
	 */
	lvgl_lock();
	create_ui();
#ifndef CONFIG_LV_Z_RUN_LVGL_ON_WORKQUEUE
	lv_timer_handler();
#endif
	lvgl_unlock();

	ret = display_blanking_off(display_dev);
	if (ret < 0 && ret != -ENOSYS) {
		LOG_ERR("Failed to turn blanking off (error %d)", ret);
		return 0;
	}

#ifdef CONFIG_LV_Z_MEM_POOL_SYS_HEAP
	lvgl_print_heap_info(false);
#else
	printf("lvgl in malloc mode\n");
#endif

	while (1) {
#ifdef CONFIG_LV_Z_RUN_LVGL_ON_WORKQUEUE
		/* LVGL is driven by a dedicated workqueue */
		k_msleep(10);
#else
		uint32_t sleep_ms;

		lvgl_lock();
		sleep_ms = lv_timer_handler();
#ifdef CONFIG_APP_SCREENSHOT
		app_screenshot_poll();
#endif
		lvgl_unlock();

		k_msleep(MIN(sleep_ms, INT32_MAX));
#endif
	}

	return 0;
}
