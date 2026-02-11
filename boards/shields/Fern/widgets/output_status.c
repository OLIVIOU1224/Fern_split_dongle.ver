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
    lv
