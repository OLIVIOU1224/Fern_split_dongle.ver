#include <zmk/display/status_screen.h>
#include <lvgl.h>

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);
    
    // 화면 전체를 검은색 배경으로 채우기 (노이즈 제거 확인용)
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, "FERN DONGLE");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    return screen;
}
