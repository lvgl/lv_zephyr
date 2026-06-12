/*
 * Copyright (c) 2026 LVGL
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef CONFIG_APP_SCREENSHOT

#include <zephyr/kernel.h>
#include <lvgl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "screenshot.h"

#define SCREENSHOT_PATH       "screenshot.bmp"
/* Give the UI time to build up before watching for idle */
#define MIN_UPTIME_MS         2000
/* Capture even if animations never stop (avoids hanging CI) */
#define MAX_UPTIME_MS         15000
/* Handler iterations without running animations before capturing */
#define SETTLE_ITERATIONS     50

static int write_bmp(const char *path, const lv_draw_buf_t *img)
{
	/* 24-bit uncompressed BMP; LVGL RGB888 is B,G,R in memory which is
	 * exactly the BMP pixel order, rows written bottom-up.
	 */
	const uint32_t w = img->header.w;
	const uint32_t h = img->header.h;
	const uint32_t stride = img->header.stride;
	const uint32_t row_size = ((w * 3u) + 3u) & ~3u;
	const uint32_t data_size = row_size * h;
	const uint32_t file_size = 54u + data_size;
	const uint8_t pad[3] = {0};
	uint8_t header[54] = {
		'B', 'M',
		file_size & 0xff, (file_size >> 8) & 0xff,
		(file_size >> 16) & 0xff, (file_size >> 24) & 0xff,
		0, 0, 0, 0,
		54, 0, 0, 0,           /* pixel data offset */
		40, 0, 0, 0,           /* BITMAPINFOHEADER size */
		w & 0xff, (w >> 8) & 0xff, (w >> 16) & 0xff, (w >> 24) & 0xff,
		h & 0xff, (h >> 8) & 0xff, (h >> 16) & 0xff, (h >> 24) & 0xff,
		1, 0,                  /* planes */
		24, 0,                 /* bits per pixel */
		/* remaining fields (compression, sizes, palette) stay zero */
	};

	FILE *f = fopen(path, "wb");

	if (f == NULL) {
		return -1;
	}
	fwrite(header, 1, sizeof(header), f);
	for (int32_t y = (int32_t)h - 1; y >= 0; y--) {
		fwrite(img->data + (uint32_t)y * stride, 1, w * 3u, f);
		fwrite(pad, 1, row_size - w * 3u, f);
	}
	fclose(f);
	return 0;
}

void app_screenshot_poll(void)
{
	static unsigned int settled;
	const int64_t uptime = k_uptime_get();

	if (uptime < MIN_UPTIME_MS) {
		return;
	}
	if (lv_anim_count_running() > 0 && uptime < MAX_UPTIME_MS) {
		settled = 0;
		return;
	}
	if (++settled < SETTLE_ITERATIONS && uptime < MAX_UPTIME_MS) {
		return;
	}

	lv_draw_buf_t *img = lv_snapshot_take(lv_screen_active(), LV_COLOR_FORMAT_RGB888);

	if (img == NULL) {
		fprintf(stderr, "screenshot: lv_snapshot_take() failed\n");
		exit(1);
	}
	if (write_bmp(SCREENSHOT_PATH, img) != 0) {
		fprintf(stderr, "screenshot: cannot write %s\n", SCREENSHOT_PATH);
		exit(1);
	}
	printf("screenshot saved to %s\n", SCREENSHOT_PATH);
	exit(0);
}

#endif /* CONFIG_APP_SCREENSHOT */
