/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "custom_status_screen.h"
#include "widgets/battery_status.h"
#include "widgets/modifiers.h"
#include "widgets/bongo_cat.h"
#include "widgets/layer_status.h"
#include "widgets/output_status.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static struct zmk_widget_output_status output_status_widget;
static struct zmk_widget_layer_status layer_status_widget;
static struct zmk_widget_dongle_battery_status dongle_battery_status_widget;
static struct zmk_widget_modifiers modifiers_widget;
static struct zmk_widget_bongo_cat bongo_cat_widget;

// 스타일은 static으로 선언하여 메모리 재할당을 방지합니다.
static lv_style_t global_style;
static bool style_initialized = false;

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen;
    screen = lv_obj_create(NULL);

    // 스타일 중복 초기화 방지
    if (!style_initialized) {
        lv_style_init(&global_style);
        lv_style_set_text_font(&global_style, &lv_font_unscii_8);
        lv_style_set_text_letter_space(&global_style, 1);
        lv_style_set_text_line_space(&global_style, 1);
        style_initialized = true;
    }
    
    lv_obj_add_style(screen, &global_style, LV_PART_MAIN);
    
    // 1. 레이어 상태 (좌상단) - 오타 수정됨
    zmk_widget_layer_status_init(&layer_status_widget, screen);
    lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_TOP_LEFT, 0, 0);

    // 2. 출력 상태 (BLE/USB 상태 - 필요시 주석 해제)
    // zmk_widget_output_status_init(&output_status_widget, screen);
    // lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_RIGHT, 0, 0);

    // 3. 봉고캣 (중앙 우측) - 0.91인치 높이를 고려해 y좌표 조정
    zmk_widget_bongo_cat_init(&bongo_cat_widget, screen);
    lv_obj_align(zmk_widget_bongo_cat_obj(&bongo_cat_widget), LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    // 4. 모디파이어 상태 (좌하단)
    zmk_widget_modifiers_init(&modifiers_widget, screen);
    lv_obj_align(zmk_widget_modifiers_obj(&modifiers_widget), LV_ALIGN_BOTTOM_LEFT, 0, 0);

    // 5. 배터리 상태 (우하단)
    zmk_widget_dongle_battery_status_init(&dongle_battery_status_widget, screen);
    lv_obj_align(zmk_widget_dongle_battery_status_obj(&dongle_battery_status_widget), LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    
    return screen;
}
