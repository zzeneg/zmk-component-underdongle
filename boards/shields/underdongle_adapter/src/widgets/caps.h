#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_caps {
    sys_snode_t node;
    lv_obj_t *obj;
};

int zmk_widget_caps_init(struct zmk_widget_caps *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_caps_obj(struct zmk_widget_caps *widget);