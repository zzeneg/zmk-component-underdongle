#include "battery.h"

#include <zmk/display.h>
#include <zmk/battery.h>
#include <zmk/ble.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/split_central_status_changed.h>
#include <zmk/event_manager.h>

#include <fonts.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct battery_state {
    uint8_t source;
    uint8_t level;
};

struct connection_status {
    uint8_t source;
    bool connected;
};

static void set_battery_state(lv_obj_t *widget_obj, struct battery_state state, bool is_initialized) {
    if (!is_initialized || state.source >= ZMK_SPLIT_BLE_PERIPHERAL_COUNT) {
        return;
    }

    lv_obj_t *info_container = lv_obj_get_child(widget_obj, state.source);
    if (!info_container) {
        return;
    }

    lv_obj_t *num = lv_obj_get_child(info_container, 0);
    if (!num) {
        return;
    }
    lv_label_set_text_fmt(num, LV_SYMBOL_BATTERY_FULL " %d%%", state.level);

    if (state.level < 20) {
        lv_obj_set_style_text_color(num, lv_color_hex(0xFFB802), 0);
    } else {
        lv_obj_set_style_text_color(num, lv_color_hex(0xFFFFFF), 0);
    }
}

static void set_connection_status(lv_obj_t *widget_obj, struct connection_status status, bool is_initialized) {
    if (!is_initialized || status.source >= ZMK_SPLIT_BLE_PERIPHERAL_COUNT) {
        return;
    }

    lv_obj_t *info_container = lv_obj_get_child(widget_obj, status.source);
    if (!info_container) {
        return;
    }

    lv_obj_t *num = lv_obj_get_child(info_container, 0);
    lv_obj_t *nc_num = lv_obj_get_child(info_container, 1);
    if (!num || !nc_num) {
        return;
    }

    // Prevent animation stacking on rapid connection changes
    lv_anim_del(num, NULL);
    lv_anim_del(nc_num, NULL);

    if (status.connected) {
        lv_obj_fade_out(nc_num, 150, 0);
        lv_obj_fade_in(num, 150, 250);
    } else {
        lv_obj_fade_out(num, 150, 0);
        lv_obj_fade_in(nc_num, 150, 250);
    }
}

void battery_state_update_cb(struct battery_state state) {
    struct zmk_widget_battery *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_battery_state(widget->obj, state, widget->initialized);
    }
}

static struct battery_state get_battery_state(const zmk_event_t *eh) {
    if (eh == NULL) {
        return (struct battery_state){.source = 0, .level = 0};
    }

    const struct zmk_peripheral_battery_state_changed *bat_ev = as_zmk_peripheral_battery_state_changed(eh);
    if (bat_ev == NULL) {
        return (struct battery_state){.source = 0, .level = 0};
    }

    return (struct battery_state){.source = bat_ev->source,.level = bat_ev->state_of_charge};
}

void connection_status_update_cb(struct connection_status status) {
    struct zmk_widget_battery *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_connection_status(widget->obj, status, widget->initialized);
    }
}

static struct connection_status get_connection_status(const zmk_event_t *eh) {
    if (eh == NULL) {
        return (struct connection_status){.source = 0, .connected = false};
    }

    const struct zmk_split_central_status_changed *conn_ev = as_zmk_split_central_status_changed(eh);
    if (conn_ev == NULL) {
        return (struct connection_status){.source = 0, .connected = false};
    }

    return (struct connection_status){.source = conn_ev->slot, .connected = conn_ev->connected};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_state, struct battery_state, battery_state_update_cb, get_battery_state);
ZMK_SUBSCRIPTION(widget_battery_state, zmk_peripheral_battery_state_changed);

ZMK_DISPLAY_WIDGET_LISTENER(widget_connection_status, struct connection_status, connection_status_update_cb, get_connection_status);
ZMK_SUBSCRIPTION(widget_connection_status, zmk_split_central_status_changed);

int zmk_widget_battery_init(struct zmk_widget_battery *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    // lv_obj_set_width(widget->obj, lv_pct(100));
    // lv_obj_set_flex_flow(widget->obj, LV_FLEX_FLOW_ROW);
    // lv_obj_set_flex_align(widget->obj, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    // lv_obj_set_style_pad_column(widget->obj, 12, LV_PART_MAIN);
    // lv_obj_set_style_pad_bottom(widget->obj, 12, LV_PART_MAIN);
    // lv_obj_set_style_pad_hor(widget->obj, 16, LV_PART_MAIN);

        for (int i = 0; i < ZMK_SPLIT_BLE_PERIPHERAL_COUNT; i++) {
        lv_obj_t *info_container = lv_obj_create(widget->obj);
        lv_obj_center(info_container);
        lv_obj_set_height(info_container, lv_pct(100));
        // lv_obj_set_flex_grow(info_container, 1);

        lv_obj_t *num = lv_label_create(info_container);
        lv_obj_set_style_text_font(num, &cascadia_latin_ru_fa_14, 0);
        lv_obj_set_style_text_color(num, lv_color_white(), 0);
        lv_obj_set_style_opa(num, 255, 0);
        lv_obj_align(num, LV_ALIGN_CENTER, 0, 0);
        lv_label_set_text(num, "N/A");

        lv_obj_set_style_opa(num, 0, 0);

        lv_obj_t *nc_num = lv_label_create(info_container);
        lv_obj_set_style_text_color(nc_num, lv_color_hex(0xe63030), 0);
        lv_obj_align(nc_num, LV_ALIGN_CENTER, 0, 0);
        lv_label_set_text(nc_num, LV_SYMBOL_CLOSE);
        lv_obj_set_style_opa(nc_num, 255, 0);
    }

    sys_slist_append(&widgets, &widget->node);
    
    widget->initialized = true;
    widget_connection_status_init();
    widget_battery_state_init();

    return 0;
}

lv_obj_t *zmk_widget_battery_obj(struct zmk_widget_battery *widget) { return widget->obj; }