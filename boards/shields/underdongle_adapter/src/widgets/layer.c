#include "layer.h"

#include <zmk/display.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>

#include <fonts.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct layer_state {
    uint8_t index;
};

static void layer_update_cb(struct layer_state state) {
    struct zmk_widget_layer *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        const char *layer_name = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(state.index));
        char value[10] = {};
        sprintf(value, "%s", layer_name);
        lv_label_set_text(widget->obj, value);
    }
}

static struct layer_state layer_get_state(const zmk_event_t *eh) {
    uint8_t index = zmk_keymap_highest_layer_active();
    return (struct layer_state){
        .index = index,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer, struct layer_state, layer_update_cb, layer_get_state)
ZMK_SUBSCRIPTION(widget_layer, zmk_layer_state_changed);

int zmk_widget_layer_init(struct zmk_widget_layer *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    lv_obj_set_style_text_font(widget->obj, &cascadia_fa_16, 0);
    lv_obj_set_style_text_color(widget->obj, lv_color_white(), 0);
    lv_obj_align(widget->obj, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(widget->obj, "N/A");

    sys_slist_append(&widgets, &widget->node);

    widget_layer_init();
    return 0;
}

lv_obj_t *zmk_widget_layer_obj(struct zmk_widget_layer *widget) {
    return widget->obj;
}
