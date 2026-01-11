#include "time.h"

#include <zmk/display.h>
#include <zmk/ble.h>
#include <zmk/event_manager.h>
#include <hid.h>
#include <fonts.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

static struct time_notification get_time(const zmk_event_t *eh) {
    struct time_notification *notification = as_time_notification(eh);
    if (notification) {
        return *notification;
    }
    return (struct time_notification){.hour = 0, .minute = 0};
}

static void time_update_cb(struct time_notification time) {
    struct zmk_widget_time *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        char value[10] = {};
        sprintf(value, "%02i:%02i", time.hour, time.minute);
        lv_label_set_text(widget->obj, value);
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_time, struct time_notification, time_update_cb, get_time)
ZMK_SUBSCRIPTION(widget_time, time_notification);

int zmk_widget_time_init(struct zmk_widget_time *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    lv_obj_set_style_text_font(widget->obj, &cascadia_digits_28, 0);
    lv_obj_set_style_text_color(widget->obj, lv_color_white(), 0);
    lv_obj_align(widget->obj, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(widget->obj, "N/A");

    sys_slist_append(&widgets, &widget->node);

    widget_time_init();

    return 0;
}

lv_obj_t *zmk_widget_time_obj(struct zmk_widget_time *widget) { return widget->obj; }