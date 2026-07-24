/*
 * Copyright (c) 2026 LVGL
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "intro.h"
#include <lvgl.h>

#define TIMER_PERIOD_MS   16    /* ~60 fps */
#define VEL_X             4     /* pixels per tick */
#define VEL_Y             3

static lv_obj_t  *s_btn;
static int32_t    s_x, s_y;
static int32_t    s_vx, s_vy;
static lv_timer_t *s_timer;

static void bounce_cb(lv_timer_t *timer)
{
	lv_display_t *disp = lv_display_get_default();
	int32_t scr_w = lv_display_get_horizontal_resolution(disp);
	int32_t scr_h = lv_display_get_vertical_resolution(disp);
	int32_t btn_w = lv_obj_get_width(s_btn);
	int32_t btn_h = lv_obj_get_height(s_btn);

	s_x += s_vx;
	s_y += s_vy;

	if (s_x <= 0)              { s_x = 0;             s_vx =  VEL_X; }
	if (s_y <= 0)              { s_y = 0;             s_vy =  VEL_Y; }
	if (s_x + btn_w >= scr_w) { s_x = scr_w - btn_w; s_vx = -VEL_X; }
	if (s_y + btn_h >= scr_h) { s_y = scr_h - btn_h; s_vy = -VEL_Y; }

	lv_obj_set_pos(s_btn, s_x, s_y);
}

/* Center the button and start the bounce timer once the screen is visible. */
static void on_scr_loaded(lv_event_t *e)
{
	lv_obj_t *scr = lv_event_get_target(e);

	lv_obj_update_layout(scr);
	lv_display_t *disp = lv_display_get_default();
	s_x = (lv_display_get_horizontal_resolution(disp) - lv_obj_get_width(s_btn)) / 2;
	s_y = (lv_display_get_vertical_resolution(disp) - lv_obj_get_height(s_btn)) / 2;
	lv_obj_set_pos(s_btn, s_x, s_y);

	s_timer = lv_timer_create(bounce_cb, TIMER_PERIOD_MS, NULL);
}

/* Stop the bounce timer when the intro screen is no longer active. */
static void on_scr_unloaded(lv_event_t *e)
{
	if (s_timer) {
		lv_timer_delete(s_timer);
		s_timer = NULL;
	}
}

lv_obj_t *intro_start(void)
{
	s_vx = VEL_X;
	s_vy = VEL_Y;

	lv_obj_t *scr = lv_obj_create(NULL);
	lv_obj_add_event_cb(scr, on_scr_loaded,   LV_EVENT_SCREEN_LOADED,   NULL);
	lv_obj_add_event_cb(scr, on_scr_unloaded, LV_EVENT_SCREEN_UNLOADED, NULL);

	lv_obj_set_style_bg_color(scr, lv_color_hex(0x1a1a2e), 0);

	s_btn = lv_button_create(scr);
	lv_label_set_text(lv_label_create(s_btn), "LVGL <3 Zephyr");

	return scr;
}
