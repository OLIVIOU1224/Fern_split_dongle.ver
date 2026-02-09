#include <zephyr/kernel.h>
#include <zmk/display/status_screen.h>
#include <lvgl.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/ble.h>
#include <zmk/endpoints.h>

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);

    // 왼쪽(L) 배터리 표시용 라벨
    lv_obj_t *l_label = lv_label_create(screen);
    lv_label_set_text(l_label, "L: --%");
    lv_obj_align(l_label, LV_ALIGN_TOP_MID, 0, 10);

    // 오른쪽(R) 배터리 표시용 라벨
    lv_obj_t *r_label = lv_label_create(screen);
    lv_label_set_text(r_label, "R: --%");
    lv_obj_align(r_label, LV_ALIGN_BOTTOM_MID, 0, -10);

    return screen;
}
