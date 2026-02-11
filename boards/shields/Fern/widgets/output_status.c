/*
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/usb.h>
#include <zmk/endpoints.h>

#include "output_status.h"
#if IS_ENABLED(CONFIG_ZMK_BLE)
#  include <zmk/events/ble_active_profile_changed.h>
#  include <zmk/ble.h>
#endif

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

LV_IMG_DECLARE(sym_usb);
LV_IMG_DECLARE(sym_bt);
LV_IMG_DECLARE(sym_ok);
LV_IMG_DECLARE(sym_nok);
LV_IMG_DECLARE(sym_open);
LV_IMG_DECLARE(sym_1);
LV_IMG_DECLARE(sym_2);
LV_IMG_DECLARE(sym_3);
LV_IMG_DECLARE(sym_4);
LV_IMG_DECLARE(sym_5);

const lv_img_dsc_t *sym_num[] = { &sym_1, &sym_2, &sym_3, &sym_4, &sym_5 };

struct output_status_state {
    struct zmk_endpoint_instance selected_endpoint;
    int active_profile_index;
    bool active_profile_connected;
    bool active_profile_bonded;
    bool usb_is_hid_ready;
};

static struct output_status_state get_state(const zmk_event_t *_eh) {
    struct output_status_state st;
    st.selected_endpoint = zmk_endpoints_selected();
#if IS_ENABLED(CONFIG_ZMK_BLE)
    st.active_profile_index     = zmk_ble_active_profile_index();
    st.active_profile_connected = zmk_ble_active_profile_is_connected();
    st.active_profile_bonded    = !zmk_ble_active_profile_is_open();
#else
    st.active_profile_index     = 0;
    st.active_profile_connected = false;
    st.active_profile_bonded    = false;
#endif
    st.usb_is_hid_ready = zmk_usb_is_hid_ready();
    return st;
}

static void set_status_symbol(lv_obj_t *widget, struct output_status_state state) {
    if (widget == NULL) return;

    // 객체 찾기 (안전을 위해 child 존재 여부 확인 권장하지만 일단 순서대로 접근)
    lv_obj_t *usb_hid_status = lv_obj_get_child(widget, 1);
    lv_obj_t *bt_number = lv_obj_get_child(widget, 3);
    lv_obj_t *bt_status = lv_obj_get_child(widget, 4);
    lv_obj_t *selection_line = lv_obj_get_child(widget, 5);

    // 1. 선택 바 위치 및 가시성 제어 (노이즈 방지를 위해 위치를 확실히 고정)
    if (state.selected_endpoint.transport == ZMK_TRANSPORT_USB) {
        lv_obj_set_x(selection_line, 1);
        lv_obj_set_width(selection_line, 10);
    } else {
        lv_obj_set_x(selection_line, 21);
        lv_obj_set_width(selection_line, 15);
    }

    // 2. USB 상태 (이미지 소스가 NULL이 되지 않도록 주의)
    if (usb_hid_status) {
        lv_img_set_src(usb_hid_status, state.usb_is_hid_ready ? &sym_ok : &sym_nok);
    }

    // 3. BT 번호 및 상태
    if (state.active_profile_index >= 0 && state.active_profile_index < 5) {
        if (bt_number) lv_img_set_src(bt_number, sym_num[state.active_profile_index]);
        if (bt_status) {
            if (state.active_profile_bonded) {
                lv_img_set_src(bt_status, state.active_profile_connected ? &sym_ok : &sym_nok);
            } else {
                lv_img_set_src(bt_status, &sym_open);
            }
        }
    }
}

static void output_status_update_cb(struct output_status_state state) {
    struct zmk_widget_output_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { 
        set_status_symbol(widget->obj, state); 
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_output_status, struct output_status_state,
                            output_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_output_status, zmk_endpoint_changed);
#if IS_ENABLED(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(widget_output_status, zmk_ble_active_profile_changed);
#endif
ZMK_SUBSCRIPTION(widget_output_status, zmk_usb_conn_state_changed);

int zmk_widget_output_status_init(struct zmk_widget_output_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    
    // 크기를 명확히 지정하여 범위를 벗어난 그리기 방지
    lv_obj_set_size(widget->obj, 45, 16); 
    lv_obj_set_style_bg_opa(widget->obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(widget->obj, 0, 0);
    lv_obj_set_style_pad_all(widget->obj, 0, 0);
    // 스크롤바가 노이즈처럼 보일 수 있으므로 제거
    lv_obj_set_scrollbar_mode(widget->obj, LV_SCROLLBAR_MODE_OFF);

    // Child 0: USB Icon
    lv_obj_t *usb = lv_img_create(widget->obj);
    lv_img_set_src(usb, &sym_usb);
    lv_obj_align(usb, LV_ALIGN_TOP_LEFT, 1, 2);

    // Child 1: USB Status
    lv_obj_t *usb_hid_status = lv_img_create(widget->obj);
    lv_obj_align(usb_hid_status, LV_ALIGN_TOP_LEFT, 3, 3);

    // Child 2: BT Icon
    lv_obj_t *bt = lv_img_create(widget->obj);
    lv_img_set_src(bt, &sym_bt);
    lv_obj_align(bt, LV_ALIGN_TOP_LEFT, 21, 2);

    // Child 3: BT Number
    lv_obj_t *bt_number = lv_img_create(widget->obj);
    lv_obj_align(bt_number, LV_ALIGN_TOP_LEFT, 32, 9);

    // Child 4: BT Status
    lv_obj_t *bt_status = lv_img_create(widget->obj);
    lv_obj_align(bt_status, LV_ALIGN_TOP_LEFT, 32, 3);
    
    // Child 5: Selection Bar (선 형태로 단순화)
    lv_obj_t *selection_line = lv_obj_create(widget->obj);
    lv_obj_set_height(selection_line, 1);
    lv_obj_set_style_bg_color(selection_line, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(selection_line, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(selection_line, 0, 0);
    // 화면 맨 상단에 위치
    lv_obj_align(selection_line, LV_ALIGN_TOP_LEFT, 0, 0);
 
    sys_slist_append(&widgets, &widget->node);
    widget_output_status_init();
    set_status_symbol(widget->obj, get_state(NULL));
    
    return 0;
}
lv_obj_t *zmk_widget_output_status_obj(struct zmk_widget_output_status *widget) {
    return widget->obj;
}
