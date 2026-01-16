#include "layout.h"

#include <zmk/display.h>
#include <zmk/ble.h>
#include <zmk/event_manager.h>
#include <hid.h>
#include <fonts.h>

#define LV_SYMBOL_LANG "\xEF\x84\x9C" /*61724, 0xF11C*/

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

static struct layout_notification get_layout(const zmk_event_t *eh) {
    struct layout_notification *notification = as_layout_notification(eh);
    if (notification) {
        return *notification;
    }

    return (struct layout_notification){.value = 0};
}

static void layout_update_cb(struct layout_notification layout) {
    struct zmk_widget_layout *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        char layouts[sizeof(CONFIG_UNDERDONGLE_LAYOUTS)];
        strcpy(layouts, CONFIG_UNDERDONGLE_LAYOUTS);
        char *current_layout = strtok(layouts, ",");
        size_t i = 0;
        while (current_layout != NULL && i < layout.value) {
            i++;
            current_layout = strtok(NULL, ",");
        }

        if (current_layout != NULL) {
            lv_label_set_text_fmt(widget->obj, LV_SYMBOL_LANG " %s", current_layout);
        } else {
            lv_label_set_text_fmt(widget->obj, LV_SYMBOL_LANG " %i", layout.value);
        }
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layout, struct layout_notification, layout_update_cb, get_layout)
ZMK_SUBSCRIPTION(widget_layout, layout_notification);

int zmk_widget_layout_init(struct zmk_widget_layout *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    lv_obj_set_style_text_font(widget->obj, &cascadia_latin_ru_fa_14, 0);
    lv_obj_set_style_text_color(widget->obj, lv_color_white(), 0);
    lv_label_set_text(widget->obj, "N/A");

    sys_slist_append(&widgets, &widget->node);

    widget_layout_init();

    return 0;
}

lv_obj_t *zmk_widget_layout_obj(struct zmk_widget_layout *widget) { return widget->obj; }