#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_battery {
    sys_snode_t node;
    lv_obj_t *obj;
    bool initialized;
};

int zmk_widget_battery_init(struct zmk_widget_battery *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_battery_obj(struct zmk_widget_battery *widget);