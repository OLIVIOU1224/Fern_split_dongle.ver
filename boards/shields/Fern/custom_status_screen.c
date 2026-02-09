#include <zmk/display/status_screen.h>
#include <lvgl.h>

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);
    
    // 검은 배경
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *label = lv_label_create(screen);
    // 시스템 폰트 중 가장 큰 것을 사용하도록 유도 (보통 기본 폰트)
    lv_label_set_text(label, "ZMK"); 
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    
    // 화면 하단에 배치 (2/3 노이즈 영역과 대조 확인용)
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);

    return screen;
}
