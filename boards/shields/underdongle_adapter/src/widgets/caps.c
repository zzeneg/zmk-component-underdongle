#include "caps.h"

#include <zmk/display.h>
#include <zmk/events/caps_word_state_changed.h>
#include <zmk/event_manager.h>

#include <fonts.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

static uint32_t color_inactive = 0x444444;
static uint32_t color_active = 0xfcba03;

struct modifier_state {
    bool caps_word;
};

static struct modifier_state current_state = {
    .caps_word = false,
};

static void modifier_update_cb(struct modifier_state state) {
    struct zmk_widget_caps *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        if (state.caps_word) {
            lv_obj_set_style_text_color(widget->obj, lv_color_hex(color_active), LV_PART_MAIN);
        } else {
            lv_obj_set_style_text_color(widget->obj, lv_color_hex(color_inactive), LV_PART_MAIN);
        }
    }
}

static struct modifier_state modifiers_get_caps_state(const zmk_event_t *eh) {
    const struct zmk_caps_word_state_changed *ev = as_zmk_caps_word_state_changed(eh);
    current_state.caps_word = ev->active;
    return current_state;
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_caps, struct modifier_state, modifier_update_cb,
                            modifiers_get_caps_state)
ZMK_SUBSCRIPTION(widget_caps, zmk_caps_word_state_changed);

int zmk_widget_caps_init(struct zmk_widget_caps *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    lv_label_set_text(widget->obj, "CAPS");
    lv_obj_set_style_text_font(widget->obj, &cascadia_latin_ru_fa_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(widget->obj, lv_color_hex(color_inactive), LV_PART_MAIN);

    sys_slist_append(&widgets, &widget->node);

    widget_caps_init();
    return 0;
}

lv_obj_t *zmk_widget_caps_obj(struct zmk_widget_caps *widget) { return widget->obj; }
