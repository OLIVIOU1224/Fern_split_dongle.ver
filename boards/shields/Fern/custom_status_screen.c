#include <zmk/display/status_screen.h>
#include <lvgl.h>

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);
    
    // 화면 전체를 일단 검은색으로 (노이즈 초기화 시도)
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, "CHECKING...");
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // 강제로 갱신을 트리거하기 위한 타이머 역할 (애니메이션 효과)
    lv_obj_set_style_anim_speed(label, 100, 0);

    return screen;
}
