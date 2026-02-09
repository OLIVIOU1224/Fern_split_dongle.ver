#include <zmk/display/status_screen.h>
#include <lvgl.h>

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);
    
    // 배경을 흰색으로 해서 패널 전체가 켜지는지 확인
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *label = lv_label_create(screen);
    // 글자를 위쪽에 배치하여 중앙 줄 노이즈와 겹치지 않게 함
    lv_label_set_text(label, "FERN");
    lv_obj_set_style_text_color(label, lv_color_black(), 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    return screen;
}
