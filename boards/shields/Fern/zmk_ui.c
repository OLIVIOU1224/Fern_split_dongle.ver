#include <zephyr/kernel.h>
#include <zmk/display/status_screen.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/display/widgets/output_status.h>
#include <zmk/display/widgets/peripheral_battery_status.h>
#include <lvgl.h>

// 위젯 객체 선언
static struct zmk_widget_layer_status layer_status_widget;
static struct zmk_widget_output_status output_status_widget;
static struct zmk_widget_peripheral_battery_status peripheral_battery_status_widget;

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen;
    screen = lv_obj_create(NULL);

    // 1. 레이어 표시 (맨 위)
    zmk_widget_layer_status_init(&layer_status_widget, screen);
    lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_TOP_MID, 0, 2);

    // 2. 출력/블루투스 상태 (레이어 바로 아래)
    zmk_widget_output_status_init(&output_status_widget, screen);
    lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_MID, 0, 25);

    // 3. 좌/우 배터리 상태 (맨 아래)
    // ZMK의 peripheral_battery_widget은 동글에 연결된 모든 주변장치를 리스트로 보여줍니다.
    zmk_widget_peripheral_battery_status_init(&peripheral_battery_status_widget, screen);
    lv_obj_align(zmk_widget_peripheral_battery_status_obj(&peripheral_battery_status_widget), LV_ALIGN_BOTTOM_MID, 0, -5);

    return screen;
}
