/*
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#include "custom_status_screen.h"
#include "widgets/battery_status.h"
#include "widgets/output_status.h"
#include "widgets/wpm_status.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static struct zmk_widget_output_status output_status_widget;

#if IS_ENABLED(CONFIG_ZMK_BATTERY)
static struct zmk_widget_dongle_battery_status dongle_battery_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_WPM)
static struct zmk_widget_wpm_status wpm_status_widget;
#endif

lv_style_t global_style;

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen;
    screen = lv_obj_create(NULL);

    // [전략 1] OLED 친화적인 검은색 배경 설정
    lv_style_init(&global_style);
    lv_style_set_bg_color(&global_style, lv_color_black());
    lv_style_set_bg_opa(&global_style, LV_OPA_COVER);
    lv_style_set_text_color(&global_style, lv_color_white());
    lv_style_set_text_font(&global_style, &lv_font_unscii_8);
    lv_obj_add_style(screen, &global_style, LV_PART_MAIN);
    
    // [전략 2] 좌측 상단: 아웃풋 상태 (USB/BT 프로필)
    zmk_widget_output_status_init(&output_status_widget, screen);
    lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_LEFT, 2, 4);

    // [전략 3] 중앙 또는 좌측 하단: WPM 상태
#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_WPM)
    zmk_widget_wpm_status_init(&wpm_status_widget, screen);
    // 아웃풋 위젯과 겹치지 않게 하단에 배치
    lv_obj_align(zmk_widget_wpm_status_obj(&wpm_status_widget), LV_ALIGN_BOTTOM_LEFT, 2, -4);
#endif

    // [전략 4] 우측: 배터리 상태 (가장 중요한 정보)
#if IS_ENABLED(CONFIG_ZMK_BATTERY)
    zmk_widget_dongle_battery_status_init(&dongle_battery_status_widget, screen);
    // 우측 상단에 맞춰 배터리 잔량 표시 (우측 끝에 정렬)
    lv_obj_align(zmk_widget_dongle_battery_status_obj(&dongle_battery_status_widget), LV_ALIGN_TOP_RIGHT, -2, 4);
#endif

    return screen;
}
