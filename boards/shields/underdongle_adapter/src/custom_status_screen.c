#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#include "widgets/layer.h"
#include "widgets/layout.h"
#include "widgets/battery.h"
#include "widgets/modifiers.h"
#include "widgets/time.h"
#include "widgets/volume.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static struct zmk_widget_layer layer_widget;
static struct zmk_widget_layout layout_widget;
static struct zmk_widget_battery battery_widget;
static struct zmk_widget_modifiers modifiers_widget;
static struct zmk_widget_time time_widget;
static struct zmk_widget_volume volume_widget;

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen;
    screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, 255, LV_PART_MAIN);

    zmk_widget_layer_init(&layer_widget, screen);
    lv_obj_align(zmk_widget_layer_obj(&layer_widget), LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    
    zmk_widget_battery_init(&battery_widget, screen);
    lv_obj_align(zmk_widget_battery_obj(&battery_widget), LV_ALIGN_TOP_RIGHT, 0, 0);

    zmk_widget_time_init(&time_widget, screen);
    lv_obj_align(zmk_widget_time_obj(&time_widget), LV_ALIGN_CENTER, 0, 0);

    zmk_widget_modifiers_init(&modifiers_widget, screen);
    lv_obj_align(zmk_widget_modifiers_obj(&modifiers_widget), LV_ALIGN_BOTTOM_MID, 0, 0);

    zmk_widget_layout_init(&layout_widget, screen);
    lv_obj_align(zmk_widget_layout_obj(&layout_widget), LV_ALIGN_BOTTOM_LEFT, 0, 0);

    zmk_widget_volume_init(&volume_widget, screen);
    lv_obj_align(zmk_widget_volume_obj(&volume_widget), LV_ALIGN_TOP_LEFT, 0, 0);

    return screen;
}