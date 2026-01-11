#include "modifier_indicator.h"

#include <zmk/display.h>
#ifdef CONFIG_DT_HAS_ZMK_BEHAVIOR_CAPS_WORD_ENABLED
#include <zmk/events/caps_word_state_changed.h>
#endif
#include <zmk/events/keycode_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/hid.h>
#include <dt-bindings/zmk/hid_usage.h>
#include <dt-bindings/zmk/hid_usage_pages.h>

#include <fonts.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

static uint32_t color_inactive = 0x333333;
static uint32_t color_active = 0xfcba03;

struct modifier_state {
    bool gui;
    bool alt;
    bool ctrl;
    bool shift;
    bool caps_word;
};

static struct modifier_state current_state = {
    .gui = false,
    .alt = false,
    .ctrl = false,
    .shift = false,
    .caps_word = false,
};

static void set_modifier_color(lv_obj_t *label, bool active) {
    if (active) {
        lv_obj_set_style_text_color(label, lv_color_hex(color_active), LV_PART_MAIN);
    } else {
        lv_obj_set_style_text_color(label, lv_color_hex(color_inactive), LV_PART_MAIN);
    }
}

static void modifier_update_cb(struct modifier_state state) {
    struct zmk_widget_modifier_indicator *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_modifier_color(widget->gui_label, state.gui);
        set_modifier_color(widget->alt_label, state.alt);
        set_modifier_color(widget->ctrl_label, state.ctrl);
        set_modifier_color(widget->shift_label, state.shift);
#ifdef CONFIG_DT_HAS_ZMK_BEHAVIOR_CAPS_WORD_ENABLED
        set_modifier_color(widget->caps_label, state.caps_word);
#endif
    }
}

static void update_modifier_state(uint8_t keycode, bool pressed) {
    bool changed = false;

    switch (keycode) {
        case HID_USAGE_KEY_KEYBOARD_LEFTSHIFT:
        case HID_USAGE_KEY_KEYBOARD_RIGHTSHIFT:
            if (current_state.shift != pressed) {
                current_state.shift = pressed;
                changed = true;
            }
            break;
        case HID_USAGE_KEY_KEYBOARD_LEFTCONTROL:
        case HID_USAGE_KEY_KEYBOARD_RIGHTCONTROL:
            if (current_state.ctrl != pressed) {
                current_state.ctrl = pressed;
                changed = true;
            }
            break;
        case HID_USAGE_KEY_KEYBOARD_LEFTALT:
        case HID_USAGE_KEY_KEYBOARD_RIGHTALT:
            if (current_state.alt != pressed) {
                current_state.alt = pressed;
                changed = true;
            }
            break;
        case HID_USAGE_KEY_KEYBOARD_LEFT_GUI:
        case HID_USAGE_KEY_KEYBOARD_RIGHT_GUI:
            if (current_state.gui != pressed) {
                current_state.gui = pressed;
                changed = true;
            }
            break;
    }

    if (changed) {
        modifier_update_cb(current_state);
    }
}

static int keycode_state_changed_listener(const zmk_event_t *eh) {
    const struct zmk_keycode_state_changed *event = as_zmk_keycode_state_changed(eh);
    if (event && is_mod(event->usage_page, event->keycode)) {
        update_modifier_state(event->keycode, event->state);
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(widget_modifier_indicator_mod, keycode_state_changed_listener);
ZMK_SUBSCRIPTION(widget_modifier_indicator_mod, zmk_keycode_state_changed);

#ifdef CONFIG_DT_HAS_ZMK_BEHAVIOR_CAPS_WORD_ENABLED
static struct modifier_state modifier_indicator_get_state(const zmk_event_t *eh) {
    const struct zmk_caps_word_state_changed *ev = as_zmk_caps_word_state_changed(eh);
    LOG_INF("DISP | Caps Word State Changed: %d", ev->active);
    current_state.caps_word = ev->active;
    return current_state;
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_modifier_indicator, struct modifier_state, modifier_update_cb, modifier_indicator_get_state)
ZMK_SUBSCRIPTION(widget_modifier_indicator, zmk_caps_word_state_changed);
#endif

int zmk_widget_modifier_indicator_init(struct zmk_widget_modifier_indicator *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    // lv_obj_set_style_bg_opa(widget->obj, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(widget->obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(widget->obj, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(widget->obj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(widget->obj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(widget->obj, 4, LV_PART_MAIN);

    widget->gui_label = lv_label_create(widget->obj);
    lv_label_set_text(widget->gui_label, "GUI");
    lv_obj_set_style_text_font(widget->gui_label, &cascadia_latin_ru_fa_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(widget->gui_label, lv_color_hex(color_inactive), LV_PART_MAIN);

    widget->alt_label = lv_label_create(widget->obj);
    lv_label_set_text(widget->alt_label, "ALT");
    lv_obj_set_style_text_font(widget->alt_label, &cascadia_latin_ru_fa_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(widget->alt_label, lv_color_hex(color_inactive), LV_PART_MAIN);

    widget->ctrl_label = lv_label_create(widget->obj);
    lv_label_set_text(widget->ctrl_label, "CTRL");
    lv_obj_set_style_text_font(widget->ctrl_label, &cascadia_latin_ru_fa_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(widget->ctrl_label, lv_color_hex(color_inactive), LV_PART_MAIN);

    widget->shift_label = lv_label_create(widget->obj);
    lv_label_set_text(widget->shift_label, "SHFT");
    lv_obj_set_style_text_font(widget->shift_label, &cascadia_latin_ru_fa_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(widget->shift_label, lv_color_hex(color_inactive), LV_PART_MAIN);

    widget->caps_label = lv_label_create(widget->obj);
    lv_label_set_text(widget->caps_label, "CAPS");
    lv_obj_set_style_text_font(widget->caps_label, &cascadia_latin_ru_fa_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(widget->caps_label, lv_color_hex(color_inactive), LV_PART_MAIN);

    sys_slist_append(&widgets, &widget->node);

    // Initialize modifier state from current HID state
    zmk_mod_flags_t mods = zmk_hid_get_explicit_mods();
    current_state.shift = (mods & (MOD_LSFT | MOD_RSFT)) != 0;
    current_state.ctrl = (mods & (MOD_LCTL | MOD_RCTL)) != 0;
    current_state.alt = (mods & (MOD_LALT | MOD_RALT)) != 0;
    current_state.gui = (mods & (MOD_LGUI | MOD_RGUI)) != 0;

    // Apply initial state to display
    modifier_update_cb(current_state);

#ifdef CONFIG_DT_HAS_ZMK_BEHAVIOR_CAPS_WORD_ENABLED
    widget_modifier_indicator_init();
#endif
    return 0;
}

lv_obj_t *zmk_widget_modifier_indicator_obj(struct zmk_widget_modifier_indicator *widget) {
    return widget->obj;
}
