#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/wpm.h>

#include "bongo_cat.h"

// LVGL 9에서는 명시적 캐스팅이 중요합니다.
#define SRC(array) (const void **)array, (uint16_t)(sizeof(array) / sizeof(lv_img_dsc_t *))

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static int64_t last_anim_update_time = 0;
#define ANIM_UPDATE_INTERVAL_MS 50 

LV_IMG_DECLARE(bongo_cat_none);
LV_IMG_DECLARE(bongo_cat_left1);
LV_IMG_DECLARE(bongo_cat_left2);
LV_IMG_DECLARE(bongo_cat_right1);
LV_IMG_DECLARE(bongo_cat_right2);
LV_IMG_DECLARE(bongo_cat_both1);
LV_IMG_DECLARE(bongo_cat_both1_open);
LV_IMG_DECLARE(bongo_cat_both2);

// 애니메이션 배열 (기존과 동일)
const lv_img_dsc_t *idle_imgs[] = { &bongo_cat_both1_open, &bongo_cat_both1_open, &bongo_cat_both1 };
const lv_img_dsc_t *slow_imgs[] = { &bongo_cat_left1, &bongo_cat_both1, &bongo_cat_right1, &bongo_cat_both1 };
const lv_img_dsc_t *mid_imgs[] = { &bongo_cat_left2, &bongo_cat_none, &bongo_cat_right2, &bongo_cat_none };
const lv_img_dsc_t *fast_imgs[] = { &bongo_cat_both2, &bongo_cat_none };

struct bongo_cat_wpm_status_state { uint8_t wpm; };

static enum anim_state {
    anim_state_none,
    anim_state_idle,
    anim_state_slow,
    anim_state_mid,
    anim_state_fast
} current_anim_state = anim_state_none;

static void set_animation(lv_obj_t *animing, struct bongo_cat_wpm_status_state state) {
    if (animing == NULL) return;

    int64_t now = k_uptime_get();
    if ((now - last_anim_update_time) < ANIM_UPDATE_INTERVAL_MS) return;
    last_anim_update_time = now;

    enum anim_state next_state;
    const void **next_src;
    uint16_t next_count;
    uint32_t next_duration;

    if (state.wpm < 5) {
        next_state = anim_state_idle;
        next_src = (const void **)idle_imgs;
        next_count = sizeof(idle_imgs) / sizeof(lv_img_dsc_t *);
        next_duration = 1000;
    } else if (state.wpm < 30) {
        next_state = anim_state_slow;
        next_src = (const void **)slow_imgs;
        next_count = sizeof(slow_imgs) / sizeof(lv_img_dsc_t *);
        next_duration = 500;
    } else if (state.wpm < 70) {
        next_state = anim_state_mid;
        next_src = (const void **)mid_imgs;
        next_count = sizeof(mid_imgs) / sizeof(lv_img_dsc_t *);
        next_duration = 200;
    } else {
        next_state = anim_state_fast;
        next_src = (const void **)fast_imgs;
        next_count = sizeof(fast_imgs) / sizeof(lv_img_dsc_t *);
        next_duration = 100;
    }

    if (current_anim_state != next_state) {
        lv_animimg_set_src(animing, next_src, next_count);
        lv_animimg_set_duration(animing, next_duration);
        lv_animimg_set_repeat_count(animing, LV_ANIM_REPEAT_INFINITE);
        lv_animimg_start(animing);
        current_anim_state = next_state;
    }
}

struct bongo_cat_wpm_status_state bongo_cat_wpm_status_get_state(const zmk_event_t *eh) {
    struct zmk_wpm_state_changed *ev = as_zmk_wpm_state_changed(eh);
    return (struct bongo_cat_wpm_status_state) { .wpm = ev ? ev->state : 0 };
}

void bongo_cat_wpm_status_update_cb(struct bongo_cat_wpm_status_state state) {
    struct zmk_widget_bongo_cat *widget;
    sys_slist_for_each_container(&widgets, widget, node) {
        set_animation(widget->obj, state);
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_bongo_cat, struct bongo_cat_wpm_status_state,
                            bongo_cat_wpm_status_update_cb, bongo_cat_wpm_status_get_state)
ZMK_SUBSCRIPTION(widget_bongo_cat, zmk_wpm_state_changed);

int zmk_widget_bongo_cat_init(struct zmk_widget_bongo_cat *widget, lv_obj_t *parent) {
    widget->obj = lv_animimg_create(parent);
    
    // 0.91인치 화면에 맞춰 크기를 명시적으로 제한 (중요!)
    lv_obj_set_size(widget->obj, 128, 32); 
    
    // 초기 이미지 설정 (노이즈 방지)
    struct bongo_cat_wpm_status_state initial_state = { .wpm = 0 };
    set_animation(widget->obj, initial_state);

    sys_slist_append(&widgets, &widget->node);
    widget_bongo_cat_init();
    return 0;
}

lv_obj_t *zmk_widget_bongo_cat_obj(struct zmk_widget_bongo_cat *widget) {
    return widget->obj;
}
