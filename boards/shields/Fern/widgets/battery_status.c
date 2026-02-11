/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/services/bas.h>

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

/* ### 수정: LVGL 9에서는 I1(1비트) 포맷 사용 시 가로 폭을 8의 배수로 정렬하는 것이 안전합니다. ### */
/* (5px 폭이지만 안전하게 8px 분량의 공간 할당) */
#define BUFFER_SIZE 16

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct battery_state {
    uint8_t source;
    uint8_t level;
    bool usb_present;
};

struct battery_object {
    lv_obj_t *symbol;
    lv_obj_t *label;
} battery_objects[ZMK_SPLIT_BLE_PERIPHERAL_COUNT + SOURCE_OFFSET];
    
static lv_color_t battery_image_buffer[ZMK_SPLIT_BLE_PERIPHERAL_COUNT + SOURCE_OFFSET][BUFFER_SIZE];

static void draw_battery(lv_obj_t *canvas, uint8_t level, bool usb_present) {
    /* ### 수정: 그리기 전 캔버스를 깨끗하게 비워 노이즈 방지 ### */
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);
    
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_rect_dsc_t rect_fill_dsc;
    lv_draw_rect_dsc_init(&rect_fill_dsc);
    rect_fill_dsc.bg_color = lv_color_white();

    if (usb_present) {
        rect_fill_dsc.bg_opa = LV_OPA_TRANSP;
        rect_fill_dsc.border_color = lv_color_white();
        rect_fill_dsc.border_width = 1;
    }

    lv_canvas_set_px(canvas, 0, 0, lv_color_white(), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 4, 0, lv_color_white(), LV_OPA_COVER);

    lv_area_t rect_coords;
    bool rect_draw = true;
    
    if (level <= 10 || usb_present) {
        rect_coords = (lv_area_t){1, 2, 3, 6};
    } else if (level <= 30) {
        rect_coords = (lv_area_t){1, 2, 3, 5};
    } else if (level <= 50) {
        rect_coords = (lv_area_t){1, 2, 3, 4};
    } else if (level <= 70) {
        rect_coords = (lv_area_t){1, 2, 3, 3};
    } else if (level <= 90) {
        rect_coords = (lv_area_t){1, 2, 3, 2};
    } else {
        rect_draw = false;
    }

    if (rect_draw) {
        lv_draw_rect(&layer, &rect_fill_dsc, &rect_coords);
    }

    lv_canvas_finish_layer(canvas, &layer);
}

static void draw_battery(lv_obj_t *canvas, uint8_t level, bool usb_present) {
    if (canvas == NULL) return;

    /* 캔버스 전체를 검은색으로 밀어서 잔상/노이즈 제거 */
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);
    
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_rect_dsc_t rect_fill_dsc;
    lv_draw_rect_dsc_init(&rect_fill_dsc);
    rect_fill_dsc.bg_color = lv_color_white();

    if (usb_present) {
        rect_fill_dsc.bg_opa = LV_OPA_TRANSP;
        rect_fill_dsc.border_color = lv_color_white();
        rect_fill_dsc.border_width = 1;
    }

    /* 배터리 헤드 부분 (폭 8픽셀 캔버스 기준 좌표 조정) */
    lv_canvas_set_px(canvas, 0, 0, lv_color_white(), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 4, 0, lv_color_white(), LV_OPA_COVER);

    lv_area_t rect_coords;
    bool rect_draw = true;
    
    // 게이지 그리기 (좌표 1, 2, 3, 6 등)
    if (level <= 10 || usb_present) { rect_coords = (lv_area_t){1, 2, 3, 6}; }
    else if (level <= 30) { rect_coords = (lv_area_t){1, 2, 3, 5}; }
    else if (level <= 50) { rect_coords = (lv_area_t){1, 2, 3, 4}; }
    else if (level <= 70) { rect_coords = (lv_area_t){1, 2, 3, 3}; }
    else if (level <= 90) { rect_coords = (lv_area_t){1, 2, 3, 2}; }
    else { rect_draw = false; }

    if (rect_draw) {
        lv_draw_rect(&layer, &rect_fill_dsc, &rect_coords);
    }

    lv_canvas_finish_layer(canvas, &layer);
}

int zmk_widget_dongle_battery_status_init(struct zmk_widget_dongle_battery_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);

    /* 1. 위젯 크기 명시적 지정 (동글 0.91 OLED 전체 크기) */
    lv_obj_set_size(widget->obj, 128, 32);
    lv_obj_set_style_bg_color(widget->obj, lv_color_black(), 0);
    lv_obj_set_style_pad_all(widget->obj, 0, 0);
    lv_obj_set_style_border_width(widget->obj, 0, 0);

    for (int i = 0; i < ZMK_SPLIT_BLE_PERIPHERAL_COUNT + SOURCE_OFFSET; i++) {
        /* 2. 캔버스 생성 - 폭을 반드시 8의 배수로 설정 (5 -> 8) */
        lv_obj_t *image_canvas = lv_canvas_create(widget->obj);
        lv_canvas_set_buffer(image_canvas, battery_image_buffer[i], 8, 8, LV_COLOR_FORMAT_I1);
        lv_canvas_fill_bg(image_canvas, lv_color_black(), LV_OPA_COVER); // 초기화

        /* 3. 라벨 생성 */
        lv_obj_t *battery_label = lv_label_create(widget->obj);
        lv_label_set_text(battery_label, "---%");

        /* 4. 배치 (중앙 한 줄 현상을 막기 위해 좌표를 넉넉히 벌림) */
        lv_obj_align(image_canvas, LV_ALIGN_TOP_RIGHT, -2, i * 10);
        lv_obj_align_to(battery_label, image_canvas, LV_ALIGN_OUT_LEFT_MID, -2, 0);

        battery_objects[i] = (struct battery_object){
            .symbol = image_canvas,
            .label = battery_label,
        };
    }

    sys_slist_append(&widgets, &widget->node);
    widget_dongle_battery_status_init();

    return 0;
}

lv_obj_t *zmk_widget_dongle_battery_status_obj(struct zmk_widget_dongle_battery_status *widget) {
    return widget->obj;
}
