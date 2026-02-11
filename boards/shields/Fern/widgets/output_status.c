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
    if (widget == NULL) { return; }

    lv_obj_t *usb = lv_obj_get_child(widget, 0);
    lv_obj_t *usb_hid_status = lv_obj_get_child(widget, 1);
    lv_obj_t *bt = lv_obj_get_child(widget, 2);
    lv_obj_t *bt_number = lv_obj_get_child(widget, 3);
    lv_obj_t *bt_status = lv_obj_get_child(widget, 4);
    lv_obj_t *selection_line = lv_obj_get_child(widget, 5);

    // USB / BT 선택 표시 바 업데이트
    if (state.selected_endpoint.transport == ZMK_TRANSPORT_USB) {
        lv_obj_set_x(selection_line, 0);
        lv_obj_set_width(selection_line, 12);
    } else {
        lv_obj_set_x(selection_line, 20);
        lv_obj_set_width(selection_line, 20);
    }

    // USB HID 연결 상태
    if (usb_hid_status) {
        lv_img_set_src(usb_hid_status, state.usb_is_hid_ready ? &sym_ok : &sym_nok);
    }

    // Bluetooth 프로필 번호 (0-4 index를 1-5 이미지로 매핑)
    if (bt_number && state.active_profile_index >= 0 && state.active_profile_index < 5) {
        lv_img_set_src(bt_number, sym_num[state.active_profile_index]);
    }

    // Bluetooth 연결 및 본딩 상태
    if (bt_status) {
        if (state.active_profile_bonded) {
            lv_img_set_src(bt_status, state.active_profile_connected ? &sym_ok : &sym_nok);
        } else {
            lv_img_set_src(bt_status, &sym_open);
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
    lv_obj_set_size(widget->obj, 45, 20);
    lv_obj_set_style_bg_opa(widget->obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(widget->obj, 0, 0);
    lv_obj_set_style_pad_all(widget->obj, 0, 0);

    // Child 0: USB Icon
    lv_obj_t *usb = lv_img_create(widget->obj);
    lv_img_set_src(usb, &sym_usb);
    lv_obj_align(usb, LV_ALIGN_TOP_LEFT, 1, 4);

    // Child 1: USB Status (OK/NOK)
    lv_obj_t *usb_hid_status = lv_img_create(widget->obj);
    lv_obj_align(usb_hid_status, LV_ALIGN_TOP_LEFT, 3, 5);

    // Child 2: BT Icon
    lv_obj_t *bt = lv_img_create(widget->obj);
    lv_img_set_src(bt, &sym_bt);
    lv_obj_align(bt, LV_ALIGN_TOP_LEFT, 21, 4);

    // Child 3: BT Number (1-5)
    lv_obj_t *bt_number = lv_img_create(widget->obj);
    lv_obj_align(bt_number, LV_ALIGN_TOP_LEFT, 32, 11);

    // Child 4: BT Status (OK/NOK/Open)
    lv_obj_t *bt_status = lv_img_create(widget->obj);
    lv_obj_align(bt_status, LV_ALIGN_TOP_LEFT, 32, 5);
    
    // Child 5: Selection Bar
    lv_obj_t *selection_line = lv_obj_create(widget->obj);
    lv_obj_set_height(selection_line, 1);
    lv_obj_set_style_bg_color(selection_line, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(selection_line, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(selection_line, 0, 0);
    lv_obj_align(selection_line, LV_ALIGN_TOP_LEFT, 0, 2);
 
    sys_slist_append(&widgets, &widget->node);
    widget_output_status_init();
    
    set_status_symbol(widget->obj, get_state(NULL));
    
    return 0;
}

lv_obj_t *zmk_widget_output_status_obj(struct zmk_widget_output_status *widget) {
    return widget->obj;
}
