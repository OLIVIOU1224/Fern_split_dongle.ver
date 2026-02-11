/*
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/battery.h>
#include <zmk/ble.h>
#include <zmk/display.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/usb.h>

#include "battery_status.h"

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_DONGLE_BATTERY)
    #define SOURCE_OFFSET 1
#else
    #define SOURCE_OFFSET 0
#endif

#ifndef ZMK_SPLIT_BLE_PERIPHERAL_COUNT
#  define ZMK_SPLIT_BLE_PERIPHERAL_COUNT 0
#endif

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct battery_state {
    uint8_t source;
    uint8_t level;
    bool usb_present;
};

// 캔버스를 제거하고 일반 lv_obj_t (도형)로 대체
struct battery_object {
    lv_obj_t *rect; 
    lv_obj_t *label;
} battery_objects[ZMK_SPLIT_BLE_PERIPHERAL_COUNT + SOURCE_OFFSET];

static void update_battery_visual(int index, uint8_t level, bool usb_present) {
    lv_obj_t *rect = battery_objects[index].rect;
    if (!rect) return;

    // 배터리 잔량에 따라 테두리 색상이나 배경으로 표현
    if (usb_present) {
        lv_obj_set_style_border_side(rect, LV_BORDER_SIDE_FULL, 0);
        lv_obj_set_style_border_width(rect, 1, 0);
        lv_obj_set_style_border_color(rect, lv_color_white(), 0);
    } else {
        lv_obj_set_style_border_width(rect, 0, 0);
        lv_obj_set_style_bg_color(rect, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(rect, LV_OPA_COVER, 0);
    }
}

static void set_battery_symbol(struct battery_state state) {
    if (state.source >= ZMK_SPLIT_BLE_PERIPHERAL_COUNT + SOURCE_OFFSET) return;

    lv_obj_t *label = battery_objects[state.source].label;
    if (label) {
        lv_label_set_text_fmt(label, "%3u%%", state.level);
    }
    update_battery_visual(state.source, state.level, state.usb_present);
}

static void battery_status_update_cb(struct battery_state state) {
    set_battery_symbol(state);
}

static struct battery_state battery_status_get_state(const zmk_event_t *eh) {
    if (as_zmk_peripheral_battery_state_changed(eh) != NULL) {
        const struct zmk_peripheral_battery_state_changed *ev = as_zmk_peripheral_battery_state_changed(eh);
        return (struct battery_state){.source = ev->source + SOURCE_OFFSET, .level = ev->state_of_charge, .usb_present = false};
    }
    
    return (struct battery_state) {
        .source = 0,
        .level = zmk_battery_state_of_charge(),
        .usb_present = zmk_usb_is_powered(),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_dongle_battery_status, struct battery_state,
                            battery_status_update_cb, battery_status_get_state)

ZMK_SUBSCRIPTION(widget_dongle_battery_status, zmk_peripheral_battery_state_changed);
#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_DONGLE_BATTERY)
    ZMK_SUBSCRIPTION(widget_dongle_battery_status, zmk_battery_state_changed);
    ZMK_SUBSCRIPTION(widget_dongle_battery_status, zmk_usb_conn_state_changed);
#endif

int zmk_widget_dongle_battery_status_init(struct zmk_widget_dongle_battery_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, 40, 32); // 배터리 위젯 크기 최적화
    lv_obj_set_style_bg_opa(widget->obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(widget->obj, 0, 0);

    for (int i = 0; i < ZMK_SPLIT_BLE_PERIPHERAL_COUNT + SOURCE_OFFSET; i++) {
        // 캔버스 대신 아주 작은 '바(Bar)' 또는 '사각형' 생성
        lv_obj_t *rect = lv_obj_create(widget->obj);
        lv_obj_set_size(rect, 6, 8); 
        lv_obj_align(rect, LV_ALIGN_TOP_RIGHT, 0, i * 11);
        lv_obj_set_style_bg_color(rect, lv_color_white(), 0);

        lv_obj_t *label = lv_label_create(widget->obj);
        lv_obj_set_style_text_font(label, &lv_font_unscii_8, 0);
        lv_obj_align_to(label, rect, LV_ALIGN_OUT_LEFT_MID, -2, 0);

        battery_objects[i] = (struct battery_object){ .rect = rect, .label = label };
    }

    sys_slist_append(&widgets, &widget->node);
    widget_dongle_battery_status_init();
    return 0;
}

lv_obj_t *zmk_widget_dongle_battery_status_obj(struct zmk_widget_dongle_battery_status *widget) {
    return widget->obj;
}
