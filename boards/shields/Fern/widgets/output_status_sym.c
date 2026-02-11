/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <lvgl.h>

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

/* --- 아이콘 1 (sym_1) --- */
const LV_ATTRIBUTE_MEM_ALIGN uint8_t sym_1_map[] = {
    0xff, 0xff, 0xff, 0xff,    /* Color 0 (White) */
    0x00, 0x00, 0x00, 0xff,    /* Color 1 (Black) */
    0x30, 0x70, 0x70, 0x30, 0x30, 0x30, 
};

const lv_image_dsc_t sym_1 = {
    .header = {
        .magic = 0x19,
        .cf = LV_COLOR_FORMAT_I1,
        .w = 5,
        .h = 6,
        .stride = 1,
    },
    .data_size = 14,
    .data = sym_1_map,
};

/* --- 아이콘 2 (sym_2) --- */
const LV_ATTRIBUTE_MEM_ALIGN uint8_t sym_2_map[] = {
    0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0xff,
    0x70, 0xd8, 0x18, 0x30, 0x60, 0xf8, 
};

const lv_image_dsc_t sym_2 = {
    .header = {
        .magic = 0x19,
        .cf = LV_COLOR_FORMAT_I1,
        .w = 5,
        .h = 6,
        .stride = 1,
    },
    .data_size = 14,
    .data = sym_2_map,
};

/* --- 아이콘 3 (sym_3) --- */
const LV_ATTRIBUTE_MEM_ALIGN uint8_t sym_3_map[] = {
    0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0xff,
    0x70, 0x98, 0x30, 0x18, 0xd8, 0x70, 
};

const lv_image_dsc_t sym_3 = {
    .header = {
        .magic = 0x19,
        .cf = LV_COLOR_FORMAT_I1,
        .w = 5,
        .h = 6,
        .stride = 1,
    },
    .data_size = 14,
    .data = sym_3_map,
};

/* --- 아이콘 4 (sym_4) --- */
const LV_ATTRIBUTE_MEM_ALIGN uint8_t sym_4_map[] = {
    0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0xff,
    0x10, 0x30, 0x70, 0xd0, 0xf8, 0x10, 
};

const lv_image_dsc_t sym_4 = {
    .header = {
        .magic = 0x19,
        .cf = LV_COLOR_FORMAT_I1,
        .w = 5,
        .h = 6,
        .stride = 1,
    },
    .data_size = 14,
    .data = sym_4_map,
};

/* --- 아이콘 5 (sym_5) --- */
const LV_ATTRIBUTE_MEM_ALIGN uint8_t sym_5_map[] = {
    0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0xff,
    0x78, 0x40, 0x70, 0x18, 0xd8, 0x70, 
};

const lv_image_dsc_t sym_5 = {
    .header = {
        .magic = 0x19,
        .cf = LV_COLOR_FORMAT_I1,
        .w = 5,
        .h = 6,
        .stride = 1,
    },
    .data_size = 14,
    .data = sym_5_map,
};

/* --- OK 아이콘 --- */
const LV_ATTRIBUTE_MEM_ALIGN uint8_t sym_ok_map[] = {
    0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0xff,
    0x08, 0x18, 0xb0, 0xe0, 0x40, 
};

const lv_image_dsc_t sym_ok = {
    .header = {
        .magic = 0x19,
        .cf = LV_COLOR_FORMAT_I1,
        .w = 5,
        .h = 5,
        .stride = 1,
    },
    .data_size = 13,
    .data = sym_ok_map,
};

/* --- NOK 아이콘 --- */
const LV_ATTRIBUTE_MEM_ALIGN uint8_t sym_nok_map[] = {
    0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0xff,
    0x88, 0xd8, 0x70, 0xd8, 0x88, 
};

const lv_image_dsc_t sym_nok = {
    .header = {
        .magic = 0x19,
        .cf = LV_COLOR_FORMAT_I1,
        .w = 5,
        .h = 5,
        .stride = 1,
    },
    .data_size = 13,
    .data = sym_nok_map,
};

/* --- OPEN 아이콘 --- */
const LV_ATTRIBUTE_MEM_ALIGN uint8_t sym_open_map[] = {
    0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0xff,
    0x20, 0x70, 0xd8, 0x70, 0x20, 
};

const lv_image_dsc_t sym_open = {
    .header = {
        .magic = 0x19,
        .cf = LV_COLOR_FORMAT_I1,
        .w = 5,
        .h = 5,
        .stride = 1,
    },
    .data_size = 13,
    .data = sym_open_map,
};

/* --- Bluetooth 아이콘 --- */
const LV_ATTRIBUTE_MEM_ALIGN uint8_t sym_bt_map[] = {
    0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0xff,
    0x3e, 0x00, 0x67, 0x00, 0xe3, 0x80, 0xe9, 
    0x80, 0x8c, 0x80, 0xc9, 0x80, 0xe3, 0x80,
    0xe3, 0x80, 0xc9, 0x80, 0x8c, 0x80, 0xe9,
    0x80, 0xe3, 0x80, 0x67, 0x00, 0x3e, 0x00, 
};

const lv_image_dsc_t sym_bt = {
    .header = {
        .magic = 0x19,
        .cf = LV_COLOR_FORMAT_I1,
        .w = 9,
        .h = 14,
        .stride = 2, /* (9+7)/8 = 2 */
    },
    .data_size = 36,
    .data = sym_bt_map,
};

/* --- USB 아이콘 --- */
const LV_ATTRIBUTE_MEM_ALIGN uint8_t sym_usb_map[] = {
    0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0xff,
    0x7f, 0x00, 0x41, 0x00, 0x55, 0x00, 0x41,
    0x00, 0xff, 0x80, 0x80, 0x80, 0x80, 0x80, 
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0xff, 0x80, 
};

const lv_image_dsc_t sym_usb = {
    .header = {
        .magic = 0x19,
        .cf = LV_COLOR_FORMAT_I1,
        .w = 9,
        .h = 14,
        .stride = 2, /* (9+7)/8 = 2 */
    },
    .data_size = 36,
    .data = sym_usb_map,
};
