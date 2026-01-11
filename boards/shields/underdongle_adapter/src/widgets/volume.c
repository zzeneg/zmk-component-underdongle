#include "volume.h"

#include <zmk/display.h>
#include <zmk/ble.h>
#include <zmk/event_manager.h>
#include <hid.h>
#include <fonts.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

static struct volume_notification get_volume(const zmk_event_t *eh) {
    struct volume_notification *notification = as_volume_notification(eh);
    if (notification) {
        return *notification;
    }
    return (struct volume_notification){.value = 0};
}

static void volume_update_cb(struct volume_notification volume) {
    struct zmk_widget_volume *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        char value[10] = {};
        sprintf(value, LV_SYMBOL_VOLUME_MAX" %i%%", volume.value);
        lv_label_set_text(widget->obj, value);
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_volume, struct volume_notification, volume_update_cb, get_volume)
ZMK_SUBSCRIPTION(widget_volume, volume_notification);

int zmk_widget_volume_init(struct zmk_widget_volume *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    lv_obj_set_style_text_font(widget->obj, &cascadia_fa_16, 0);
    lv_obj_set_style_text_color(widget->obj, lv_color_white(), 0);
    lv_label_set_text(widget->obj, "N/A");

    sys_slist_append(&widgets, &widget->node);

    widget_volume_init();

    return 0;
}

lv_obj_t *zmk_widget_volume_obj(struct zmk_widget_volume *widget) { return widget->obj; }