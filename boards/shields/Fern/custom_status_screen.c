#include <zephyr/kernel.h>
#include <zmk/display/status_screen.h>
#include <lvgl.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/split_obj.h>

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);

    // 왼쪽 배터리 라벨
    lv_obj_t *l_label = lv_label_create(screen);
    lv_label_set_text(l_label, "L");
    lv_obj_align(l_label, LV_ALIGN_TOP_MID, 0, 10);

    // 오른쪽 배터리 라벨
    lv_obj_t *r_label = lv_label_create(screen);
    lv_label_set_text(r_label, "R");
    lv_obj_align(r_label, LV_ALIGN_BOTTOM_MID, 0, -20);

    // 여기에 실제 수치를 띄우는 위젯 로직이 들어가야 하지만, 
    // 우선 화면이 뜨는지 확인하기 위해 텍스트만 배치합니다.
    return screen;
}
