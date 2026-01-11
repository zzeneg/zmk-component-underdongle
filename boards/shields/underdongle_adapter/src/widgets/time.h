#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_time {
    sys_snode_t node;
    lv_obj_t *obj;
};

int zmk_widget_time_init(struct zmk_widget_time *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_time_obj(struct zmk_widget_time *widget);