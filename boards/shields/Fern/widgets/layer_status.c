/*
 * Copyright (c) 2020 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct layer_status_state {
    uint8_t index;
    const char *label;
};

static void set_layer_symbol(lv_obj_t *label, struct layer_status_state state) {
    if (label == NULL) return;

    // 버퍼를 넉넉하게 잡고 안전하게 처리
    char text[16] = {0};

    if (state.label == NULL) {
        snprintf(text, sizeof(text), "L%i", state.index);
    } else {
        snprintf(text, sizeof(text), "%s", state.label);
    }

    lv_label_set_text(label, text);
}

static void layer_status_update_cb(struct layer_status_state state) {
    struct zmk_widget_layer_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { 
        set_layer_symbol(widget->obj, state); 
    }
}

static struct layer_status_state layer_status_get_state(const zmk_event_t *eh) {
    uint8_t index = zmk_keymap_highest_layer_active();
    return (struct layer_status_state) {
        .index = index,
        .label = zmk_keymap_layer_name(index)
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_status, struct layer_status_state, layer_status_update_cb,
                            layer_status_get_state)

ZMK_SUBSCRIPTION(widget_layer_status, zmk_layer_state_changed);

int zmk_widget_layer_status_init(struct zmk_widget_layer_status *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    
    // 노이즈 방지를 위해 스크롤 모드를 일단 끄거나 제한적으로 설정합니다.
    // 만약 이름이 길지 않다면 롱모드를 제거하는 것이 가장 안전합니다.
    lv_obj_set_width(widget->obj, 64); // 고정 폭 설정 (128의 절반 정도)
    lv_label_set_long_mode(widget->obj, LV_LABEL_LONG_DOTS); // 스크롤 대신 '...' 표시 (안전)

    // 텍스트 정렬 설정
    #if defined(CONFIG_ZMK_DONGLE_DISPLAY_LAYER_TEXT_ALIGN)
    if (strcmp(
