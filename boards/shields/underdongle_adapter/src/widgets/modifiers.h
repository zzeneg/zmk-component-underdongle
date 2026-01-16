#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_modifiers {
    sys_snode_t node;
    lv_obj_t *obj;
    lv_obj_t *gui_label;
    lv_obj_t *alt_label;
    lv_obj_t *ctrl_label;
    lv_obj_t *shift_label;
};

int zmk_widget_modifiers_init(struct zmk_widget_modifiers *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_modifiers_obj(struct zmk_widget_modifiers *widget);