/*
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/keymap.h>

#include "wpm_status.h"

LV_IMG_DECLARE(sym_speedometer);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

// 상태 관리를 전역이 아닌 위젯 초기화 시점에 맞춰 안전하게 처리하도록 구조화
struct wpm_status_state
{
    int wpm;
    const char *layer;
};

static struct wpm_status_state get_state(const zmk_event_t *_eh)
{
    const struct zmk_wpm_state_changed *ev = as_zmk_wpm_state_changed(_eh);
    uint8_t index = zmk_keymap_highest_layer_active();

    return (struct wpm_status_state){
        .wpm = (ev != NULL) ? ev->state : 0,
        .layer = zmk_keymap_layer_name(index)
    };
}

static void set_wpm(struct zmk_widget_wpm_status *widget, struct wpm_status_state state)
{
    if (widget == NULL || widget->wpm_label == NULL) return;

    // 특정 레이어에서 WPM 비활성화 체크 (설정값이 있을 때만)
#if defined(CONFIG_ZMK_DONGLE_DISPLAY_WPM_DISABLED_LAYERS)
    if (state.layer != NULL &&
        strstr(CONFIG_ZMK_DONGLE_DISPLAY_WPM_DISABLED_LAYERS, state.layer) != NULL) {
        lv_label_set_text(widget->wpm_label, "-");
        return;
    }
#endif

    char wpm
